#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "unit_test_util.h"

#include "../SPKDArray.h"

static bool main_check() {

	double data[3];
	SPPoint* points_array[6];
	SPKDArray *array;
	SPKDArray *left_1;
	SPKDArray *left_2;
	SPKDArray *right_1;
	SPKDArray *right_2;

	int i;
	int error_flag;

	data[0] = 1;
	data[1] = 6;
	data[2] = 101;
	points_array[0] = spPointCreate(data, 3, 10);
	data[0] = 2;
	data[1] = 5;
	data[2] = 102;
	points_array[1] = spPointCreate(data, 3, 11);
	data[0] = 3;
	data[1] = 4;
	data[2] = 103;
	points_array[2] = spPointCreate(data, 3, 12);
	data[0] = 4;
	data[1] = 3;
	data[2] = 104;
	points_array[3] = spPointCreate(data, 3, 13);
	data[0] = 5;
	data[1] = 2;
	data[2] = 105;
	points_array[4] = spPointCreate(data, 3, 14);
	data[0] = 6;
	data[1] = 1;
	data[2] = 106;
	points_array[5] = spPointCreate(data, 3, 15);

	array = spKDArrayCreate(points_array, 6);

	for (i = 0; i < 6; i++) {
		spPointDestroy(points_array[i]);
	}

	for (i = 0; i < 6; i++) {
		ASSERT_TRUE(array->sort_mat[0][i] == i);
		ASSERT_TRUE(array->sort_mat[1][i] == 5 - i);
		ASSERT_TRUE(array->sort_mat[2][i] == i);
	}

	left_1 = (SPKDArray*)malloc(sizeof(SPKDArray));
	if(left_1 == NULL){
		printf("malloc failed in unit testing :( \n");
		return false;
	}
	right_1 = (SPKDArray*)malloc(sizeof(SPKDArray));
	if(right_1 == NULL){
		printf("malloc failed in unit testing :( \n");
		return false;
	}
	left_2 = (SPKDArray*)malloc(sizeof(SPKDArray));
	if(left_2 == NULL){
		printf("malloc failed in unit testing :( \n");
		return false;
	}
	right_2 = (SPKDArray*)malloc(sizeof(SPKDArray));
	if(right_2 == NULL){
		printf("malloc failed in unit testing :( \n");
		return false;
	}

	error_flag = spKDArraySplit(array, left_1, right_1, 0);
	ASSERT_TRUE(error_flag == 0);

	ASSERT_TRUE(left_1->dim == 3);
	ASSERT_TRUE(left_1->num_points == 3);
	for (i = 0; i < 3; i++) {
		ASSERT_TRUE(spPointGetAxisCoor(left_1->points_array[i], 0) == i + 1);
		ASSERT_TRUE(left_1->sort_mat[0][i] == i);
		ASSERT_TRUE(left_1->sort_mat[1][i] == 2 - i);
		ASSERT_TRUE(left_1->sort_mat[2][i] == i);
	}

	ASSERT_TRUE(right_1->dim == 3);
	ASSERT_TRUE(right_1->num_points == 3);
	for (i = 0; i < 3; i++) {
		ASSERT_TRUE(spPointGetAxisCoor(right_1->points_array[i], 0) == i + 4);
		ASSERT_TRUE(right_1->sort_mat[0][i] == i);
		ASSERT_TRUE(right_1->sort_mat[1][i] == 2 - i);
		ASSERT_TRUE(right_1->sort_mat[2][i] == i);
	}

	error_flag = spKDArraySplit(left_1, left_2, right_2, 0);
	ASSERT_TRUE(error_flag == 0);

	ASSERT_TRUE(left_2->dim == 3);
	ASSERT_TRUE(left_2->num_points == 2);
	for (i = 0; i < 2; i++) {
		ASSERT_TRUE(spPointGetAxisCoor(left_2->points_array[i], 0) == i + 1);
		ASSERT_TRUE(left_2->sort_mat[0][i] == i);
		ASSERT_TRUE(left_2->sort_mat[1][i] == 1 - i);
		ASSERT_TRUE(left_2->sort_mat[2][i] == i);
	}

	ASSERT_TRUE(right_2->dim == 3);
	ASSERT_TRUE(right_2->num_points == 1);
	ASSERT_TRUE(spPointGetAxisCoor(right_2->points_array[0], 0) == 3);
	ASSERT_TRUE(right_2->sort_mat[0][0] == 0);
	ASSERT_TRUE(right_2->sort_mat[1][0] == 0);
	ASSERT_TRUE(right_2->sort_mat[2][0] == 0);

	spKDArrayDestroy(left_2 , 2, 3);
	spKDArrayDestroy(right_2 , 1, 3);
	spKDArrayDestroy(left_1 , 3, 3);
	spKDArrayDestroy(right_1 , 3, 3);
	spKDArrayDestroy(array , 6, 3);
	return true;
}

int main(){
	RUN_TEST(main_check);
	return 0;
}










