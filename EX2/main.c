/*
 * TODO:
 * Finish implementation main_aux.c
 * Finish documentation main_aux.h
 * -	Update code in main.c to check everything done by main_aux was
 * 		done correctly and without errors, and act accordingly if it is
 * 		not the case
 * Debug, debug, debug
*/

#include "SSPoint.h"
#include "SSBPriorityQueue.h"
#include "main_aux.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char* argv)
{
	/* Declerations: */
	int n, d, k, index, pointsCount = 0;
	int* nearest;
	SPPoint** pointsArray;
	SPPoint* targetPT;
	/* Assertions, etc: */
	if(scanf("%d %d %d", &n. &d, &k) != 3)
	{
		printf("ERROR: Expected 3 integer values seperated by spaces. Exiting...");
		return -1;
	}
	assert(n>=1);
	assert(d>=1);
	assert(1<=k && k<=n);
	/* Point data collection: */
	pointsArray = readPoints(n,d, &pointsCount);
	if(pointsArray = NULL)
	{
		printf("ERROR: An error occured while trying to read points. Exiting...");
	}
	targetPT = readPoint(d);
	if(targetPT == NULL)
	{
		printf("ERROR: An error occured while trying to read target point. Exiting...");
	}
	/* Calculating: */
	nearest = find_KNN(pointsArray, targetPT, n, d, k); ////////////
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
	cleanPointsArray(pointsArray, n); ///////////
}