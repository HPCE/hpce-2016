#ifndef gcd_hpp
#define gcd_hpp

// https://gist.github.com/cslarsen/1635213
uint64_t gcd(uint64_t u, uint64_t v)
{
  int shl = 0;

  while ( u && v && u!=v ) {
    bool eu = !(u & 1);
    bool ev = !(v & 1);

    if ( eu && ev ) {
      ++shl;
      u >>= 1;
      v >>= 1;
    }
    else if ( eu && !ev ) u >>= 1;
    else if ( !eu && ev ) v >>= 1;
    else if ( u>=v ) u = (u-v)>>1;
    else {
      int tmp = u;
      u = (v-u)>>1;
      v = tmp;
    }
  }

  return (!u) ? (v<<shl) : (u<<shl);
}

#endif
