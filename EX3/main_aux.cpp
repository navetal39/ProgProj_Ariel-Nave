#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "main_aux.h"

/*
   Recursively frees all allocated memory in a 3d array
 */
void free3DArray(void*** array, int dim0, int dim1)
{
	for (int i = 0; i < dim0; i++)
	{
		for (int j = 0; j < dim1; j++)
			free(array[i][j]);

		free(array[i]);
	}

	free(array);
}

int sift_comparator(const void * a, const void * b)
{
	ImageSiftHits *elem1 = (ImageSiftHits *)a;
	ImageSiftHits *elem2 = (ImageSiftHits *)b;

	if(elem1->hits == elem2->hits)
		return elem1->index - elem2->index;

	if(elem1->hits > elem2->hits){
		return -1;
	}
	else{
		return 1;
	}
}


/*
   Recursively frees all allocated memory in a 2d array
 */
void free2DArray(void** array, int dim0)
{
	for (int i = 0; i < dim0; i++)
		free(array[i]);

	free(array);
}

/*
   Reads input from the user, and removes '\n' inserted by fgets
 */
void readLine(char* line)
{
	fgets(line, MAX_LINE_LEN, stdin);
	char *pos;
	if ((pos=strchr(line, '\n')) != NULL)
	    *pos = '\0';
}
