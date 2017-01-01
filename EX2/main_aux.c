#include "SPPoint.h"
#include "SPBPriorityQueue.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>


struct sp_point_t {
	int index;
	int dim;
	double* data;
};

SPPoint* readPoint (int dim, int index)
{
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
	assert(1<=k && k<=ammount);
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
		spBPQueueEnqueue(queue, spPointGetIndex(curPoint), distance);
	}
	printf("%d\n", spBPQueueSize(queue));
	for(i=0; i<k; ++i)
	{
		spBPQueuePeek(queue, tempElement);
		knn[i] = tempElement->index;
		spBPQueueDequeue(queue);
	}
	spBPQueueDestroy(queue);
	free(tempElement);
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