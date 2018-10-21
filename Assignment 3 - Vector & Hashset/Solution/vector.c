#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

const int DEFAULT_LOGICAL_LENGTH = 0;
const int DEFAULT_ALLOC_LENGTH = 4;
const int RESIZE = 2;

/*
 * Grows vector by "const int RESIZE" value    
 */
 void growVector(vector* v){
    v -> allocLength *= RESIZE;
    v -> array = realloc(v -> array, v -> allocLength * v -> elemSize);
    assert(v -> array != NULL); 
}
void VectorNew(vector *v, int elemSize, VectorFreeFunction freeFn, int initialAllocation)
{
    assert(elemSize > 0); //Check that size of the element is more than zero
    v -> elemSize = elemSize;
    v -> freeFn = freeFn;
    assert(initialAllocation >= 0); 
    if(initialAllocation == 0) initialAllocation = DEFAULT_ALLOC_LENGTH; //Give to alloc size default value
    v -> allocLength = initialAllocation;
    v -> logicalLength = DEFAULT_LOGICAL_LENGTH;
    
    v -> array = malloc(v -> elemSize * v -> allocLength); //Make space in heap
    assert(v -> array != NULL);
}

void VectorDispose(vector *v)
{
    if(v -> freeFn == NULL) return; //If vector doesn't requires free function just return
    //Go through vector and free all elements
    for(int i = 0; i < v -> logicalLength; i++){
        void* toDelete = (char*) v -> array + v -> elemSize * i;
        v -> freeFn(toDelete    );
    }
    free(v -> array);
}

int VectorLength(const vector *v)
{ return v -> logicalLength; }

void *VectorNth(const vector *v, int position)
{ 
    assert(position >=0 && position < v -> logicalLength); //Check if position is in bounds
    void* address = (char*)v -> array + v -> elemSize * position;
    return address; 
}

void VectorReplace(vector *v, const void *elemAddr, int position)
{
    assert(position >= 0 && position < v -> logicalLength); //Check if position is in bounds
    void* currentAddress = VectorNth(v, position); //Find address of element of given position
    if(v -> freeFn != NULL){
        v -> freeFn(currentAddress);
    }
    memcpy(currentAddress, elemAddr, v -> elemSize);
}



void VectorInsert(vector *v, const void *elemAddr, int position)
{
    assert(position >=0 && position <= v -> logicalLength); //Check if position is in bounds
    //Increase capacity
    if(v -> logicalLength == v -> allocLength){
        growVector(v);      
    }
    //Move elements
    for(int i = v -> logicalLength; i > position; i--){
        void* source = (char*)v -> array + v -> elemSize * (i - 1);
        void* destination = (char*)v -> array + v -> elemSize * i;
        memcpy(destination, source, v -> elemSize);
    }
    //Insert current element at position 
    memcpy((char*)v -> array + v -> elemSize * position, elemAddr, v -> elemSize);
    //Increase number of elements
    v -> logicalLength++;
}

void VectorAppend(vector *v, const void *elemAddr)
{
    VectorInsert(v, elemAddr, v -> logicalLength);
}

void VectorDelete(vector *v, int position)
{
    assert(position >=0 && position < v -> logicalLength); //Check if position is in bounds   
    if(v -> freeFn != NULL){
        v -> freeFn(VectorNth(v, position));
    }
    //Move elements
    for(int i = position; i < v -> logicalLength - 1; i++){
        void* source = (char*)v -> array + v -> elemSize * (i + 1);
        void* destination = (char*)v -> array + v -> elemSize * i;
        memcpy(destination, source, v -> elemSize);
    }
    //Decrease number of elements
    v -> logicalLength--;
}

void VectorSort(vector *v, VectorCompareFunction compare)
{
   assert(compare != NULL);
   void* base = v -> array;
   int numOfElems = v -> logicalLength;
   int elemSize = v -> elemSize;
   qsort(base, numOfElems, elemSize, compare);
}

void VectorMap(vector *v, VectorMapFunction mapFn, void *auxData)
{
    assert(mapFn != NULL);
    for(int i = 0; i < v -> logicalLength; i++){
        void* elemAddr = VectorNth(v, i);
        mapFn(elemAddr, auxData);
    }
}

static const int kNotFound = -1;
int VectorSearch(const vector *v, const void *key, VectorCompareFunction searchFn, int startIndex, bool isSorted)
{ 
    assert(startIndex >= 0 && startIndex <= v -> logicalLength);
    assert(searchFn != NULL && key != NULL);
    //If is sorted
    if(isSorted){
        void* base = VectorNth(v, startIndex);
        int numOfElems = v -> logicalLength - startIndex;
        void* address = bsearch(key, base, numOfElems, v -> elemSize, searchFn);

        if(address == NULL) return kNotFound;
        
        int index = ((char*)address - (char*)v -> array) / v -> elemSize;
        return index;   
    } else {
        for(int i = startIndex; i < v -> logicalLength; i++){
            void* currentElem = VectorNth(v, i);
            if(searchFn(key, currentElem) == 0){
                return i;
            }
        }
        return kNotFound;
    }
    return kNotFound;
} 
