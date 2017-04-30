#ifndef KDARRAY_H_
#define KDARRAY_H_

#include "SPPoint.h"


/**
 * kd_array summary
 *
 * the kd_array contains inforamtion of points
 * the kd_array has an array of (pointers to) points (numbered (0 ,......, num_points-1))
 * the kd_array has an integer matrix (each integer represents a point) -
 * where the i'th line is sorted according to the i'th dimension of the represented points 
 *
 *
 * The following functions are supported:
 *
 * spKDArrayCompare            - compares 2 integers accorsing two global dimension (refering to represented points)
 * spKDArrayDestroy          - destroys all information connected to a kd_array
 * spKDArrayAllocateArrayFields        - allocates all inforamtion connected to a kd_array
 * spKDArrayCreate          - creates new kd_array and initialize its information
 * spKDArraySplit                    - splits a kd_array into 2 parts, according to a given coordination
 * destroyPointsArray     - destroys array of (pointers to) points in given length
 * destroyIntMatrix           - destroys matrix of integers (with given number of lines)
 *
 */

/** the struct used for kd_array**/
typedef struct sp_kd_array_t {
	SPPoint** points_array;
	int** sort_mat;
	int num_points;
	int dim;
} SPKDArray;

/**
 * comparator for sorting an array of integer (that represents points) by a certain dimension (of the represented points)
 * @param (not exactly) - 2 integers - (a,b) - in the range (0, points_dim -1) (points_dim according to the global array)
 * the comparator compares data[i] (where i is global) of the points: global_array[a], global_array[b]
 * @return
 *  1  - in case global_array[a]->data[i] > global_array[b]->data[i] 
 * -1 - in case global_array[a]->data[i] < global_array[b]->data[i] 
 *  0  - in case global_array[a]->data[i] = global_array[b]->data[i] 
 */

int spKDArrayCompare(const void *val1, const void *val2);


/**
 * Free all memory related to a kd_array
 * @param array  - the kd_array to destroy
 		  length - number of malloced points in the kd_array
 		  dim    - number of malloced lines in the kd_array matrix (sort_mat)
 	if an object need to be freed is already NULL - nothing happens...
 */
void spKDArrayDestroy(SPKDArray* array, int length, int dim);

/**
 * allocates all information connected to an array (that was already allocated) - including
 * array of points and a matrix of integers
 * @param return_array - pointer to already allocated kd_array (need to allocate its fields)
 * 		  length       - number of points will be in the points_array of return_array (means points that needs to be malloced)
 * 		  points_dim   - number of lines will be in the sorted matrix of return_array (means lines to be malloced)
 * @return
 * a pointer to the array that was recieved (when its fields are already allocated)
 * in case of allocation fail returns NULL
 */
SPKDArray* spKDArrayAllocateArrayFields(SPKDArray* return_array, int length, int points_dim);

/**
 * creates a kd_array from an array of points (allocation and values initalization) - including
 * points_array (copying the recieved points array), sorted matrix of integers (where
 * each integer represents a point and each line is sorted according to a certain dimension of the points)
 * @param array_of_points - the (pointers to) points that will be in the kd_array
 * 		  length          - number of points in the given kd_array
 * @return
 * a pointer to new array that was initialized
 * in case of allocation fail returns NULL
 */
SPKDArray* spKDArrayCreate(SPPoint** array_of_points, int length);

/**
 * spliting a kd_array into 2 separate smaller kd_arrays that was already allocated (but not their fields)
 * n/2 +1 (upper value of n/2) of the points will be in part_1, the rest in part_2
 * the criteria of the split is the points value in dimension = coor
 * @param array  - a pointer to a complete kd_array that we split into part_1 and part_2
 * 		  part_1 - an allocated kd_array that will include the points with the low values in dimension = coor
 *		  part_1 - an allocated kd_array that will include the points with the low values in dimension = coor
 		  coor   - the coordination that we split accorsing to
 * @return
 * an integer:
 *  0 - means success
 * -1 - means fail
 */
int spKDArraySplit(SPKDArray* array, SPKDArray* part_1, SPKDArray* part_2, int coor);

/**
 * Destroys all the points pointed at by a pointer in the array of point pointers "pointArray",
 * then frees pointArray
 * 
 * Given an array of pointers to points and the length of said array, the function will destroy
 * all the points pointed at by a pointer in the points pointer array using spPointDestroy()
 * and then free the array itself.
 * 
 * @param pointArray - the array of pointers to the points to be destroyed
 * @assert pointArray != NULL
 * @param arraylength - the length of the point pointers array
 */
void destroyPointsArray(SPPoint** pointArray, int arrayLength);

/**
 * For each array pointed to by a member of arraysArray, this function destroys all points pointed to by
 * a pointer in said array, then frees it. Finally, it frees arraysArray itself.
 * 
 * Given an array of arrays of pointers to points, the length of said array and the lengths of the feature arrays
 * (if the flag is 1), the function will destroy each of the points array using destroyPointsArray, then free
 * the "main" array itself
 * 
 * @param arraysArray - the array of arrays of pointers to the points to be destroyed
 * @assert arraysArray != NULL
 * @param arraylength - the length of the array array
 * @param arraysLengths - contains the respective lengths of the features arrays.
 */
void destroyPointsArrayArray(SPPoint*** arraysArray, int arrayLength, int* arraysLengths);

/**
 * Free all memory related to integer matrix
 * @param mat  - the matrix to destroy
 		  dim  - number of malloced lines in the matrix
 	if a line (or the whole matrix) is NULL nothing happens...
 */
void destroyIntMatrix(int** mat, int dim);
#endif
















