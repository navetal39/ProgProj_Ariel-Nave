#include "SSPoint.h"
#include "SSBPriorityQueue.h"
#include <stdlib.h>
#include <stdio.h>

SPPoint* readPoint (int dim, int* index)
{
	assert(dim>0);
	double* data =(double*) malloc(dim * sizeof(double));
	int i, allGood = 1;
	SPPoint* retPoint = NULL;
	for(i = 0; i < dim && allGood; ++i)
	{
		if(scanf("%f",  &data[i]) != 1)
		{
			allGood--;
		}
	}
	if(allGood)
	{
		retPoint = spPointCreate(data, dim, ++(*index)); 
	}
	free(data);
	if(retPoint == NULL)
	{
		--(*index);
	}
	return retPoint;
}
SPPoint** readPoints(int ammount, int dim, int* index);
{
	int i, j, allGood = 1;
	int origIndex = *index;
	SPPoint** points = (SPPoint**) malloc(ammount*sizeOf(SPPoint*));
	for(i = 0; i < ammount && allGood; ++i)
	{
		points[i] = readPoint(dim, index);
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
		*index = origIndex;
		return NULL;
	}
}
int* find_KNN(SPPoint** points, SPPoint* target, int ammount, int dim, int k)
{

}
void cleanPointsArray(SPPoint** array, int arrayLength)
{
	int i;
	for(i = 0; i < arrayLength; ++i)
	{
		spPointDestroy(array[i]);
	}
}