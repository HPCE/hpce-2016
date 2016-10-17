#include <cstdint>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <vector>
#include <algorithm>

#include <io.h>
#include <fcntl.h>

#include <unistd.h>

#include "tbb/parallel_for.h"

void process_frame(
    unsigned levels,
    unsigned width, unsigned height,
    double *frame
)
{
    unsigned n=std::min(width,height);

    for(unsigned i=0; i<n-1; i++){
        for(int j=i; j>=0; j--){
            unsigned x=i;
            unsigned y=i-j;

            //fprintf(stderr, "  %d, %d\n", x,y);

            assert(y*width +x < width*height);



            double fIn = frame[y*width + x];

            double fQuant = round( fIn * levels ) / levels;

            frame[y*width + x] = fQuant;

            double error = fIn - fQuant;
            frame[  y    * width + x+1] += error * 0.4;
            frame[ (y+1) * width + x  ] += error * 0.4;
            frame[ (y+1) * width + x+1] += error * 0.2;
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
    unsigned levels = atoi(argv[3]);

    std::vector<uint8_t> frame(width*height);
    std::vector<double> fFrame(width*height);

    while(1){
        read_frame(STDIN_FILENO, width, height, &frame[0]);
        std::transform(frame.begin(), frame.end(), fFrame.begin(), [](uint8_t x) { return x * (1.0/256); } );

        process_frame(levels, width, height, &fFrame[0]);

        std::transform(fFrame.begin(), fFrame.end(), frame.begin(), [](double x) { return (uint8_t)std::max(0.0, std::min(255.0, x*256)); } );
        write_frame(width, height, &frame[0], STDOUT_FILENO);
    }
}
