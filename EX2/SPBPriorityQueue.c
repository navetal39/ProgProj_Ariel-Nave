#include "SPBPriorityQueue.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>


struct sp_bp_queue_t{
	BPQueueElement* arr;
	int maxSize;
	int size;
};

SPBPQueue* spBPQueueCreate(int maxSize){
	int i;
	SPBPQueue* new_queue = NULL;
	new_queue = (SPBPQueue*) malloc(sizeof(*new_queue));
	if (new_queue == NULL) {
		return NULL;
	}
	new_queue->maxSize = maxSize;
	new_queue->size = 0;
	new_queue->arr = (BPQueueElement*) malloc(maxSize * sizeof(BPQueueElement));
	memset(new_queue->arr, '\0', maxSize * sizeof(BPQueueElement));
	if (new_queue->arr == NULL) {
		return NULL;
	}
	return new_queue;
}

SPBPQueue* spBPQueueCopy(SPBPQueue* source){
	assert(source != NULL);
	SPBPQueue* copy_queue = spBPQueueCreate(source-> maxSize);
	if (copy_queue == NULL) {
		return NULL;
	}
	copy_queue->size = source->size;
	int i;
	for (i=0; i<source->size; ++i){
		copy_queue->arr[i] = source->arr[i];
	}
	return copy_queue; 
}

void spBPQueueDestroy(SPBPQueue* source){
	if (source != NULL){
		free(source->arr);
		free(source);
	}

}

void spBPQueueClear(SPBPQueue* source){
	int i;
	memset(source->arr, '\0', (source->maxSize) * sizeof(BPQueueElement));
}

int spBPQueueSize(SPBPQueue* source){
	assert(source != NULL);
	return source->size;
}

int spBPQueueGetMaxSize(SPBPQueue* source){
	assert(source != NULL);
	return source->maxSize;
}

SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue* source, int index, double value){
	assert(source != NULL);
	if (source->maxSize == 0){
		return SP_BPQUEUE_FULL;
	}
	int i;
	int temp_index = index;
	double temp_value = value;
	int temp_temp_index;      ///// used only for swap
	double temp_temp_value;   ///// used only for swap
	if (source->size == source->maxSize){
		for (i=0; i < source->size; ++i){
			if ((source->arr[i]).value > temp_value){
				/// this is swap between the values of the "waiting element" and the current element in the array
				temp_temp_index = temp_index;
				temp_temp_value = temp_value;
				temp_index = (source->arr[i]).index;
				temp_value = (source->arr[i]).value;
				(source->arr[i]).index = temp_temp_index;
				(source->arr[i]).value = temp_temp_value;
			}
		}
		return SP_BPQUEUE_FULL; //////////////////////////////////////////////////////////////////////maybe should return success
	}

	for (i=0; i < source->size; ++i){
		if ((source->arr[i]).value > temp_value){
				/// this is swap between the values of the "waiting element" and the current element in the array
				temp_temp_index = temp_index;
				temp_temp_value = temp_value;
				temp_index = (source->arr[i]).index;
				temp_value = (source->arr[i]).value;
				(source->arr[i]).index = temp_temp_index;
				(source->arr[i]).value = temp_temp_value;
		}
	}
	++(source->size);
	(source->arr[source->size]).index = temp_index;
	(source->arr[source->size]).value = temp_value;
	return SP_BPQUEUE_SUCCESS;
}

SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue* source){
	assert(source != NULL);
	if (source->size == 0){
		return SP_BPQUEUE_EMPTY;
	}
	source->size -= 1;
	///////////////////////////////////////////////////////////////////// free ????? memset ?????
	return SP_BPQUEUE_SUCCESS;
}

SP_BPQUEUE_MSG spBPQueuePeek(SPBPQueue* source, BPQueueElement* res){
	assert(source != NULL && res != NULL && source->arr != NULL);
	if (source->size == 0){
		return SP_BPQUEUE_EMPTY;
	}
	res = &(source->arr[(source->size) -1]);
	return SP_BPQUEUE_SUCCESS;
}

SP_BPQUEUE_MSG spBPQueuePeekLast(SPBPQueue* source, BPQueueElement* res){
	assert(source != NULL && res != NULL && source->arr != NULL);
	if (source->size == 0){
		return SP_BPQUEUE_EMPTY;
	}
	res = &(source->arr[0]);
	return SP_BPQUEUE_SUCCESS;
}

double spBPQueueMinValue(SPBPQueue* source){
	assert(source != NULL && source->arr != NULL);
	if (source->size == 0){
		return SP_BPQUEUE_EMPTY;
	}
	return (source->arr[0]).value;
}

double spBPQueueMaxValue(SPBPQueue* source){
	assert(source != NULL && source->arr != NULL);
	if (source->size == 0){
		return SP_BPQUEUE_EMPTY;
	}
	return (source->arr[(source->size)-1]).value;
}

bool spBPQueueIsEmpty(SPBPQueue* source){
	assert(source != NULL);
	if (source->size == 0){
		return true;
	}
	return false;
}

bool spBPQueueIsFull(SPBPQueue* source){
	assert(source != NULL);
	if (source->size == source->maxSize){
		return true;
	}
	return false;
}







