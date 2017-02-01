#include "SPBPriorityQueue.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct sp_bp_queue_t{
	int size;
	int maxSize;
	BPQueueElement *arr;

};

//declaration of auxilary functions
bool isGreater(int index1, double value1, BPQueueElement* other);
void copyElement(BPQueueElement *source, BPQueueElement *dest);


SPBPQueue* spBPQueueCreate(int maxSize){
	//makes sure size is positive
	assert(maxSize>0);
	//declare (and anitializes) variables
	SPBPQueue *ourQ;
	if(!(ourQ = (SPBPQueue *)malloc(sizeof(*ourQ)))){
		printf("Error allocating memory for queue\n");
		return NULL;
	}
	BPQueueElement *arr;
	if(!(arr = (BPQueueElement *)malloc(sizeof(BPQueueElement)*maxSize))) {
		printf("Error allocating memory\n");
		return NULL;
	}
	
	memset(arr, 0, sizeof(BPQueueElement)*maxSize);

	//anitializes fields
	ourQ -> size = 0;
	ourQ -> maxSize = maxSize;
	ourQ -> arr = arr;
	return ourQ;
}

SPBPQueue* spBPQueueCopy(SPBPQueue* source){
	//makes sure source is not NULL
	assert(source);
	//copies source's fields to a new queue 'copy'
	SPBPQueue *copy = spBPQueueCreate(source ->maxSize);
	copy -> size = source -> size;
	copy -> maxSize = source -> maxSize;
	int forLoops;
	for(forLoops = 0; forLoops< source -> size; ++forLoops){
		copyElement(&((source -> arr)[forLoops]),&((copy -> arr)[forLoops])); 
		
	}
	return copy;
}

void spBPQueueDestroy(SPBPQueue* source){
	//makes sure source is not NULL
	assert(source);
	//frees the memory that  source used
	free(source->arr);
	free(source);
}


void spBPQueueClear(SPBPQueue* source){
	//makes sure source is not NULL
	assert(source);
	//reset size and array of the queue
	source -> size = 0;
	memset((source -> arr), 0, sizeof(BPQueueElement)*(source->maxSize));

}

int spBPQueueSize(SPBPQueue* source){
	//makes sure source is not NULL
	assert(source);
	//returns size field
	return source -> size;
}

int spBPQueueGetMaxSize(SPBPQueue* source){
	//makes sure source is not NULL
	assert(source);
	//returns maxSize field
	return source -> maxSize;
}


SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue* source, int index, double value){
	//declare and initializes variables
	int forLoops, found = 0;
	//return error message of invalid argument if source is NULL
	if(!source){
		return SP_BPQUEUE_INVALID_ARGUMENT;
	}
	//case queue is full
	if(source->size == source->maxSize){
		//case value is too large for the full queue
		if(isGreater(index,value,&(source->arr)[0])){
			return SP_BPQUEUE_FULL;
		}
		//case maxSize of the source equals 1
		if(source->maxSize == 1){
			(source->arr)[0].index = index;
			(source->arr)[0].value = value;
			return SP_BPQUEUE_SUCCESS;
		}
		//searches for the right place to insert
		for(forLoops = 1; (forLoops < source->size)&& !found; ++forLoops){
			if(!(isGreater(index,value,&(source->arr)[forLoops]))){
				(source->arr)[forLoops-1] = source->arr[forLoops];
			}
			else{
				(source->arr)[forLoops-1].index = index;
				(source->arr)[forLoops-1].value = value;
				found = 1;
			}
		}
		//need to insert at the end of the queue
		if(!found){
			(source->arr)[(source->size)-1].index = index;
			(source->arr)[(source->size)-1].value = value;
		}
		return SP_BPQUEUE_SUCCESS;
	}
	//case queue is empty
	if(source->size == 0){
		(source->arr)[0].index = index;
		(source->arr)[0].value = value;
		found = 1;
	}
	//searches for the right place to insert
	for(forLoops = (source->size)-1; forLoops>=0 && !found;--forLoops){
		if(isGreater(index,value,&((source->arr)[forLoops]))){
			(source->arr)[forLoops+1] = (source->arr)[forLoops];
		}
		else{
			(source->arr)[forLoops+1].index = index;
			(source->arr)[forLoops+1].value = value;
			found = 1;
		}
	}
	//inserts in case it is the new last
	if(!found){
		(source->arr)[0].index = index;
		(source->arr)[0].value = value;
	}
	//increases the size field
	++(source->size);
	//if we got here, returns success mesage :-)
	return SP_BPQUEUE_SUCCESS;
}

SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue* source){
	//returns error message of invalid argument if source is NULL
	if(!source){
		return SP_BPQUEUE_INVALID_ARGUMENT;
	}
	//returns error message of empty queue if queue is empty and thus can't dequeue
	if(source->size == 0){
		return SP_BPQUEUE_EMPTY;
	}
	//resets memory we used to store the element w edequeued
	memset(&((source->arr)[--(source->size)]),0,sizeof(BPQueueElement));
	//if we got here, returns success mesage :-)
	return SP_BPQUEUE_SUCCESS;
}

SP_BPQUEUE_MSG spBPQueuePeek(SPBPQueue* source, BPQueueElement* res){
	//returns error message of invalid argument if source is NULL
	if(!source || !res){
		return SP_BPQUEUE_INVALID_ARGUMENT;
	}
	//returns error message of empty queue if queue is empty and thus can't peek
	if(source->size == 0){
		return SP_BPQUEUE_EMPTY;
	}
	//creates copy of the element to return using our
	//auxiliary function for copy elements
	copyElement(&((source->arr)[(source->size) - 1]),res);
	//return success if we got here
	return SP_BPQUEUE_SUCCESS;
}

SP_BPQUEUE_MSG spBPQueuePeekLast(SPBPQueue* source, BPQueueElement* res){
	//returns error message of invalid argument if source is NULL
	if(!source || !res){
		return SP_BPQUEUE_INVALID_ARGUMENT;
	}
	//returns error message of empty queue if queue is empty and thus can't peek
	if(source->size == 0){
		return SP_BPQUEUE_EMPTY;
	}
	//creates copy of the element to return using
	//our auxiliary function for copy elements
	copyElement(&((source->arr)[0]),res);
	//return success if we got here
	return SP_BPQUEUE_SUCCESS;
}

double spBPQueueMinValue(SPBPQueue* source){
	//makes sure source is not NULL and source's size is larger than zero
	assert(source && (source->size > 0));
	//returns the value of the last element in the array
	return (((source->arr)[(source->size)-1]).value);
}

double spBPQueueMaxValue(SPBPQueue* source){
	//makes sure source is not NULL and source's size is larger than zero
	assert(source && (source->size > 0));
	//returns the value of the first element in the array
	return (((source->arr)[0]).value);
}

bool spBPQueueIsEmpty(SPBPQueue* source){
	//makes sure source is not NULL
	assert(source);
	//return true iff size equals zero
	return ((source->size) == 0);
}

bool spBPQueueIsFull(SPBPQueue* source){
	//makes sure source is not NULL
	assert(source);
	//return true iff size equals maxSize
	return ((source->size) == (source->maxSize));
}



/*
 * Auxilary functions
 */


/**
 * Copies a given queue element to a given location
 *
 * 
 * @param source - queue element to be copied
 * @param dest - location for element to be copied to
 * @assert source != NULL && dest != NULL
 *
 */
void copyElement(BPQueueElement *source, BPQueueElement *dest){
	//makes sure source is not NULL
	assert(source&&dest);

	dest->index = source->index;
	dest->value = source->value;
	//declares and allocates memory for the copied element
}

/**
 * decides which element is greater
 * i.e. which element should come last in the queue
 *
 * @param value1 - the value of the element
 * @param index1 - the index of the element
 * @param other - the element to compare to
 * @return
 * true in case the element should come after the other
 * or false in case it should come before the other
 *
 */
bool isGreater(int index1, double value1, BPQueueElement* other){
	//compares values
	if (value1>other->value){
		return true;
	}
	if(value1<other->value){
		return false;
	}
	//compares indices in case values are equals
	if(index1<other->index){
		return false;
	}
	return true;
}



