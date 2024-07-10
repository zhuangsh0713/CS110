#include <emmintrin.h> /* header file for the SSE intrinsics */
#include <time.h>
#include <stdio.h>

float **a;
float **b;
float **c;
float **c2;

int n = 40000;
int p = 20000;

void init(void) {
    a = malloc(n * sizeof(float *));
    b = malloc(4 * sizeof(float *));
    c = malloc(n * sizeof(float *));
    c2 = malloc(n * sizeof(float *));
    for (int i = 0; i < n; ++i) {
        a[i] = malloc(4 * sizeof(float));
        c[i] = malloc(p * sizeof(float));
        c2[i] = malloc(p * sizeof(float));
    }
    for (int i = 0; i < 4; ++i) {
        b[i] = malloc(p * sizeof(float));
    }

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < 4; ++j) {
            a[i][j] = (float) rand() / (float) RAND_MAX;
        }
    }

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < p; ++j) {
            b[i][j] = (float) rand() / (float) RAND_MAX;
        }
    }
}

void check_correctness(char *msg) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < p; ++j) {
            if (c[i][j] != c2[i][j]) {
                printf("%s incorrect!\n", msg);
                return;
            }
        }
    }
}

void naive_matmul(void) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    // c = a * b
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < p; ++j) {
            c[i][j] = 0;
            for (int k = 0; k < 4; ++k) {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("naive: %f\n", (float) (end.tv_sec - start.tv_sec) + (float) (end.tv_nsec - start.tv_nsec) / 1000000000.0f);
}

void loop_unroll_matmul(void) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    // c2 = a * b
    // TODO: implement me!
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < p; j+=4) {
            c2[i][j] = 0;
            c2[i][j+1] = 0;
            c2[i][j+2] = 0;
            c2[i][j+3] = 0;
            for (int k = 0; k < 4; ++k) {
                c2[i][j] += a[i][k] * b[k][j];
                c2[i][j+1] += a[i][k] * b[k][j+1];
                c2[i][j+2] += a[i][k] * b[k][j+2];
                c2[i][j+3] += a[i][k] * b[k][j+3];
            }
        }
    }

    // 这里可以展开k，c2[i][j] = a[i][k] * b[k][j] + a[i][k+1] * b[k+1][j] + a[i][k+2] * b[k+2][j] + a[i][k+3] * b[k+3][j]  ;


    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("unroll: %f\n", (float) (end.tv_sec - start.tv_sec) + (float) (end.tv_nsec - start.tv_nsec) / 1000000000.0f);
    check_correctness("loop_unroll_matmul");
}

void simd_matmul(void) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    // c2 = a * b
    // TODO: implement me!

    for(int i = 0; i < n; i++){
        for(int j = 0; j < p; j+=4){
            __m128 c_element = _mm_setzero_ps() ;
            for(int k = 0; k < 4; k++){
                __m128 a_element = _mm_set1_ps(a[i][k]) ;
                __m128 b_element = _mm_loadu_ps(&b[k][j]) ;
                c_element = _mm_add_ps(c_element, _mm_mul_ps(a_element, b_element)) ;
            }
            _mm_storeu_ps(&c2[i][j], c_element);
        }
    }


    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("simd: %f\n", (float) (end.tv_sec - start.tv_sec) + (float) (end.tv_nsec - start.tv_nsec) / 1000000000.0f);
    check_correctness("simd_matmul");
}

void loop_unroll_simd_matmul(void) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    // c2 = a * b
    // TODO: implement me!

    for(int i = 0; i < n; i++){
        for(int j = 0; j < p; j+=16){
            __m128 c_element_0 = _mm_setzero_ps() ;
            __m128 c_element_1 = _mm_setzero_ps() ;
            __m128 c_element_2 = _mm_setzero_ps() ;
            __m128 c_element_3 = _mm_setzero_ps() ;
            for(int k = 0; k < 4; k++){
                __m128 a_element_0 = _mm_set1_ps(a[i][k]) ;
                __m128 b_element_0 = _mm_loadu_ps(&b[k][j]) ;
                c_element_0 = _mm_add_ps(c_element_0, _mm_mul_ps(a_element_0, b_element_0)) ;

                __m128 a_element_1 = _mm_set1_ps(a[i][k]) ;
                __m128 b_element_1 = _mm_loadu_ps(&b[k][j+4]) ;
                c_element_1 = _mm_add_ps(c_element_1, _mm_mul_ps(a_element_1, b_element_1)) ;

                __m128 a_element_2 = _mm_set1_ps(a[i][k]) ;
                __m128 b_element_2 = _mm_loadu_ps(&b[k][j+8]) ;
                c_element_2 = _mm_add_ps(c_element_2, _mm_mul_ps(a_element_2, b_element_2)) ;

                __m128 a_element_3 = _mm_set1_ps(a[i][k]) ;
                __m128 b_element_3 = _mm_loadu_ps(&b[k][j+12]) ;
                c_element_3 = _mm_add_ps(c_element_3, _mm_mul_ps(a_element_3, b_element_3)) ;
            }
            _mm_storeu_ps(&c2[i][j], c_element_0);
            _mm_storeu_ps(&c2[i][j+4], c_element_1);
            _mm_storeu_ps(&c2[i][j+8], c_element_2);
            _mm_storeu_ps(&c2[i][j+12], c_element_3);
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("unroll+simd: %f\n", (float) (end.tv_sec - start.tv_sec) + (float) (end.tv_nsec - start.tv_nsec) / 1000000000.0f);
    check_correctness("loop_unroll_simd_matmul");
}


int main(void) {
    init();

    naive_matmul();
    simd_matmul();
    loop_unroll_matmul();
    loop_unroll_simd_matmul();

    return 0;
}
