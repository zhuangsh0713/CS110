#include <emmintrin.h> /* header file for the SSE intrinsics */
#include <assert.h>

int **a;
int **b;
int **c;

void naive_add(void) {
    for (int i = 0; i < 1000; ++i) {
        for (int j = 0; j < 40; ++j) {
            c[i][j] = a[i][j] + b[i][j];
        }
    }
}

void simd_add(void) {
    // TODO: implement me!
    for(int i = 0; i < 1000; i++){
        __m128i *a_row = (__m128i*) &a[i];
        __m128i *b_row = (__m128i*) &b[i];
        __m128i *c_row = (__m128i*) &c[i];
        for(int j = 0; j < 10; j++){
            __m128i a_element = _mm_load_si128(a_row+j);
            __m128i b_element = _mm_load_si128(b_row+j);
            __m128i c_element = _mm_add_epi32(a_element, b_element);
            _mm_storeu_si128(c_row+j, c_element);
        }
    }
}

int main(void) {
    a = malloc(1000 * sizeof(int *));
    b = malloc(1000 * sizeof(int *));
    c = malloc(1000 * sizeof(int *));
    for (int i = 0; i < 1000; ++i) {
        a[i] = malloc(40 * sizeof(int));
        b[i] = malloc(40 * sizeof(int));
        c[i] = malloc(40 * sizeof(int));
    }

    for (int i = 0; i < 1000; ++i) {
        for (int j = 0; j < 40; ++j) {
            a[i][j] = i * 40 + j;
            b[i][j] = 2 * (i * 40 - j);
        }
    }

    simd_add();

    for (int i = 0; i < 1000; ++i) {
        for (int j = 0; j < 40; ++j) {
            assert(c[i][j] == (i * 40 + j + 2 * (i * 40 - j)));
        }
    }
    return 0;
}
