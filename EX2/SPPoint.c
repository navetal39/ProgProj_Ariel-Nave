#include "SPPoint.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

struct sp_point_t {
	int index;
	int dim;
	double* data;
};


SPPoint* spPointCreate(double* data, int dim, int index) {
	SPPoint* new_point = NULL;
	new_point = (SPPoint*) malloc(sizeof(*new_point));
	if (new_point == NULL) {
		return NULL;
	}

	if (data == NULL || dim <= 0 || index < 0)
	{
		return NULL;
	}

	new_point->index = index;
	new_point->dim = dim;
	new_point->data = (double*) malloc(dim * sizeof(double));
	if (new_point->data == NULL)
	{
		return NULL;
	}
	int i;
	for (i = 0; i < dim; ++i) {
		new_point->data[i] = data[i];
	}
	return new_point;
}


SPPoint* spPointCopy(SPPoint* source) {
	assert (source != NULL);
	return spPointCreate(source->data, source->dim, source->index);
}

void spPointDestroy(SPPoint* point) {
	if (point != NULL)
	{
		free(point->data);
		free(point);
	}
}

int spPointGetDimension(SPPoint* point) {
	assert(point != NULL);
	return point->dim;
}

int spPointGetIndex(SPPoint* point) {
	assert(point != NULL);
	return point->index;
}

double spPointGetAxisCoor(SPPoint* point, int axis) {
	assert (point != NULL && axis < point->dim);
	return (point->data)[axis];
}
double spPointL2SquaredDistance(SPPoint* p, SPPoint* q) {
	assert (p != NULL && q != NULL && p->dim == q->dim);
	double current_dist ;

	current_dist = 0;
	int i;
	for (i = 0; i < p->dim; ++i) {
		current_dist += ((q->data)[i] - (p->data)[i]) * ((q->data)[i] - (p->data)[i]);
	}
	return current_dist;
}