#ifndef KDARRAY_H_
#define KDARRAY_H_

#include <stdio.h>
#include <stdlib.h>
#include "SPPoint.h"
#include "SPLogger.h"
#define LOG_ERR_MSG_ALLOC "Memory allocation Failure"
#define LOG_ERR_MSG_BAD_ARGS "Invalid arguments - Can not procceed"
#define LOG_WRN_MSG_NO_DESTROY "Destroyer did not get anything to destroy"

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
 * InitComp                               - creates new CompPoint and initializes the given values to its fields
 * InitCompArray                          - creates new array of CompPoint which is parallal to a given points array
 * DestroyCompArray                       - frees all memory related to a CompPoints array
 * spKDArrayCompare                       - comparator for sorting an array of CompPoints
 * spKDArrayGetUpperValue                 - getting the upper value of a given integer
 * spKDArraySetBooleanArray               - setting an array with 0/1 ,for split method use
 * spKDArrayFillingMaps                   - setting two arrays with values ,for split method use
 * spKDArraySetPointsArray                - setting the points array for two new kd_arrays
 * spKDArraySetMatrices                   - setting the integer matrices for two new kd_arrays
 * spKDArrayDestroy                       - destroys all information connected to a kd_array
 * spKDArrayAllocateArrayFields           - allocates all inforamtion connected to a kd_array
 * spKDArrayCreate                        - creates new kd_array and initialize its information
 * spKDArraySplit                         - splits a kd_array into 2 parts, according to a given coordination
 * destroyPointsArray                     - destroys array of (pointers to) points in given length
 * destroyIntMatrix                       - destroys matrix of integers (with given number of lines)
 *
 */

/** the struct used for kd_array**/
typedef struct sp_kd_array_t {
	SPPoint** points_array;
	int** sort_mat;
	int num_points;
	int dim;
} SPKDArray;

/** the struct used for sorting points by a certain dimension**/
typedef struct compare_point_t {
	SPPoint* point;
	int index;
	int dim;
} CompPoint;

/**
 * create new CompPoint and initializes the given values to its fields
 * @param point        - (pointer to) point, will be coppied and putted in field point of CompPoint
 * 		  index        - will be putted in field index of CompPoint
 * 		  dim          - will be putted in field dim of CompPoint
 * @return
 * a pointer to a new allocated and initialized CompPoint
 * in case of allocation fail returns NULL
 */
CompPoint* InitComp(SPPoint* point, int index, int dim);

/**
 * create new array of CompPoint which is parallal to a given points array
 * @param points_array - array of (pointers to) points, will be coppied and putted in field point of the new array,
 *  the index will be accorsing to the index in points_array
 * 		  length       - the length of the given array
 * 		  dim          - the dimension that will be putted in field dim of the whole CompPoints array
 * @return
 * a pointer to the start of the (pointers to) CompPoints array
 * in case of allocation fail returns NULL
 */
CompPoint** InitCompArray(SPPoint** points_array, int length, int dim);

/**
 * Free all memory related to a CompPoints array
 * @param comp_array  - the CompPoints array need to be destroyed
 		  length 	  - the length of the array
 	if an object need to be freed is already NULL - nothing happens...
 */
void DestroyCompArray(CompPoint** comp_array, int length); 


/**
 * comparator for sorting an array of CompPoints
 * @param - 2 CompPoints (techniqually (void)* )
 * the comparator compares the two elements by their value in the dimensions of the elements (((CompPoint.point)->data)[CompPoint.dim])
 * @return
 *  1  - ((a.point)->data)[a.dim] > ((b.point)->data)[b.dim]
 * -1  - ((a.point)->data)[a.dim] < ((b.point)->data)[b.dim] 
 *  0  - ((a.point)->data)[a.dim] = ((b.point)->data)[b.dim] 
 */
int spKDArrayCompare(const void *val1, const void *val2);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ these are few functions for making the code more modular and readable... ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
 * getting the upper value of a given integer
 * @param - n     - integer 
 * @return
 * Integer - the upper value of n/2
 */
int spKDArrayGetUpperValue(int n);

/**
 * setting an array with 0/1:
 * for a point in index i in array:
 * X[i] = 0  - if the point in index i is lower than the median according to coor dimension
 * X[i] = -1 - if the point in index i is higher than the median according to coor dimension
 * @param - array     - the kd_array which we work on (the points are coming from this array) 
 *		  - X         - the boolean array (techniqually int array) we fill
 *		  - num_left  - the median index
 *		  - coor      - the dimension that determines the order of the points (as menrioned in the description)
 */
void spKDArraySetBooleanArray(SPKDArray* array, int* X, int num_left, int coor);

/**
 * setting map1 and map2 with values:
 * if X[i] = 0:
 * map1[i] = j; where the point i is the k lowest point (according to coor) ,    map2[i] =-1
 * if X[i] = 1:
 * map1[i] = j; where the point i is the k highest point (according to coor),    map2[i] =-1 
 * @param - array     - the kd_array which we work on (the points are coming from this array) 
 *		  - X         - boolean array (techniqually int array)
 *		  - map1      - the int array we fill
 *		  - map2      - the int array we fill
 */
void spKDArrayFillingMaps(SPKDArray* array, int* X, int* map1, int* map2);

/**
 * setting the points array to the kd_arrays part_1 and part_2
 * @param - array     - the kd_array which we work on (the points are coming from this array) 
 *		  - part_1    - kd_array that we fill its point array
 *		  - part_2    - kd_array that we fill its point array
 *		  - X         - boolean array (techniqually int array) that indicates part_1/part_2
 */
void spKDArraySetPointsArray(SPKDArray* array, SPKDArray* part_1, SPKDArray* part_2, int* X);

/**
 * setting the integer matrices to the kd_arrays part_1 and part_2
 * @param - array     - the kd_array which we work on (the points are coming from this array) 
 *		  - part_1    - kd_array that we fill its point array
 *		  - part_2    - kd_array that we fill its point array
 *		  - X         - boolean array (techniqually int array) that indicates part_1/part_2
 *		  - map1      - int array - helps to determine part_1 matrix values
 *		  - map2      - int array - helps to determine part_2 matrix values
 */
void spKDArraySetMatrices(SPKDArray* array, SPKDArray* part_1, SPKDArray* part_2, int* X, int* map1, int* map2);


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ end of this kind of functions here ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


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
















