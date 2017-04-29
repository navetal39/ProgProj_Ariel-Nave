#include <stdio.h>
#include <stdlib.h>
#include "SPKDArray.h"


int spKDArrayPrint(SPKDArray* arr)
{
	printf("******************** Start printf kdarr ********************\n");
	int i, j;
	SPPoint* cer;

	printf("Size is: %d\n", arr->num_points);
	printf("Dim is: %d\n", arr->dim);

	for ( j = 0; j < arr->num_points; ++j) {
		printf("no. %d\t", j);
		cer = arr->points_array[j];
		printf("%d\n", spPointGetIndex(cer));
	}
	printf("Dim: %d\n", spPointGetDimension(arr->points_array[0]));

	for ( i = 0; i < spPointGetDimension(arr->points_array[0]); i++) {
		for ( j = 0; j < arr->num_points ; ++j) {
			printf("%d\t", arr->sort_mat[i][j]);
		}
		printf("\n");
	}
	printf("******************** End printf kdarr ********************\n");
	return 1;
}

int comparator_dim;
SPPoint** global_points_array;

// compares by the global currnet dimension
int spKDArrayCompare(const void *val1, const void *val2) {
	int tmp1;
	int tmp2;
	tmp1 = *(int*)val1;
	tmp2 = *(int*)val2;
	// apperently must return integer so im doing it with if conditions...
	if ((spPointGetAxisCoor(global_points_array[tmp1], comparator_dim) - spPointGetAxisCoor(global_points_array[tmp2], comparator_dim)) > 0) {
		return 1;
	}
	if ((spPointGetAxisCoor(global_points_array[tmp1], comparator_dim) - spPointGetAxisCoor(global_points_array[tmp2], comparator_dim)) == 0) {
		return 0;
	}
	else {
		return -1;
	}
}

void spKDArrayDestroy(SPKDArray* array, int num_points, int dim) {

	if (array != NULL) {
		if (length != -2) {
			destroyPointsArray(array->points_array, num_points);
		}
		if (dim != -2) {
			destroyIntMatrix(array->sort_mat, dim);
		}
		free(array);
	}
}

SPKDArray* spKDArrayAllocateArrayFields(SPKDArray* return_array, int length, int points_dim) {

	int i;

	return_array->dim = points_dim;
	return_array->num_points = length;

	if ((return_array->points_array = (SPPoint**)malloc(length * sizeof(SPPoint*))) == NULL) {
		printf("malloc fail :(\n");
		free(return_array);
		return NULL;
	}

	if ((return_array->sort_mat = (int**)malloc((points_dim) * sizeof(int*))) == NULL) {
		printf("malloc fail :(\n");
		spKDArrayDestroy(return_array, 0, -2);
		return NULL;
	}
	for (i = 0; i < points_dim; ++i) {
		if (((return_array->sort_mat)[i] = (int*)malloc(length * sizeof(int))) == NULL) {
			printf("malloc fail :(\n");
			spKDArrayDestroy(return_array, 0 , i-1);
			return NULL;
		}
	}
	return return_array;

}


SPKDArray* spKDArrayCreate(SPPoint** array_of_points, int length) {
	SPKDArray *return_array;
	int i;
	int j;

	if ((length <= 0) || (array_of_points == NULL) || (array_of_points[0] == NULL)) {
		printf("Bad Usage ! Illegal arguments\n");
		return NULL;
	}

	if ((return_array = (SPKDArray*)malloc(sizeof(SPKDArray))) == NULL) {
		printf("malloc fail :(\n");
		return NULL;
	}

	return_array->num_points = length;
	return_array->dim = spPointGetDimension(array_of_points[0]);

	if ((return_array->points_array = (SPPoint**)malloc(length * sizeof(SPPoint*))) == NULL) {
		printf("malloc fail :(\n");
		free(return_array);
		return NULL;
	}
	for (i = 0; i < length; ++i) {
		return_array->points_array[i] = spPointCopy(array_of_points[i]);
		if ((return_array->points_array[i]) == NULL) {
			printf("malloc fail :(\n");
			spKDArrayDestroy(return_array, i - 1 , -2);
			return NULL;
		}
	}

	if ((return_array->sort_mat = (int**)malloc((return_array->dim) * sizeof(int*))) == NULL) {
		printf("malloc fail :(\n");
		spKDArrayDestroy(return_array, length, -2);
		return NULL;
	}
	for (i = 0; i < return_array->dim; ++i) {
		if ((return_array->sort_mat[i] = (int*)malloc(length * sizeof(int))) == NULL) {
			printf("malloc fail :(\n");
			spKDArrayDestroy(return_array, length, i - 1);
			return NULL;
		}
	}

	global_points_array = return_array->points_array;
	for (i = 0; i < return_array->dim; ++i) {
		for (j = 0; j < length; ++j) {
			return_array->sort_mat[i][j] = j;
		}
		comparator_dim = i;
		qsort(return_array->sort_mat[i], length, sizeof(int), spKDArrayCompare);
	}
	return return_array;
}


int spKDArraySplit(SPKDArray* array, SPKDArray* part_1, SPKDArray* part_2, int coor) {

	int i, j;
	int num_left, num_right, tmp_lefts, tmp_rights, store_tmp;
	int* X;
	int* map1;
	int* map2;

	if ((coor < 0) || (array == NULL) || (coor >= array->dim)  || (part_1 == NULL) || (part_2 == NULL)) {
		printf("Bad Usage ! Illegal arguments\n");
		free(part_1);
		free(part_2);
		return -1;
	}

	if ((array->num_points) % 2 == 0) {
		num_left = (array->num_points) / 2;
	}
	else {
		num_left = ((array->num_points) / 2) + 1 ;
	}
	num_right = (array->num_points) - num_left;
	global_points_array = array->points_array;

	if ((part_1 = spKDArrayAllocateArrayFields(part_1, num_left, array->dim)) == NULL) {
		free(part_1);
		free(part_2);
		return -1;
	}

	if ((part_2 = spKDArrayAllocateArrayFields(part_2, num_right, array->dim)) == NULL) {
		free(part_2);
		spKDArrayDestroy(part_1, num_left, array->dim);
		return -1;
	}

	X = (int*)malloc(sizeof(int) * (array->num_points));
	if (X == NULL) {
		printf("malloc fail :(\n");
		spKDArrayDestroy(part_1, num_left, array->dim);
		spKDArrayDestroy(part_2, num_right, array->dim);
		return -1;
	}
	// setting all to 1
	for (i = 0; i < array->num_points; ++i) {
		X[i] = 1;
	}
	// seting the lefts to 0
	for (i = 0; i < num_left; ++i) {
		X[array->sort_mat[coor][i]] = 0;
	}
	//initializing the points array in part_! and part_2
	tmp_lefts = 0;
	tmp_rights = 0;
	for (i = 0; i < array->num_points; ++i) {
		if (X[i] == 0) {
			part_1->points_array[tmp_lefts] = spPointCopy(array->points_array[i]);
			tmp_lefts++;
		}
		else {
			part_2->points_array[tmp_rights] = spPointCopy(array->points_array[i]);
			tmp_rights++;
		}
	}


	map1 = (int*)malloc(sizeof(int) * (array->num_points));
	if (map1 == NULL) {
		printf("malloc fail :(\n");
		free(X);
		spKDArrayDestroy(part_1, num_left, array->dim);
		spKDArrayDestroy(part_2, num_right, array->dim);
		return -1;
	}
	map2 = (int*)malloc(sizeof(int) * (array->num_points));
	if (map2 == NULL) {
		printf("malloc fail :(\n");
		free(map1);
		free(X);
		spKDArrayDestroy(part_1, num_left, array->dim);
		spKDArrayDestroy(part_2, num_right, array->dim);
		return -1;
	}

	tmp_lefts = 0;
	tmp_rights = 0;
	// filling the maps
	for (i = 0; i < array->num_points; ++i) {
		if (X[i] == 0) {
			map1[i] = tmp_lefts;
			tmp_lefts++;
			map2[i] = -1;
		}
		else {
			map2[i] = tmp_rights;
			tmp_rights++;
			map1[i] = -1;
		}
	}

	for (i = 0; i < array->dim; ++i) {
		tmp_lefts = 0;
		tmp_rights = 0;
		for (j = 0; j < array->num_points; ++j) {
			store_tmp = array->sort_mat[i][j];
			if (X[store_tmp] == 0) {
				part_1->sort_mat[i][tmp_lefts] = map1[store_tmp];
				tmp_lefts++;
			}
			else {
				part_2->sort_mat[i][tmp_rights] = map2[store_tmp];
				tmp_rights++;
			}
		}
	}
	free(X);
	free(map1);
	free(map2);
	return 0;
}



void destroyPointsArray(SPPoint** pointArray, int arrayLength)
{
	if(!pointArray) {return; }
	int i;
	for(i = 0; i < arrayLength; ++i)
	{
		spPointDestroy(pointArray[i]);
	}
	free(pointArray);
}

void destroyPointsArrayArray(SPPoint*** arraysArray, int arrayLength,
							int* varyingLengths)
{
	if(!arraysArray) {return; }
	int i;
	for(i = 0; i<arrayLength; i++)
	{
		destroyPointsArray(arraysArray[i], varyingLengths[i]);
	}
	free(arraysArray);
}

void destroyIntMatrix(int** mat, int dim) {

	int i;
	if (mat != NULL) {
		for (i = 0; i < dim; i++) {
			if (mat[i] != NULL) {
				free(mat[i]);
			}
		}
		free(mat);
	}
}


/*
int main() {
	//check that init works
	double data[5];
	SPPoint* points_array[5];
	SPKDArray *array;
	SPKDArray *left_1;
	SPKDArray *left_2;
	SPKDArray *right_1;
	SPKDArray *right_2;
	int d;
	int error_flag;

	data[0] = 1;
	data[1] = 2;
	data[2] = 4;
	data[3] = 0;
	data[4] = 0;
	points_array[0] = spPointCreate(data, 5, 100);
	data[0] = 123;
	data[1] = 70;
	data[2] = 3;
	data[3] = 1;
	data[4] = 1;
	points_array[1] = spPointCreate(data, 5, 1);
	data[0] = 2;
	data[1] = 7;
	data[2] = 2;
	data[3] = 2;
	data[4] = 2;
	points_array[2] = spPointCreate(data, 5, 2);
	data[0] = 9;
	data[1] = 11;
	data[2] = 1;
	data[3] = 3;
	data[4] = 3;
	points_array[3] = spPointCreate(data, 5, 3);
	data[0] = 3;
	data[1] = 4;
	data[2] = 0;
	data[3] = 4;
	data[4] = 4;
	points_array[4] = spPointCreate(data, 5, 4);

	array = spKDArrayCreate(points_array, 5);
	spKDArrayPrint(array);
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");

	left_1 = (SPKDArray*)malloc(sizeof(SPKDArray));
	if (NULL == left_1) {
		//TODO ALLOCATION FAIL
		return -1;
	}
	left_2 = (SPKDArray*)malloc(sizeof(SPKDArray));
	if (NULL == left_2) {
		//TODO ALLOCATION FAIL
		return -1;
	}
	right_1 = (SPKDArray*)malloc(sizeof(SPKDArray));
	if (NULL == right_1) {
		//TODO ALLOCATION FAIL
		return -1;
	}
	right_2 = (SPKDArray*)malloc(sizeof(SPKDArray));
	if (NULL == right_2) {
		//TODO ALLOCATION FAIL
		return -1;
	}

	error_flag = spKDArraySplit(array, left_1, right_1, 1);
	printf("test2 - error flag is: %d \n", error_flag);
	printf("lefties : \n");
	spKDArrayPrint(left_1);
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("righties : \n");
	spKDArrayPrint(right_1);


	//result = (SPKDArray*)malloc(sizeof(SPKDArray));
	//if (result == NULL) {
	//	return NULL;
	//}


	//printf("Hello, World!\n");
	//return 0;
}
*/

































