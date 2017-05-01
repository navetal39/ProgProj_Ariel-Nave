#include "SPKDArray.h"

CompPoint* InitComp(SPPoint* point, int index, int dim) {
	CompPoint* return_comp;
	return_comp = (CompPoint*)malloc(sizeof(CompPoint));
	if (return_comp == NULL) {
		spLoggerPrintError(LOG_ERR_MSG_ALLOC, __FILE__,__func__,__LINE__);
		return NULL;
	}
	return_comp->point = spPointCopy(point);
	if (return_comp->point == NULL) {
		spLoggerPrintError(LOG_ERR_MSG_ALLOC, __FILE__,__func__,__LINE__);
		free(return_comp);
		return NULL;
	}
	return_comp->index = index;
	return_comp->dim = dim;
	return return_comp;
}

CompPoint** InitCompArray(SPPoint** points_array, int length, int dim) {
	CompPoint** return_array;
	int i;

	return_array = (CompPoint**)malloc(sizeof(CompPoint*) * length);
	if (return_array == NULL) {
		spLoggerPrintError(LOG_ERR_MSG_ALLOC, __FILE__,__func__,__LINE__);
		return NULL;
	}
	for (i = 0; i < length; i++) {
		return_array[i] = InitComp(points_array[i], i, dim);
		if (return_array[i] == NULL ) {
			DestroyCompArray(return_array, i);
			return NULL;
		}
	}
	return return_array;
}

void DestroyCompArray(CompPoint** comp_array, int length) {
	int i;
	if (comp_array == NULL) {
		spLoggerPrintWarning(LOG_WRN_MSG_NO_DESTROY, __FILE__,__func__,__LINE__);
		return;
	}
	for (i = 0; i < length; i++) {
		if (comp_array[i] != NULL) {
			spPointDestroy((comp_array[i])->point);
			free(comp_array[i]);
		}
	}
	free(comp_array);
	return;
}

int spKDArrayCompare(const void *val1, const void *val2) {
	CompPoint* comp1;
	CompPoint* comp2;
	comp1 = *(CompPoint**)val1;
	comp2 = *(CompPoint**)val2;
	// apperently must return integer so im doing it with if conditions...
	if ((spPointGetAxisCoor((comp1->point), comp1->dim) - spPointGetAxisCoor((comp2->point), comp2->dim)) > 0) {
		return 1;
	}
	if ((spPointGetAxisCoor((comp1->point), comp1->dim) - spPointGetAxisCoor((comp2->point), comp2->dim)) == 0) {
		return 0;
	}
	else {
		return -1;
	}
}

void spKDArrayDestroy(SPKDArray* array, int num_points, int dim) {

	if (array != NULL) {
		if (num_points != -2) {
			destroyPointsArray(array->points_array, num_points);
		}
		if (dim != -2) {
			destroyIntMatrix(array->sort_mat, dim);
		}
		free(array);
	}else{
		spLoggerPrintWarning(LOG_WRN_MSG_NO_DESTROY, __FILE__,__func__,__LINE__);
	}
}

int spKDArrayGetUpperValue(int n) {
	int k;
	if ((n) % 2 == 0) {
		k = n / 2;
	}
	else {
		k = (n / 2) + 1 ;
	}
	return k;
}

void spKDArraySetBooleanArray(SPKDArray* array, int* X, int num_left, int coor) {
	int i;

	// setting all to 1
	for (i = 0; i < array->num_points; ++i) {
		X[i] = 1;
	}
	// seting the lefts to 0
	for (i = 0; i < num_left; ++i) {
		X[array->sort_mat[coor][i]] = 0;
	}
}
void spKDArrayFillingMaps(SPKDArray* array, int* X, int* map1, int* map2) {
	int tmp_lefts = 0;
	int tmp_rights = 0;
	int i;

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
}

//initializing the points array in part_1 and part_2
void spKDArraySetPointsArray(SPKDArray* array, SPKDArray* part_1, SPKDArray* part_2, int* X) {
	int tmp_lefts = 0;
	int tmp_rights = 0;
	int i;

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
}

void spKDArraySetMatrices(SPKDArray* array, SPKDArray* part_1, SPKDArray* part_2, int* X, int* map1, int* map2) {
	int i, j, tmp_lefts, tmp_rights;
	int store_tmp = 0;

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
}

SPKDArray* spKDArrayAllocateArrayFields(SPKDArray* return_array, int length, int points_dim) {

	int i;

	return_array->dim = points_dim;
	return_array->num_points = length;

	if ((return_array->points_array = (SPPoint**)malloc(length * sizeof(SPPoint*))) == NULL) {
		spLoggerPrintError(LOG_ERR_MSG_ALLOC, __FILE__,__func__,__LINE__);
		free(return_array);
		return NULL;
	}

	if ((return_array->sort_mat = (int**)malloc((points_dim) * sizeof(int*))) == NULL) {
		spLoggerPrintError(LOG_ERR_MSG_ALLOC, __FILE__,__func__,__LINE__);
		spKDArrayDestroy(return_array, 0, -2);
		return NULL;
	}
	for (i = 0; i < points_dim; ++i) {
		if (((return_array->sort_mat)[i] = (int*)malloc(length * sizeof(int))) == NULL) {
			spLoggerPrintError(LOG_ERR_MSG_ALLOC, __FILE__,__func__,__LINE__);
			spKDArrayDestroy(return_array, 0 , i - 1);
			return NULL;
		}
	}
	return return_array;

}


SPKDArray* spKDArrayCreate(SPPoint** array_of_points, int length) {
	CompPoint ** comp_array;
	SPKDArray *return_array;
	int i , j;

	if ((length <= 0) || (array_of_points == NULL) || (array_of_points[0] == NULL)) {
		spLoggerPrintError(LOG_ERR_MSG_BAD_ARGS, __FILE__, __func__, __LINE__);
		return NULL;
	}
	if ((return_array = (SPKDArray*)malloc(sizeof(SPKDArray))) == NULL) {
		spLoggerPrintError(LOG_ERR_MSG_ALLOC, __FILE__,__func__,__LINE__);
		return NULL;
	}
	return_array->num_points = length;
	return_array->dim = spPointGetDimension(array_of_points[0]);

	if ((return_array->points_array = (SPPoint**)malloc(length * sizeof(SPPoint*))) == NULL) {
		spLoggerPrintError(LOG_ERR_MSG_ALLOC, __FILE__,__func__,__LINE__);
		free(return_array);
		return NULL;
	}
	for (i = 0; i < length; ++i) {
		return_array->points_array[i] = spPointCopy(array_of_points[i]);
		if ((return_array->points_array[i]) == NULL) {
			spLoggerPrintError(LOG_ERR_MSG_ALLOC, __FILE__,__func__,__LINE__);
			spKDArrayDestroy(return_array, i - 1 , -2);
			return NULL;
		}
	}

	if ((return_array->sort_mat = (int**)malloc((return_array->dim) * sizeof(int*))) == NULL) {
		spLoggerPrintError(LOG_ERR_MSG_ALLOC, __FILE__,__func__,__LINE__);
		spKDArrayDestroy(return_array, length, -2);
		return NULL;
	}
	for (i = 0; i < return_array->dim; ++i) {
		if ((return_array->sort_mat[i] = (int*)malloc(length * sizeof(int))) == NULL) {
			spLoggerPrintError(LOG_ERR_MSG_ALLOC, __FILE__,__func__,__LINE__);
			spKDArrayDestroy(return_array, length, i - 1);
			return NULL;
		}
	}

	for (i = 0; i < return_array->dim; ++i) {

		if ((comp_array = InitCompArray(array_of_points, length, i)) == NULL) {
			spLoggerPrintError(LOG_ERR_MSG_ALLOC, __FILE__,__func__,__LINE__);
			spKDArrayDestroy(return_array, length, i);
			return NULL;
		}
		qsort(comp_array, length, sizeof(CompPoint*), spKDArrayCompare);
		for (j = 0; j < length; j++) {
			return_array->sort_mat[i][j] = (comp_array[j])->index;
		}
		DestroyCompArray(comp_array, length);
	}
	return return_array;
}


int spKDArraySplit(SPKDArray* array, SPKDArray* part_1, SPKDArray* part_2, int coor) {
	int num_left, num_right;
	int* X, *map1 , *map2;

	if ((coor < 0) || (array == NULL) || (coor >= array->dim)  || (part_1 == NULL) || (part_2 == NULL)) {
		spLoggerPrintError(LOG_ERR_MSG_BAD_ARGS, __FILE__, __func__, __LINE__);
		free(part_1);
		free(part_2);
		return -1;
	}
	num_left = spKDArrayGetUpperValue(array->num_points);
	num_right = (array->num_points) - num_left;

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
		spLoggerPrintError(LOG_ERR_MSG_ALLOC, __FILE__,__func__,__LINE__);
		spKDArrayDestroy(part_1, num_left, array->dim);
		spKDArrayDestroy(part_2, num_right, array->dim);
		return -1;
	}
	spKDArraySetBooleanArray(array, X, num_left, coor);
	spKDArraySetPointsArray(array, part_1, part_2, X);

	map1 = (int*)malloc(sizeof(int) * (array->num_points));
	if (map1 == NULL) {
		spLoggerPrintError(LOG_ERR_MSG_ALLOC, __FILE__,__func__,__LINE__);
		free(X);
		spKDArrayDestroy(part_1, num_left, array->dim);
		spKDArrayDestroy(part_2, num_right, array->dim);
		return -1;
	}
	map2 = (int*)malloc(sizeof(int) * (array->num_points));
	if (map2 == NULL) {
		spLoggerPrintError(LOG_ERR_MSG_ALLOC, __FILE__,__func__,__LINE__);
		free(map1);
		free(X);
		spKDArrayDestroy(part_1, num_left, array->dim);
		spKDArrayDestroy(part_2, num_right, array->dim);
		return -1;
	}
	spKDArrayFillingMaps(array, X, map1, map2);
	spKDArraySetMatrices(array, part_1, part_2, X, map1, map2);
	free(X);
	free(map1);
	free(map2);
	return 0;
}



void destroyPointsArray(SPPoint** pointArray, int arrayLength)
{
	if (!pointArray) { return; }
	int i;
	for (i = 0; i < arrayLength; ++i)
	{
		spPointDestroy(pointArray[i]);
	}
	free(pointArray);
}

void destroyPointsArrayArray(SPPoint*** arraysArray, int arrayLength,
                             int* varyingLengths)
{
	if (!arraysArray) {return; }
	int i;
	for (i = 0; i < arrayLength; i++)
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
