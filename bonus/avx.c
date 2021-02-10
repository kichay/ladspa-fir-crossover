#include <immintrin.h>

__m256 a;
__m256 b;
__m256 c;

void main(int argc, char * argv) {
    c = _mm256_mul_ps (a, b);
}
