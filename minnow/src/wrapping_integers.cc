#include "wrapping_integers.hh"
#include<iostream>
#include<limits>

using namespace std;

Wrap32 Wrap32::wrap(uint64_t n, Wrap32 zero_point) {
    // Your code here.
    // (void)n;
    // (void)zero_point;
    // return Wrap32 { 0 };
    uint32_t const res = (n % 4294967296 + zero_point.raw_value_) % 4294967296;
    Wrap32 wrap32(res);
    return wrap32;
}

uint64_t Wrap32::unwrap(Wrap32 zero_point, uint64_t checkpoint) const {
    // Your code here.
    // (void) zero_point;
    // (void) checkpoint;
    // return {};
    uint32_t const offset = raw_value_ - wrap(checkpoint, zero_point).raw_value_;
    uint64_t result = checkpoint + offset;
    if (offset > (1U << 31) && result >= (1UL << 32)) {
        result -= (1UL << 32);
    }
    return result;
}

uint64_t distance(uint64_t a, uint64_t b) {
    if (a >= b) {
        return a - b;
    }
    return b - a;
}
