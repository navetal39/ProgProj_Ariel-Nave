#ifndef MAIN_AUX_H
#define MAIN_AUX_H


#define MAX_LINE_LEN 1024

typedef struct ImageSiftHits
{
	int index;
	int hits;
}sift_hits;

/*
	if 'a' < 'b' : return value is positive
	if 'a' > 'b' : return value is negative
	if 'a' == 'b' : return 0

	comparator for sorting the hits array
*/
int sift_comparator(const void * a, const void * b);

/*
   Recursively frees all allocated memory in a 3d array
 */
void free3DArray(void*** array, int dim0, int dim1);

/*
   Recursively frees all allocated memory in a 2d array
 */
void free2DArray(void** array, int dim0);

/*
   Reads input from the user, and removes '\n' inserted by fgets
 */
void readLine(char* line);

#endif