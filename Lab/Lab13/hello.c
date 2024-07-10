#include <omp.h>
#include <stdio.h>

int main() {
  // omp_set_num_threads(5);
#pragma omp parallel
  {
    int thread_ID = omp_get_thread_num();
    printf("hello world %d\n", thread_ID);
  }
  return 0;
}
