#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define MATRIX_SIZE 1024

void matmul_naive(double *a, double *b, double *c) {
  for (int i = 0; i < MATRIX_SIZE; i++) {
    for (int j = 0; j < MATRIX_SIZE; j++) {
      for (int k = 0; k < MATRIX_SIZE; k++) {
        c[i * MATRIX_SIZE + j] +=
            a[i * MATRIX_SIZE + k] * b[k * MATRIX_SIZE + j];
      }
    }
  }
}

void matmul_optimized_slices(double *a, double *b, double *c) {
  
  #pragma omp parallel
  {
    int threads = omp_get_num_threads();
    int id = omp_get_thread_num();
    for(int i = id; i < MATRIX_SIZE; i += threads){
      for(int j = 0; j < MATRIX_SIZE; j++){
        double sum = 0;
        for(int k = 0; k < MATRIX_SIZE; k++){
          sum += a[i * MATRIX_SIZE + k] * b[k * MATRIX_SIZE + j];
        }
        c[i * MATRIX_SIZE + j] = sum;
      }
    }
  }
}

void matmul_optimized_chunks(double *a, double *b, double *c) {
  
  #pragma omp parallel
  {
    int threads = omp_get_num_threads();
    int chunk_size = MATRIX_SIZE / threads;
    int id = omp_get_thread_num();
    int begin_row = id * chunk_size;
    
    for(int i = 0; i < chunk_size; i++){
      int current_row = begin_row + i;
      if (current_row >= MATRIX_SIZE) {
        break;
      }
        for(int j = 0; j < MATRIX_SIZE; j++){
          double sum = 0;
          for(int k = 0; k < MATRIX_SIZE; k++){
            sum += a[current_row * MATRIX_SIZE + k] * b[k * MATRIX_SIZE + j];
          }
          c[current_row * MATRIX_SIZE + j] = sum;
        }
      
    }
  }
}


void matmul_optimized(double *a, double *b, double *c) {
  #pragma omp parallel
  {
    int threads = omp_get_num_threads();
    int id = omp_get_thread_num();
    for(int i = id; i < MATRIX_SIZE; i += threads){
      for(int j = 0; j < MATRIX_SIZE; j++){
        double sum = 0;
        for(int k = 0; k < MATRIX_SIZE; k+=4){
          sum += a[i * MATRIX_SIZE + k] * b[k * MATRIX_SIZE + j];
          sum += a[i * MATRIX_SIZE + k+1] * b[(k+1) * MATRIX_SIZE + j];
          sum += a[i * MATRIX_SIZE + k+2] * b[(k+2) * MATRIX_SIZE + j];
          sum += a[i * MATRIX_SIZE + k+3] * b[(k+3) * MATRIX_SIZE + j];
        }
        c[i * MATRIX_SIZE + j] = sum;
      }
    }
  }
}

int main() {
  double *a = (double *)malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(double));
  double *b = (double *)malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(double));
  double *c = (double *)calloc(MATRIX_SIZE * MATRIX_SIZE, sizeof(double));

  if (!a || !b || !c) {
    printf("Error: could not allocate memory.\n");
    return 1;
  }

  for (int i = 0; i < MATRIX_SIZE; i++) {
    for (int j = 0; j < MATRIX_SIZE; j++) {
      a[i * MATRIX_SIZE + j] = (double)rand() / RAND_MAX;
      b[i * MATRIX_SIZE + j] = (double)rand() / RAND_MAX;
    }
  }

  double start_time = omp_get_wtime();
  matmul_naive(a, b, c);
  double end_time = omp_get_wtime();
  printf("Naive: %.5f seconds\n", end_time - start_time);

  start_time = omp_get_wtime();
  matmul_optimized_slices(a, b, c);
  end_time = omp_get_wtime();
  printf("Optimized (slices): %.5f seconds\n", end_time - start_time);

  start_time = omp_get_wtime();
  matmul_optimized_chunks(a, b, c);
  end_time = omp_get_wtime();
  printf("Optimized (chunks): %.5f seconds\n", end_time - start_time);

  start_time = omp_get_wtime();
  matmul_optimized(a, b, c);
  end_time = omp_get_wtime();
  printf("Optimized (slices + unroll loop): %.5f seconds\n", end_time - start_time);

  free(a);
  free(b);
  free(c);

  return 0;
}
