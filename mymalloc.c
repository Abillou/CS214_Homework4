/**
 * Compile this with
 * gcc -o mymallocheader mymalloc.h
 * gcc -o memtest -g -Wall -Wvla -fsanitize=address memtest.c mymalloc.c -lm
 */

#include "mymalloc.h"

void* heap = NULL;
void* end = NULL;
void* next_ptr = &heap;
struct block* freeList;
int fit;

void myinit(int allocAlg) {
  heap = malloc(1024 * 1024);
  fit = allocAlg;
  end = heap + (1024 * 1024);
  freeList = (void*)(heap);
  freeList->header = 1024 * 1024;
  freeList->footer = 0;
  freeList->start = (uintptr_t)heap;
  freeList->next = NULL;
  freeList->prev = NULL;
  next_ptr = heap;
}

void* mymalloc(size_t size) {
  if (size == 0 || size > 1024 * 1024) {
    return NULL;
  }

  switch (fit) {
    case 0: {
      struct block* temp = freeList;
      bool dont_need_pad = 0;
      uintptr_t lastVisited = temp->start;
      while (temp != NULL) {
        if (lastVisited > temp->start) break;
        dont_need_pad = 0;
        if (temp->start % 8 == 0) {
          dont_need_pad = 1;
        }
        if (temp->footer == 0 &&
            temp->header >= size + (8 - (temp->start % 8)) -
                                8 * (dont_need_pad) + sizeof(struct block)) {
          if (temp->start % 8 != 0) {
            allocate_block(
                temp,
                temp->start + (8 - (temp->start % 8)) + sizeof(struct block),
                size + sizeof(struct block));
            return (void*)(temp->start + (8 - (temp->start % 8)) +
                           sizeof(struct block) + sizeof(struct block));
          } else {
            allocate_block(temp, temp->start, size + sizeof(struct block));
            return (void*)(temp->start + sizeof(struct block));
          }
        }
        lastVisited = temp->start;
        temp = temp->next;
      }
      return NULL;
    }

    case 1: {
      struct block* temp = next_ptr;
      bool dont_need_pad = 0;
      uintptr_t lastVisited = temp->start;
      while (temp != NULL) {
        if (lastVisited > temp->start) break;
        dont_need_pad = 0;
        if (temp->start % 8 == 0) {
          dont_need_pad = 1;
        }
        if (temp->footer == 0 &&
            temp->header >= size + (8 - (temp->start % 8)) -
                                8 * (dont_need_pad) + sizeof(struct block)) {
          next_ptr = (void*)temp->start;
          if (temp->start % 8 != 0) {
            allocate_block(
                temp,
                temp->start + (8 - (temp->start % 8)) + sizeof(struct block),
                size + sizeof(struct block));
            return (void*)(temp->start + (8 - (temp->start % 8)) +
                           sizeof(struct block) + sizeof(struct block));
          } else {
            allocate_block(temp, temp->start, size + sizeof(struct block));
            return (void*)(temp->start + sizeof(struct block));
          }
        }
        lastVisited = temp->start;
        temp = temp->next;
      }
      return NULL;
    }

    case 2: {
      struct block* temp = freeList;
      bool dont_need_pad = 0;
      int min = 1024 * 1024;
      struct block* best_block = temp;
      uintptr_t lastVisited = temp->start;
      while (temp != NULL) {
        if (lastVisited > temp->start) return NULL;
        dont_need_pad = 0;
        if (temp->start % 8 == 0) {
          dont_need_pad = 1;
        }
        if (temp->footer == 0 &&
            temp->header >= size + (8 - (temp->start % 8)) -
                                8 * (dont_need_pad) + sizeof(struct block)) {
          if (temp->header < min) {
            best_block = temp;
            min = temp->header;
          }
        }
        lastVisited = temp->start;
        temp = temp->next;
      }
      temp = best_block;
      if (temp->start % 8 != 0) {
        allocate_block(
            temp, temp->start + (8 - (temp->start % 8)) + sizeof(struct block),
            size + sizeof(struct block));
        return (void*)(temp->start + (8 - (temp->start % 8)) +
                       sizeof(struct block) + sizeof(struct block));
      } else {
        allocate_block(temp, temp->start, size + sizeof(struct block));
        return (void*)(temp->start + sizeof(struct block));
      }
      return NULL;
    }
  }
  return NULL;
}
void allocate_block(struct block* block, uintptr_t start, size_t size) {
  if (start == (uintptr_t)heap && start - block->start >= 0) {
    block->header -= size;
    block->start = (uintptr_t)heap + size;
    block = memmove((void*)((uintptr_t)((void*)block) + size), (void*)block,
                    block->header);
    struct block* newBlock = (void*)heap;
    newBlock->header = size;
    newBlock->footer = 1;
    newBlock->start = (uintptr_t)heap;
    newBlock->next = block;
    newBlock->prev = NULL;
    block->prev = newBlock;
    freeList = newBlock;
    if (block->header > 1024 * 1024 && block->next != NULL &&
        block->next->next != NULL) {
      block->next = block->next->next;
    }
  } else if (start - block->start == 0) {
    struct block* next = block->next;
    int totalSize = block->header;
    block->header = size;
    block->start = start;
    block->footer = 1;
    if (totalSize - size != 0) {
      struct block* newBlock =
          (void*)((uintptr_t)((void*)block) + block->header);
      newBlock->header = totalSize - size;
      newBlock->footer = 0;
      newBlock->start = start + size;
      newBlock->prev = block;
      block->next = newBlock;
      newBlock->next = next;
    } else {
      if (next != NULL) block->next = next;
    }
  } else {
    struct block* next = block->next;
    int totalSize = block->header;
    block->header = start - block->start;
    struct block* newBlock = (void*)((uintptr_t)((void*)block) + block->header);
    newBlock->header = size;
    newBlock->footer = 1;
    newBlock->start = start;
    newBlock->prev = block;
    block->next = newBlock;
    if (totalSize - block->header - size != 0) {
      struct block* newNextBlock =
          (void*)((uintptr_t)((void*)newBlock) + newBlock->header);
      newNextBlock->header = totalSize - block->header - size;
      newNextBlock->footer = 0;
      newNextBlock->start = newBlock->start + newBlock->header;
      newNextBlock->prev = newBlock;
      newBlock->next = newNextBlock;
      newNextBlock->next = next;
    } else {
      newBlock->next = next;
    }
  }
}

void myfree(void* ptr) {
  if (ptr == NULL) {
    return;
  }
  if ((uintptr_t)ptr < (uintptr_t)heap || (uintptr_t)ptr > (uintptr_t)end) {
    return;
  }
  struct block* temp = freeList;
  uintptr_t lastVisited = temp->start;
  while (temp != NULL) {
    if (lastVisited > temp->start) return;
    if ((uintptr_t)(temp->start + sizeof(struct block)) == (uintptr_t)ptr) {
      if (temp->footer == 0) {
        return;
      }
      temp->footer = 0;
      if (temp->prev != NULL && temp->prev->footer == 0) {
        // temp = temp->prev;
        // temp->header += temp->next->header;
        // temp->next = temp->next->next;

        temp->prev->header += temp->header;
        temp->prev->next = temp->next;
      }
      if (temp->next != NULL && temp->next->footer == 0) {
        temp->header += temp->next->header;
        temp->next = temp->next->next;
      }
      break;
    }
    lastVisited = temp->start;
    temp = temp->next;
  }
}

void* myrealloc(void* ptr, size_t size) {
  if (ptr == NULL && size == 0) {
    return NULL;
  } else if (ptr == NULL) {
    ptr = mymalloc(size);
    return ptr;
  } else if (size == 0) {
    myfree(ptr);
    return NULL;
  }
  struct block* temp = freeList;
  uintptr_t lastVisited = temp->start;
  while (temp != NULL) {
    if (lastVisited > temp->start) break;
    if ((uintptr_t)(temp->start + sizeof(struct block)) == (uintptr_t)ptr) {
      if (temp->next != NULL && temp->next->footer == 0 &&
          temp->header + temp->next->header >= size) {
        if (temp->next->header != (size - temp->header)) {
          struct block* next = temp->next->next;
          struct block* newBlock = (void*)((void*)temp + temp->header);
          newBlock->header =
              temp->next->header - (size - temp->header) - sizeof(struct block);
          newBlock->footer = 0;
          newBlock->start = (uintptr_t)temp + temp->header;
          newBlock->prev = temp;
          temp->next = newBlock;
          newBlock->next = next;
        }
        temp->header = size + sizeof(struct block);
        return temp;
      }
    }
    lastVisited = temp->start;
    temp = temp->next;
  }
  temp = freeList;
  bool found = false;
  lastVisited = temp->start;
  while (temp != NULL) {
    if (lastVisited > temp->start) break;

    if (temp->start + sizeof(struct block) == (uintptr_t)ptr) {
      found = true;
      break;
    }
    lastVisited = temp->start;
    temp = temp->next;
  }
  if (!found) {
    return NULL;
  }
  if (size < temp->header) {
    return ptr;
  }
  void* new_location = mymalloc(size);
  struct block* new_node = freeList;
  while (new_node != NULL) {
    if (new_node->start + sizeof(struct block) == (uintptr_t)new_location) {
      struct block* next = new_node->next;
      struct block* prev = new_node->prev;
      void* new_address =
          memmove((void*)((uintptr_t)new_location - sizeof(struct block)),
                  (void*)(temp->start), temp->header);
      struct block* new_block = (struct block*)new_address;
      new_block->header = size + sizeof(struct block);
      new_block->start = (uintptr_t)new_location;
      new_block->footer = 1;
      new_block->prev = prev;
      new_block->next = next;
      myfree(ptr);
      break;
    }
    new_node = new_node->next;
  }

  return new_location;
}

void mycleanup() { free(heap); }

double utilization() {
  struct block* temp = freeList;
  double mem_used = 0;
  struct block* last_alloc = NULL;
  uintptr_t lastVisited = temp->start;

  while (temp != NULL) {
    if (lastVisited > temp->start) break;
    if (temp->footer == 1) {
      mem_used += temp->header;
      last_alloc = temp;
    }

    lastVisited = temp->start;
    temp = temp->next;
  }
  double ending = 1;
  if (last_alloc != NULL) {
    ending = (last_alloc->start + last_alloc->header) - (uintptr_t)heap;

  } else {
    printf("(This didn't work as intended!) ");
  }

  return mem_used / ending;
}
