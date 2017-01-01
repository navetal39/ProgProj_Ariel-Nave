#include "SPPoint.h"
#include "SPBPriorityQueue.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

SPPoint* readPoint (int dim, int index)
{
	printf("reading point of dim %d... index: %d\n", dim, index);
	assert(dim>0);
	double* data =(double*) malloc(dim * sizeof(double));
	int i, allGood = 1;
	SPPoint* retPoint = NULL;
	for(i = 0; i < dim && allGood; ++i)
	{
		if(scanf("%lf",  &data[i]) != 1)
		{
			allGood--;
		}
	}
	if(allGood)
	{
		retPoint = spPointCreate(data, dim, index);
	}
	free(data);
	return retPoint;
}
SPPoint** readPoints(int ammount, int dim, int index)
{
	assert(dim>0);
	int i, j, allGood = 1;
	SPPoint** points = (SPPoint**) malloc(ammount*sizeof(SPPoint*));
	for(i = 0; i < ammount && allGood; ++i)
	{
		points[i] = readPoint(dim, ++index);
		if(points[i] == NULL)
		{
			for(j = 0; j<i; ++j)
			{
				spPointDestroy(points[j]);
			}
			allGood--;
		}
	}
	if(allGood)
	{
		return points;
	}else{
		return NULL;
	}
}
int* find_KNN(SPPoint** points, SPPoint* target, int ammount, int k)
{
	int i;
	int* knn = (int*)malloc(k*sizeof(int));
	double distance;
	SPBPQueue* queue =  spBPQueueCreate(k);
	SPPoint* curPoint;
	BPQueueElement* tempElement = (BPQueueElement*)malloc(sizeof(BPQueueElement));

	for(i=0; i<ammount; ++i)
	{
		curPoint = points[i];
		distance = spPointL2SquaredDistance(target, curPoint);
		spBPQueueEnqueue(queue, curPoint->index, distance);
	}
	printf("%d\n", queue->size);
	for(i=0; i<(queue->size); ++i)
	{
		printf("peeking...");
		spBPQueuePeek(queue, tempElement);
		printf("putting %d in knn[%d]...", tempElement->index, i);
		knn[i] = tempElement->index;
		printf("dequeueing...\n");
		spBPQueueDequeue(queue);
	}
	printf("destroying!\n");
	spBPQueueDestroy(queue);
	free(tempElement);
	printf("returning!\n");
	return knn;
}
void destroyPointsArray(SPPoint** pointArray, int arrayLength)
{
	int i;
	for(i = 0; i < arrayLength; ++i)
	{
		spPointDestroy(pointArray[i]);
	}
}