#include "SPPoint.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>


SPPoint* spPointCreate(double* data, int dim, int index)
{
	SPPoint *newPoint;
	int i;

	assert(data != NULL || dim <= 0 || index < 0);

	if (NULL == (newPoint = (SPPoint*)malloc(sizeof(SPPoint))))
	{
		printf("Error allocating memory: %s\n", strerror(errno));
		return NULL;
	}

	newPoint->dim = dim;
	newPoint->index = index;

	if (NULL == (newPoint->data = (double*)malloc(sizeof(double) * dim)))
	{
		printf("Error allocating memory: %s\n", strerror(errno));
		return NULL;
	}

	for (i = 0; i < dim; ++i)
	{
		newPoint->data[i] = data[i];
	}

	return newPoint;
}

SPPoint* spPointCopy(SPPoint* source)
{
	SPPoint *copyPoint;

	assert(NULL != source);

	copyPoint = spPointCreate(source->data, source->dim, source->index);
	if (NULL == copyPoint)
	{
		printf("Error copying point.\n");
		return NULL;
	}
	return copyPoint;

}

void spPointDestroy(SPPoint* point)
{
	if (point != NULL)
	{
		free(point->data);
		free(point);
	}
}

int spPointGetDimension(SPPoint* point)
{
	assert(NULL != point);
	return point->dim;
}

int spPointGetIndex(SPPoint* point)
{
	assert(NULL != point);
	return point->index;
}

double spPointGetAxisCoor(SPPoint* point, int axis)
{
	assert(point != NULL && axis < point->dim);
	return point->data[axis];
}

double spPointL2SquaredDistance(SPPoint* p, SPPoint* q)
{
	double sqDistance=0;
	int i;

	assert(NULL != p && NULL != q && p->dim == q->dim);

	for (i=0; i<p->dim; ++i)
	{
		sqDistance += (p->data[i]-q->data[i])*(p->data[i]-q->data[i]);
	}
	return sqDistance;
}
