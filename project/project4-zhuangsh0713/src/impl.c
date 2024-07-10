#include <omp.h>
#include <stdlib.h>
#include <string.h>
#include <immintrin.h>

void impl(int N, int step, double *p) {
  double *p_next = (double *)calloc(N * N, sizeof(double));
  memcpy(p_next, p, N * N * sizeof(double));

  for (int k = 0; k < step; k++) {
#pragma omp parallel for
    for (int i = 1; i < N - 1; i++) {
      for (int j = 1; j < N - 1; j += 4) { // Unroll the loop by a factor of 4
        if (j + 3 < N - 1) { // Ensure we don't go out of bounds
          p_next[i * N + j] = (p[(i - 1) * N + j] + p[(i + 1) * N + j] + p[i * N + j + 1] + p[i * N + j - 1]) / 4.0;
          p_next[i * N + j + 1] = (p[(i - 1) * N + j + 1] + p[(i + 1) * N + j + 1] + p[i * N + j + 2] + p[i * N + j]) / 4.0;
          p_next[i * N + j + 2] = (p[(i - 1) * N + j + 2] + p[(i + 1) * N + j + 2] + p[i * N + j + 3] + p[i * N + j + 1]) / 4.0;
          p_next[i * N + j + 3] = (p[(i - 1) * N + j + 3] + p[(i + 1) * N + j + 3] + p[i * N + j + 4] + p[i * N + j + 2]) / 4.0;
        } else {
          // Handle the remaining elements without going out of bounds
          for (int jj = j; jj < N - 1; jj++) {
            p_next[i * N + jj] = (p[(i - 1) * N + jj] + p[(i + 1) * N + jj] + p[i * N + jj + 1] + p[i * N + jj - 1]) / 4.0;
          }
        }
      }
    }

    double *temp = p;
    p = p_next;
    p_next = temp;
  }

  free(p_next); // Ensure we free the correct pointer
}
