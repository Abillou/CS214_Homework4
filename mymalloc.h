#define _GNU_SOURCE
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void myinit(int allocAlg);
void* mymalloc(size_t size);
void myfree(void* ptr);
void* myrealloc(void* ptr, size_t size);
void mycleanup();
double utilization();

struct block {
  int header;
  int footer;
  uintptr_t start;
  struct block* next;  // this is for the linked
  struct block* prev;
};

void allocate_block(struct block* block, uintptr_t start, size_t size);