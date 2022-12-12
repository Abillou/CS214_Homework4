#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#include "mymalloc.h"

int main(int argc, char* argv[]) {
  int operation_count = 10;  // Do operation_count operations for testing
  // First fit analysis
  myinit(0);
  void** first_addresses = (void**)malloc(sizeof(void*));
  *(first_addresses) = NULL;
  int malloc_count = 0;
  int random_free_index;
  int random_realloc_index;
  time_t t1;
  struct timeval start;
  struct timeval stop;
  srand((unsigned)time(&t1));
  gettimeofday(&start, NULL);
  for (int i = 0; i < operation_count; i++) {
    int operation_type =
        rand() % 3;  // Randomly select a number between 0 and 2
    if (operation_type == 0) {
      size_t random_size = 1 + (rand() % 256);
      void* ptr = mymalloc(random_size);  // malloc a random size from 1 to 256
                                          // bytes and store the address
      malloc_count++;
      first_addresses =
          (void**)realloc(first_addresses, sizeof(void*) * malloc_count);
      *(first_addresses + (malloc_count - 1)) = ptr;
    } else if (operation_type == 1) {
      random_free_index =
          (malloc_count == 0)
              ? 0
              : rand() % malloc_count;  // Avoid dividing by zero
      myfree(*(first_addresses +
               random_free_index));  // free a random address that was already
                                     // malloced, frees a NULL pointer if empty
      *(first_addresses + random_free_index) = NULL;  // make freed pointer NULL
    } else {
      random_realloc_index =
          (malloc_count == 0)
              ? 0
              : rand() % malloc_count;  // Avoid dividing by zero
      void* random_address = *(first_addresses + random_realloc_index);
      size_t random_size = 1 + (rand() % 256);
      void* ptr = myrealloc(
          random_address,
          random_size);  // realloc a random malloced address with size from 1
                         // to 256 bytes and store the address
      for (int i = 0; i < malloc_count; i++) {
        if (*(first_addresses + i) == random_address) {
          *(first_addresses + i) = NULL;
        }
      }
      malloc_count++;
      first_addresses =
          (void**)realloc(first_addresses, sizeof(void*) * malloc_count);
      *(first_addresses + (malloc_count - 1)) = ptr;
    }
  }
  gettimeofday(&stop, NULL);
  double elapsed_time = (double)(stop.tv_usec - start.tv_usec) / 1000000 +
                        (double)(stop.tv_sec - start.tv_sec);
  double operations_per_second = operation_count / elapsed_time;
  printf("First fit throughput: %f ops/sec\n", operations_per_second);
  printf("First fit utilization: %.2f\n", utilization());
  free(first_addresses);
  mycleanup();

  // Next fit analysis
  myinit(1);
  void** next_addresses = (void**)malloc(sizeof(void*));
  *(next_addresses) = NULL;
  malloc_count = 0;
  gettimeofday(&start, NULL);
  for (int i = 0; i < operation_count; i++) {
    int operation_type =
        rand() % 3;  // Randomly select a number between 0 and 2
    if (operation_type == 0) {
      size_t random_size = 1 + (rand() % 256);
      void* ptr = mymalloc(random_size);  // malloc a random size from 1 to 256
                                          // bytes and store the address
      malloc_count++;
      next_addresses =
          (void**)realloc(next_addresses, sizeof(void*) * malloc_count);
      *(next_addresses + (malloc_count - 1)) = ptr;
    } else if (operation_type == 1) {
      random_free_index =
          (malloc_count == 0)
              ? 0
              : rand() % malloc_count;  // Avoid dividing by zero
      myfree(*(next_addresses +
               random_free_index));  // free a random address that was already
                                     // malloced, frees a NULL pointer if empty
      *(next_addresses + random_free_index) = NULL;  // make freed pointer NULL
    } else {
      random_realloc_index =
          (malloc_count == 0)
              ? 0
              : rand() % malloc_count;  // Avoid dividing by zero
      void* ptr = myrealloc(
          *(next_addresses + random_realloc_index),
          1 + (rand() % 256));  // realloc a random malloced address with size
                                // from 1 to 256 bytes and store the address
      malloc_count++;
      next_addresses =
          (void**)realloc(next_addresses, sizeof(void*) * malloc_count);
      *(next_addresses + (malloc_count - 1)) = ptr;
    }
  }
  gettimeofday(&stop, NULL);
  elapsed_time = (double)(stop.tv_usec - start.tv_usec) / 1000000 +
                 (double)(stop.tv_sec - start.tv_sec);
  operations_per_second = operation_count / elapsed_time;
  printf("Next fit throughput: %f ops/sec\n", operations_per_second);
  printf("Next fit utilization: %.2f\n", utilization());
  free(next_addresses);
  mycleanup();

  // Best fit analysis
  myinit(2);
  void** best_addresses = (void**)malloc(sizeof(void*));
  *(best_addresses) = NULL;
  malloc_count = 0;
  gettimeofday(&start, NULL);
  for (int i = 0; i < operation_count; i++) {
    int operation_type =
        rand() % 3;  // Randomly select a number between 0 and 2
    if (operation_type == 0) {
      size_t random_size = 1 + (rand() % 256);
      void* ptr = mymalloc(random_size);  // malloc a random size from 1 to 256
                                          // bytes and store the address
      malloc_count++;
      best_addresses =
          (void**)realloc(best_addresses, sizeof(void*) * malloc_count);
      *(best_addresses + (malloc_count - 1)) = ptr;
    } else if (operation_type == 1) {
      random_free_index =
          (malloc_count == 0)
              ? 0
              : rand() % malloc_count;  // Avoid dividing by zero
      myfree(*(best_addresses +
               random_free_index));  // free a random address that was already
                                     // malloced, frees a NULL pointer if empty
      *(best_addresses + random_free_index) = NULL;  // make freed pointer NULL
    } else {
      random_realloc_index =
          (malloc_count == 0)
              ? 0
              : rand() % malloc_count;  // Avoid dividing by zero
      void* ptr = myrealloc(
          *(best_addresses + random_realloc_index),
          1 + (rand() % 256));  // realloc a random malloced address with size
                                // from 1 to 256 bytes and store the address
      malloc_count++;
      best_addresses =
          (void**)realloc(best_addresses, sizeof(void*) * malloc_count);
      *(best_addresses + (malloc_count - 1)) = ptr;
    }
  }
  gettimeofday(&stop, NULL);
  elapsed_time = (double)(stop.tv_usec - start.tv_usec) / 1000000 +
                 (double)(stop.tv_sec - start.tv_sec);
  operations_per_second = operation_count / elapsed_time;
  printf("Best fit throughput: %f ops/sec\n", operations_per_second);
  printf("Best fit utilization: %.2f\n", utilization());
  free(best_addresses);
  mycleanup();
}