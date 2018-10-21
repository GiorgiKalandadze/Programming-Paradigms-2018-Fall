#include "hashset.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

const int DEFAULT_INITIAL_ALLOCATION = 4;
const int DEFAULT_LOG_LENGTH = 0;
const int DEFAULT_START_POSITION;
const bool DEFAULT_SORTED_CONDITION = false;
void HashSetNew(hashset *h, int elemSize, int numBuckets,
		HashSetHashFunction hashfn, HashSetCompareFunction comparefn, HashSetFreeFunction freefn)
{
	h -> set = malloc(numBuckets * sizeof(vector));	
	assert(elemSize > 0); //Check if elemSize is nonnegative
	h -> elemSize = sizeof(vector);
	assert(numBuckets > 0); //Check if numBuckets is nonnegative
	h -> size = numBuckets;
	h -> numOfElems = DEFAULT_LOG_LENGTH;
	//An assert is raised if either the hash or compare functions are NULL.
	assert(hashfn != NULL);
	h -> hashFn = hashfn;
	assert(comparefn != NULL);
	h -> compareFn = comparefn;
	h -> freeFn = freefn;

	for(int i = 0; i < h -> size; i++){
		vector* v = (char*)h -> set + i * h -> elemSize; 
		VectorNew(v, elemSize, h -> freeFn, DEFAULT_INITIAL_ALLOCATION);
	}
}

void HashSetDispose(hashset *h)
{
	if(h -> freeFn == NULL) return; //If free function doens't exists return
	//Free, set's each element's(vector's), elements
	for(int i = 0; i < h -> size; i++){
		vector* current = (char*)h -> set + i * h -> elemSize;
		VectorDispose(current);
	}
	//Free set
	free(h -> set);
}

int HashSetCount(const hashset *h)
{ return h -> elemSize; }

void HashSetMap(hashset *h, HashSetMapFunction mapfn, void *auxData)
{
	assert(mapfn != NULL);
    for(int i = 0; i < h -> size; i++){
        vector* v = (char*)h -> set + i * h -> elemSize;
		VectorMap(v, mapfn, auxData); 
	}
}

void HashSetEnter(hashset *h, const void *elemAddr)
{
	assert(elemAddr != NULL);
	int indexOfBucket = h -> hashFn(elemAddr, h -> size); //Index of bucket(vector)
	assert(indexOfBucket >= 0 && indexOfBucket < h -> size);

	vector* v = (char*)h -> set + indexOfBucket * h -> elemSize;
	//Check if element already exists
	int position = VectorSearch(v, elemAddr, h -> compareFn, DEFAULT_START_POSITION, DEFAULT_SORTED_CONDITION);

	if(position == -1){ //If element doesn't exists add it at the and of the bucket
		VectorAppend(v, elemAddr);
		h -> numOfElems++;
	} else { //If element exists replace it
		VectorReplace(v, elemAddr, position);
	}
}

void *HashSetLookup(const hashset *h, const void *elemAddr)
{ 
	assert(elemAddr != NULL);
	int indexOfBucket = h -> hashFn(elemAddr, h -> size);
	assert(indexOfBucket >= 0 && indexOfBucket < h -> size);
	
	vector* v = (char*)h -> set + indexOfBucket * h -> elemSize;
	//Check if element exists
	int position = VectorSearch(v, elemAddr, h -> compareFn, DEFAULT_START_POSITION, DEFAULT_SORTED_CONDITION);
	if(position == -1){
		return NULL;
	} else {
		return VectorNth(v, position);
	}
}
