#include "../SPPoint.h"
#include "unit_test_util.h"
#include <stdbool.h>
#include <stdlib.h>

//Checks if copy Works
bool pointBasicCopyTest() {
	double data[2] = { 1.0, 1.0 };
	int dim = 2;
	int index = 1;
	SPPoint* p = spPointCreate(data, dim, index);
	SPPoint* q = spPointCopy(p);
	ASSERT_TRUE(spPointGetIndex(p) == spPointGetIndex(q));
	ASSERT_TRUE(spPointGetDimension(p) == spPointGetDimension(q));
	for (int i = 0; i < spPointGetDimension(p); i++) {
		ASSERT_TRUE(spPointGetAxisCoor(p, i) == spPointGetAxisCoor(q, i));
	}
	spPointDestroy(p);
	spPointDestroy(q);
	return true;
}
bool pointBasicL2Distance() {
	double data1[2] = { 1.0, 1.0 };
	double data2[2] = { 1.0, 0.0 };
	int dim1 = 2;
	int dim2 = 2;
	int index1 = 1;
	int index2 = 1;
	SPPoint* p = spPointCreate(data1, dim1, index1);
	SPPoint* q = spPointCreate(data2, dim2, index2);
	ASSERT_TRUE(spPointL2SquaredDistance(p, p) == 0.0);
	ASSERT_TRUE(spPointL2SquaredDistance(q, q) == 0.0);
	ASSERT_FALSE(spPointL2SquaredDistance(p, q) == 0.0);
	spPointDestroy(p);
	spPointDestroy(q);
	return true;
}

// I must start with create and getters test because all the other tests counts on it

bool pointCreateAndGetTest(){
	int i;
	int j;
	int dim;
	int index;
	double* data;
	SPPoint* check_point;
	for (i = 0; i < 50; i++) {
		index = rand();
		dim = rand() % 50;
		if (dim <= 0) {
			continue;
		}
		data = (double*) malloc(dim * sizeof(double));
		if (data == NULL) {
			return false;
		}
		for (j = 0; j < dim; ++j) {
			data[j] = ((double) rand() / 123);
		}
		check_point = spPointCreate(data, dim, index);
		if (check_point == NULL) {
			return false;
		}

		// the check occurs here - checking all of the 'get' values are identical to values we initialized with

		ASSERT_TRUE(spPointGetIndex(check_point) == index);
		ASSERT_TRUE(spPointGetDimension(check_point) == dim);
		for (j = 0; j < spPointGetDimension(check_point); ++j) {
			ASSERT_TRUE(spPointGetAxisCoor(check_point, j) == data[j]);
		}

		spPointDestroy(check_point);

		// we need to free the data array we used for creating the check point
		free(data);

		// next iteration of loop
	}
	return true;
}


bool pointCopyTest() {
	int i;
	int j;
	int dim;
	int index;
	double* data;
	SPPoint* created_point, coppied_point;
	for (i = 0; i < 50; i++) {
		index = rand();
		dim = rand() % 50;
		if (dim <= 0) {
			continue;
		}
		data = (double*) malloc(dim * sizeof(double));
		if (data == NULL) {
			return false;
		}
		for (j = 0; j < dim; ++j) {
			data[j] = ((double) rand() / 123);
		}
		created_point = spPointCreate(data, dim, index);
		if (created_point == NULL) {
			return false;
		}
		coppied_point = spPointCopy(created_point);
		if (coppied_point == NULL) {
			return false;
		}

		// the check occurs here - checking all of the values of the coppied point are identical to the created point

		ASSERT_TRUE(spPointGetIndex(created_point) == spPointGetIndex(coppied_point));
		ASSERT_TRUE(spPointGetDimension(created_point) == spPointGetDimension(coppied_point));
		for (j = 0; j < spPointGetDimension(coppied_point); ++j) {
			ASSERT_TRUE(spPointGetAxisCoor(created_point, j) == spPointGetAxisCoor(coppied_point, j));
		}

		spPointDestroy(created_point);
		spPointDestroy(coppied_point);

		// we need to free the data array we used for creating the check point
		free(data);

		// next iteration of loop
	}
	return true;
}

bool pointL2DistanceTest() {

	/*  the idea : I assume that if it works with these numbers it will work with any random numbers
		and it makes the test much more readable and easy to understand
	*/
	double data1[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0}
	double data2[6] = {5.0, 4.0, 3.0, 2.0, 1.0, 0.0}
	double data3[6] = {1.0, 1.0, 1.0, -1.0, 0.0, 1.123}
	int dim = 6
	int index_1 = 12;
	int index_2 = 1;
	int index_3 = 3;

	SPPoint point_check_1 = spPointCreate((double*)data1, dim, index_1);
	SPPoint point_check_2 = spPointCreate((double*)data2, dim, index_2);
	SPPoint point_check_3 = spPointCreate((double*)data3, dim, index_3);

	ASSERT_TRUE(spPointL2SquaredDistance(point_check_1, point_check_2) == 55.0);
	ASSERT_TRUE(spPointL2SquaredDistance(point_check_3, point_check_2) == 40.261129);
	ASSERT_TRUE(spPointL2SquaredDistance(point_check_1, point_check_3) == 5.261129);

	spPointDestroy(point_check_1);
	spPointDestroy(point_check_2);
	spPointDestroy(point_check_3);
	return true;
}

int main() {
	RUN_TEST(pointBasicCopyTest);
	RUN_TEST(pointBasicL2Distance);
	RUN_TEST(pointCreateAndGetTest);
	RUN_TEST(pointCopyTest);
	RUN_TEST(pointL2DistanceTest);
	return 0;
}


