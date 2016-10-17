#include <cstdint>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <vector>

#include <io.h>
#include <fcntl.h>

#include <unistd.h>

#include "tbb/parallel_for.h"

void process_frame(
    float gamma,
    unsigned width, unsigned height,
    const uint8_t *frameIn,
    uint8_t *frameOut
)
{
    assert( gamma > 0 );

    for(unsigned x=0; x<width; x++){
        for(unsigned y=0; y<height; y++){
            double fIn = frameIn[y*width + x] * (1.0/256.0);

            double fOut = pow( fIn, gamma );

            frameOut[y*width + x] = (uint8_t)floor(fOut * 256.0);
        }
    }
}


void read_frame(
    int srcFile, // Handle to the source file
    unsigned width, unsigned height,
    uint8_t *pixels
)
{
    auto todo=width*height;

    while(todo>0){
        auto got = read( srcFile, pixels, todo );
        if(got<0){
            fprintf(stderr, "Error while reading frame.\n");
            exit(1); // Exit with error
        }
        if(got==0){
            exit(0); // end of stream
        }

        pixels += got;  // Advance the pointer to the next location
        todo -= got;    // Record that we got the pixels
    }
}

void write_frame(
    unsigned width, unsigned height,
    const uint8_t *pixels,
    int dstFile // Handle to the destination file
)
{
    auto todo=width*height;

    while(todo>0){
        auto got = write( dstFile, pixels, todo );
        if(got<=0){
            fprintf(stderr, "Error while writing frame, todo=%d, got=%d.\n", todo, got);
            exit(1); // Exit with error
        }

        pixels += got;  // Advance the pointer to the next location
        todo -= got;    // Record that we got the pixels
    }
}

int main(int argc, char *argv[])
{
    #ifdef  __MINGW64__
    _setmode(_fileno(stdin), _O_BINARY);
    _setmode(_fileno(stdout), _O_BINARY);
    #endif

    if(argc!=4){
        fprintf(stderr, "Usage : %s width height gamma\n", argv[0]);
        exit(1);
    }

    unsigned width = atoi(argv[1]);
    unsigned height = atoi(argv[2]);
    double gamma = strtod(argv[3],0);

    std::vector<uint8_t> input(width*height), output(width*height);

    while(1){
        read_frame(STDIN_FILENO, width, height, &input[0]);

        process_frame(gamma, width, height, &input[0], &output[0]);

        write_frame(width, height, &output[0], STDOUT_FILENO);
    }
}
