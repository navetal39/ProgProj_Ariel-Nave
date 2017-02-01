#include "SPPoint.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

//defines the struct point with fields
//data pointer, dimension and index
struct sp_point_t{
	double *data;
	int dim;
	int index;
};

SPPoint* spPointCreate(double *data, int dim, int index){
	//declare variables
	SPPoint *point;
	int forLoops;
	//returns NULL in case data is NULL
	if(!data){
		return NULL;
	}
	//returns NULL in case dimension is non-positive
	if(dim <= 0){
		return NULL;
	}
	//return NULL in case index is negative
	if(index<0){
		return NULL;
	}
	//memory allocation
	if(!(point = (SPPoint *)malloc(sizeof(*point)))) {
		return NULL;
	}
	
	if(!(point->data = (double *)malloc(dim*sizeof(double)))) {
		return NULL;
	}
	//update fields of the new point
	for(forLoops = 0; forLoops<dim; ++forLoops){
		(point->data)[forLoops] = data[forLoops];
	}
	point->dim = dim;
	point->index = index;
	//returns a pointer to the new point
	return point;
}

SPPoint* spPointCopy(SPPoint *source){
	//makes sure source is not NULL
	assert(source);
	//returns a new point with same fields as source
	return spPointCreate(source->data, source->dim, source->index);
}

void spPointDestroy(SPPoint *point){
	//nothing to do if point is NULL
	if(!point){
		return;
	}
	//frees memory we used
	free(point->data);
	free(point);
}

int spPointGetDimension(SPPoint* point){
	//makes sure point is not NULL
	assert(point);
	//returns point's dimension
	return point->dim;
}

int spPointGetIndex(SPPoint* point){
	//makes sure point is not NULL
	assert(point);
	//returns point's index
	return point->index;
}


double spPointGetAxisCoor(SPPoint* point, int axis){
	//makes sure point is not NULL
	//and axis is smaller then the dimension
	assert(point && (axis<point->dim)&&axis>=0);
	//returns the data in the suitable axis
	return point->data[axis];
}

double spPointL2SquaredDistance(SPPoint* p, SPPoint* q){
	//makes sure both points are
	//not NULL and they are of equal dimension
	assert(p && q && p->dim == q->dim);
	//declares and initializes variables
	int i;
	double sum=0;
	//computes the distance squared
	for(i = 0; i < p->dim; ++i){
		sum += (p->data[i] - q->data[i])*(p->data[i] - q->data[i]);
	}
	//returns what we have got :-)
	return sum;
}
