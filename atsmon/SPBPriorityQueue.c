#include "SPBPriorityQueue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

/**
 * This is a helper function.
 * It clones a BPQueueElement from element to res
 *
 * @param element - The BPQueueElement to be copied
 * @param res - The BPQueueElement element is copied to
 */
void cloneBPQueueElement(BPQueueElement* element, BPQueueElement* res)
{
	assert(element != NULL && res != NULL);

	res->index = element->index;
	res->value = element->value;
}

SPBPQueue* spBPQueueCreate(int maxSize)
{
	SPBPQueue *newSPBQueue;

	if (NULL == (newSPBQueue = (SPBPQueue*)malloc(sizeof(SPBPQueue))))
	{
		printf("Error allocating memory: %s\n", strerror(errno));
		return NULL;
	}
	newSPBQueue->maxSize = maxSize;
	newSPBQueue->size = 0;
	newSPBQueue->data = (BPQueueElement*)malloc(sizeof(BPQueueElement) * maxSize);
	if (NULL == newSPBQueue->data)
	{
		printf("Error allocating memory: %s\n", strerror(errno));
		return NULL;
	}
	return newSPBQueue;
}

SPBPQueue* spBPQueueCopy(SPBPQueue* source)
{
	int i;
	SPBPQueue* newSPBQueue;

	assert(NULL != source);

	if (NULL == (newSPBQueue = (SPBPQueue*)malloc(sizeof(SPBPQueue))))
	{
		printf("Error allocating memory: %s\n", strerror(errno));
		return NULL;
	}

	newSPBQueue->size = source->size;
	newSPBQueue->maxSize = source->maxSize;

	newSPBQueue->data = (BPQueueElement*)malloc(sizeof(BPQueueElement) * source->maxSize);
	if (NULL == newSPBQueue->data)
	{
		printf("Error allocating memory: %s\n", strerror(errno));
		return NULL;
	}
	for (i = 0; i < source->maxSize; ++i)
	{
		(newSPBQueue->data)[i].value = (source->data)[i].value;
		(newSPBQueue->data)[i].index = (source->data)[i].index;
	}
	return newSPBQueue;

}

void spBPQueueDestroy(SPBPQueue* source)
{
	assert(NULL != source);

	free(source->data);
	free(source);
}

void spBPQueueClear(SPBPQueue* source)
{
	assert(NULL != source);
	source->size = 0;
}

int spBPQueueSize(SPBPQueue* source)
{
	assert(NULL != source);

	return source->size;
}

int spBPQueueGetMaxSize(SPBPQueue* source)
{
	assert(NULL != source);

	return source->maxSize;
}

SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue* source, int index, double value)
{
	if (NULL == source)
	{
		return SP_BPQUEUE_INVALID_ARGUMENT;
	}

	int i, insert_index;

	insert_index = 0;
	for (i = 0; i < source->size; ++i)
	{
		if (value == (source->data)[i].value)
		{
			if(index >= source->data[i].index)
			{
				break;
			}
		}
		if (value > (source->data)[i].value)
		{
			break;
		}
		insert_index = i + 1;
	}

	if (spBPQueueIsFull(source))
	{
		if (insert_index != 0)
		{
			for (i = 0; i < insert_index - 1; ++i)
			{
				(source->data)[i].value = (source->data)[i + 1].value;
				(source->data)[i].index = (source->data)[i + 1].index;
			}
			(source->data)[insert_index - 1].value = value;
			(source->data)[insert_index - 1].index = index;
		}
		return SP_BPQUEUE_FULL;
	}
	else
	{
		for (i = source->size; i > insert_index; --i)
		{
			(source->data)[i].value = (source->data)[i - 1].value;
			(source->data)[i].index = (source->data)[i - 1].index;
		}
		(source->data)[insert_index].value = value;
		(source->data)[insert_index].index = index;
		++(source->size);
		return SP_BPQUEUE_SUCCESS;
	}
}

SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue* source)
{
	if (NULL == source)
	{
		return SP_BPQUEUE_INVALID_ARGUMENT;
	}

	if (spBPQueueIsEmpty(source))
	{
		return SP_BPQUEUE_EMPTY;
	}
	--(source->size);
	return SP_BPQUEUE_SUCCESS;

}

SP_BPQUEUE_MSG spBPQueuePeek(SPBPQueue* source, BPQueueElement* res)
{
	if (NULL == source || NULL == res)
	{
		return SP_BPQUEUE_INVALID_ARGUMENT;
	}

	if (0 == source->size)
	{
		return SP_BPQUEUE_EMPTY;
	}
	cloneBPQueueElement((source->data)+(source->size) - 1, res);
	return SP_BPQUEUE_SUCCESS;
}

SP_BPQUEUE_MSG spBPQueuePeekLast(SPBPQueue* source, BPQueueElement* res)
{
	if (NULL == source || NULL == res)
	{
		return SP_BPQUEUE_INVALID_ARGUMENT;
	}

	if (0 == source->size)
	{
		return SP_BPQUEUE_EMPTY;
	}

	cloneBPQueueElement(source->data, res);
	return SP_BPQUEUE_SUCCESS;
}

double spBPQueueMinValue(SPBPQueue* source)
{
	assert((NULL != source) && (0 != source->size));

	return ((source->data) + (source->size) - 1)->value;
}

double spBPQueueMaxValue(SPBPQueue* source)
{
	assert((NULL != source) && (0 != source->size));

	return (source->data)->value;
}

bool spBPQueueIsEmpty(SPBPQueue* source)
{
	assert(NULL != source);

	if (0 == source->size)
	{
		return true;
	}
	return false;
}

bool spBPQueueIsFull(SPBPQueue* source)
{
	assert(NULL != source);

	if (source->size == source->maxSize)
	{
		return true;
	}
	return false;
}
