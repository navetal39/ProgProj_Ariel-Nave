#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "unit_test_util.h"

#include "../SPKDTree.h"


static bool main_rand() {

	printf("starting main_rand...\n");
	double data[3];
	SPPoint* points_array[6];
	SP_KDT_SPLIT method;

	KDTree tree_rand;

	SPPoint* query_point;
	double temp_data[3];
	int* int_arr;
	int i;

	int_arr = (int*)malloc(sizeof(int) * 3);
	if (int_arr == NULL) {
		printf("malloc failed :(\n");
		return false;
	}

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

	method = SP_KD_SPLIT_RANDOM;
	tree_rand = spKDTreeCreate(points_array , 6,  method);
	
	ASSERT_TRUE(tree_rand != NULL);

	for (i = 0; i < 6; ++i) {
		if (points_array[i] != NULL) {
			spPointDestroy(points_array[i]);
		}
	}

	for (i = 0; i < 3; ++i) {
		temp_data[i] = 0;
	}
	query_point = spPointCreate(temp_data, 3, 1000);
	if (query_point == NULL) {
		printf("malloc failed :(\n");
		return false;
	}

	i = spKDTreeKNN(tree_rand, query_point, 2, int_arr);;
	ASSERT_TRUE(int_arr[0] == 10);
	ASSERT_TRUE(int_arr[1] == 11);
	ASSERT_TRUE(int_arr[2] == -1);

	free(int_arr);
	spPointDestroy(query_point);
	spKDTreeDestroy(tree_rand);
	return true;
}

static bool main_inc() {

	printf("starting main_inc...\n");
	double data[3];
	SPPoint* points_array[6];
	SP_KDT_SPLIT method;

	KDTree tree_inc;

	SPPoint* query_point;
	double temp_data[3];
	int* int_arr;
	int i;

	int_arr = (int*)malloc(sizeof(int) * 3);
	if (int_arr == NULL) {
		printf("malloc failed :(\n");
		return false;
	}

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

	method = SP_KD_SPLIT_INCREMENTAL;
	tree_inc = spKDTreeCreate(points_array , 6,  method);



	ASSERT_TRUE(tree_inc != NULL);

	for (i = 0; i < 6; ++i) {
		if (points_array[i] != NULL) {
			spPointDestroy(points_array[i]);
		}
	}

	ASSERT_TRUE(spPointGetIndex(spKDTreeGetData(spKDTreeGetLeft(spKDTreeGetLeft(spKDTreeGetLeft(tree_inc))))) == 11);
	ASSERT_TRUE(spPointGetIndex(spKDTreeGetData(spKDTreeGetRight(spKDTreeGetLeft(spKDTreeGetLeft(tree_inc))))) == 12);
	ASSERT_TRUE(spPointGetIndex(spKDTreeGetData(spKDTreeGetRight(spKDTreeGetLeft(tree_inc)))) == 10);
	ASSERT_TRUE(spPointGetIndex(spKDTreeGetData(spKDTreeGetLeft(spKDTreeGetLeft(spKDTreeGetRight(tree_inc))))) == 14);
	ASSERT_TRUE(spPointGetIndex(spKDTreeGetData(spKDTreeGetRight(spKDTreeGetLeft(spKDTreeGetRight(tree_inc))))) == 15);
	ASSERT_TRUE(spPointGetIndex(spKDTreeGetData(spKDTreeGetRight(spKDTreeGetRight(tree_inc)))) == 13);

	for (i = 0; i < 3; ++i) {
		temp_data[i] = 10;
	}
	query_point = spPointCreate(temp_data, 3, 1000);
	if (query_point == NULL) {
		printf("malloc failed :(\n");
		return false;
	}

	i = spKDTreeKNN(tree_inc, query_point, 2, int_arr);
	ASSERT_TRUE(int_arr[0] == 10);
	ASSERT_TRUE(int_arr[1] == 11);
	ASSERT_TRUE(int_arr[2] == -1);

	free(int_arr);
	spPointDestroy(query_point);
	spKDTreeDestroy(tree_inc);
	return true;
}

static bool main_spread() {

	printf("starting main_spread...\n");
	double data[3];
	SPPoint* points_array[6];
	SP_KDT_SPLIT method;

	KDTree tree_spread;

	SPPoint* query_point;
	double temp_data[3];
	int* int_arr;
	int i;

	int_arr = (int*)malloc(sizeof(int) * 3);
	if (int_arr == NULL) {
		printf("malloc failed :(\n");
		return false;
	}

	data[0] = 1;
	data[1] = 1006;
	data[2] = 1;
	points_array[0] = spPointCreate(data, 3, 10);
	data[0] = 10002;
	data[1] = 1005;
	data[2] = 102;
	points_array[1] = spPointCreate(data, 3, 11);
	data[0] = 10003;
	data[1] = 1004;
	data[2] = 103;
	points_array[2] = spPointCreate(data, 3, 12);
	data[0] = 10004;
	data[1] = 1003;
	data[2] = 104;
	points_array[3] = spPointCreate(data, 3, 13);
	data[0] = 10005;
	data[1] = 1002;
	data[2] = 105;
	points_array[4] = spPointCreate(data, 3, 14);
	data[0] = 10006;
	data[1] = 1;
	data[2] = 106;
	points_array[5] = spPointCreate(data, 3, 15);

	method = SP_KD_SPLIT_MAX_SPREAD;
	tree_spread =  spKDTreeCreate(points_array , 6,  method);

	if (tree_spread == NULL) {
		printf("malloc failed :(\n");
		return false;
	}

	for (i = 0; i < 6; ++i) {
		if (points_array[i] != NULL) {
			spPointDestroy(points_array[i]);
		}
	}

	ASSERT_TRUE(spPointGetIndex(spKDTreeGetData(spKDTreeGetLeft(spKDTreeGetLeft(spKDTreeGetLeft(tree_spread))))) == 10);
	ASSERT_TRUE(spPointGetIndex(spKDTreeGetData(spKDTreeGetRight(spKDTreeGetLeft(spKDTreeGetLeft(tree_spread))))) == 11);
	ASSERT_TRUE(spPointGetIndex(spKDTreeGetData(spKDTreeGetRight(spKDTreeGetLeft(tree_spread)))) == 12);
	ASSERT_TRUE(spPointGetIndex(spKDTreeGetData(spKDTreeGetLeft(spKDTreeGetLeft(spKDTreeGetRight(tree_spread))))) == 15);
	ASSERT_TRUE(spPointGetIndex(spKDTreeGetData(spKDTreeGetRight(spKDTreeGetLeft(spKDTreeGetRight(tree_spread))))) == 14);
	ASSERT_TRUE(spPointGetIndex(spKDTreeGetData(spKDTreeGetRight(spKDTreeGetRight(tree_spread)))) == 13);

	for (i = 0; i < 3; ++i) {
		temp_data[i] = 1;
	}
	query_point = spPointCreate(temp_data, 3, 1000);
	if (query_point == NULL) {
		printf("malloc failed :(\n");
		return false;
	}

	i = spKDTreeKNN(tree_spread, query_point, 2, int_arr);
	ASSERT_TRUE(int_arr[0] == 10);
	ASSERT_TRUE(int_arr[1] == 15);
	ASSERT_TRUE(int_arr[2] == -1);

	free(int_arr);
	spPointDestroy(query_point);
	spKDTreeDestroy(tree_spread);
	return true;
}

int main() {
	RUN_TEST(main_rand);
	RUN_TEST(main_inc);
	RUN_TEST(main_spread);
	return 0;
}