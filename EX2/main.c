/*
 * TODO:
 * Figure out what to do with the returned values from the queue functions in main_aux
 * Test, test test
 * Debug, debug, debug
*/

#include "SPPoint.h"
#include "SPBPriorityQueue.h"
#include "main_aux.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

int main()
{
	/* Declerations: */
	int n, d, k, index, pointsCount = 0;
	int* nearest;
	SPPoint** pointsArray;
	SPPoint* targetPT;
	/* Assertions, etc: */
	if(scanf("%d %d %d", &n, &d, &k) != 3)
	{
		printf("ERROR: Expected 3 integer values seperated by spaces. Exiting...");
		return -1;
	}
	assert(n>=1);
	assert(d>=1);
	assert(1<=k && k<=n);
	/* Point data collection: */
	pointsArray = readPoints(n,d, pointsCount);
	if(pointsArray == NULL)
	{
		printf("ERROR: An error occured while trying to read points. Exiting...");
		return -1;
	}
	targetPT = readPoint(d, 0);
	if(targetPT == NULL)
	{
		printf("ERROR: An error occured while trying to read target point. Exiting...");
		return -1;
	}
	/* Calculating: */
	nearest = find_KNN(pointsArray, targetPT, n, k);
	for(index = 0; index < k; ++index)
	{
		printf("%d", nearest[index]);
		if(index<k-1)
		{
			printf(", ");
		}else{
			printf("\n");
		}
	}
	/* Cleaning up: */
	free(nearest);
	spPointDestroy(targetPT);
	destroyPointsArray(pointsArray, n);
	return 0;
}