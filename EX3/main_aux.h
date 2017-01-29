#ifndef MAIN_AUX_H
#define MAIN_AUX_H


extern "C" {
	#include "SPBPriorityQueue.h"
	#include "SPPoint.h"
}


#define ENTER_PATH_TO_DIR "Enter images directory path:\n"
#define ENTER_IMAGES_PREFIX "Enter images prefix:\n"
#define ENTER_IMAGE_SUFFIX "Enter images suffix:\n"
#define ENTER_NUM_BINS "Enter number of bins:\n"
#define ENTER_NUM_IMAGES "Enter number of images:\n"
#define ENTER_NUM_FEATURES "Enter number of features:\n"
#define ENTER_QUERY_OR_TERMINATE "Enter a query image or # to terminate:\n"

#define ERROR_NUM_BINS "An error occurred - invalid number of bins\n"
#define ERROR_NUM_IMAGES "An error occurred - invalid number of images\n"
#define ERROR_NUM_FEATURES "An error occurred - invalid number of features\n"
#define ERROR_ALLOCATION "An error occurred - allocation failure\n"

#define ERROR_QUEUE "An error occurred - queue function failed"

#define PRINT_GLOBAL "Nearest images using global descriptors:\n"
#define PRINT_LOCAL "Nearest images using local descriptors:\n"
#define PRINT_EXIT "Exiting...\n"

#define MAX_IMAGE_PATH_LEN 1024
#define MAX_LINE_LEN 1024
#define NUM_IMAGES_RETURN 5
#define INPUT_MIN_VALUE 1

#define ALLOCATE_BUFFERS\
	do{\
		img_rgb_hist = (SPPoint***)malloc(num_images * sizeof(SPPoint**));\
		img_sift_descriptors = (SPPoint***)malloc(num_images * sizeof(SPPoint**));\
		img_num_features = (int*) malloc(num_images * sizeof(int));\
		img_sift_hits = (sift_hits*) malloc(num_images * sizeof(sift_hits));\
		if (img_rgb_hist == NULL || img_sift_descriptors == NULL || img_num_features == NULL || img_sift_hits == NULL){\
			printf(ERROR_ALLOCATION);\
			if (img_rgb_hist != NULL) {\
				free(img_rgb_hist);\
			}\
			if (img_sift_descriptors != NULL) {\
				free(img_sift_descriptors);\
			}\
			if (img_num_features != NULL) {\
				free(img_num_features);\
			}\
			if (img_sift_hits != NULL) {\
				free(img_sift_hits);\
			}\
			return -1;\
		}\
	}while(0);

typedef struct image_sift_hits_counter
{
	int index;
	int hits;
}sift_hits;


SPPoint*** GetRGBHistograms(SPPoint*** result_hist, char* path_dir, char* img_prefix, char* img_suffix, int num_images, int nBins);


SPPoint*** getSiftDescriptors(SPPoint*** result_hist, char* path_dir, char* img_prefix, char* img_suffix, int num_images, int num_features, int* img_num_features);


void HistogramsOrSiftsDestroy(SPPoint** hist_or_sift, int num_histograms);


SPBPQueue* PutBestGlobalInQueue(SPPoint** query_rgb_hist, SPPoint*** img_rgb_hist, int num_images);


int PrintBestGlobalFromQueue(SPBPQueue* best_queue);


sift_hits* PutBestLocalInArray(sift_hits* img_sift_hits, SPPoint*** img_sift_descriptors, SPPoint** query_sift_descriptors, int num_images, int* img_num_features, int query_num_features);


int PrintBestLocalFromArray(sift_hits* img_sift_hits);


void DestroyAllHistogramsAndSifts(SPPoint*** img_rgb_hist, SPPoint*** img_sift_descriptors,  int num_images, int* img_num_features);

/*
	if 'a' < 'b' : return value is positive
	if 'a' > 'b' : return value is negative
	if 'a' == 'b' : return 0

	comparator for sorting the hits array
*/
int sift_comparator(const void * a, const void * b);

/*
   Reads input from the user, and removes '\n' inserted by fgets
 */
void readLine(char* line);

#endif