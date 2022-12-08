#include "mymalloc.h"

void* heap = NULL;
int fitType = 0;

void myinit(int allocAlg) {
    heap = malloc(1024 * 1024);
    fitType = allocAlg;    
}

void* mymalloc(size_t size){
    
    if(size == 0){
        return NULL;
    }

    switch(fitType){
        case 0:                        //First Fit
            break;
        
        case 1:                        //Next Fit
            break;

        case 2:                        //Best Fit
            break;
    }
}

void myfree(void* ptr){

}

void* myrealloc(void* ptr, size_t size){
    if(ptr == NULL && size == 0){
        return NULL;
    }

    if(ptr == NULL){
        mymalloc(size);
    }

    if(size == 0){
        myfree(ptr);
        return NULL;
    }

}
void mycleanup(){
    free(heap);
}
