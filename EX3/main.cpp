#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sp_image_proc_util.h"
#include "main_aux.h"



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

int main{

	char path_dir[MAX_LINE_LEN];
	char img_prefix[MAX_LINE_LEN];
	char img_suffix[MAX_LINE_LEN];
	char tmp_input[MAX_LINE_LEN];
	char tmp_image_path[MAX_IMAGE_PATH_LEN];

	int num_images = -1;
	int nBins = -1;
	int num_features = -1;
	int loops_tmp;

	// Main data
	int*** img_rgb_hist;
	double*** img_sift_descriptors;
	int* img_num_features;
	sift_hits* img_sift_hits;

	// Query data
	int** query_rgb_hist;
	int query_num_features = -1;
	double** queryImgSiftDescriptors;

	// Loop data
	int* best_sift_of_feature;

	// The array should store the best images that were found
	SPBPQueue* best_queue;
	BPQueueElement* tmp_queue_element;



	printf(ENTER_PATH_TO_DIR );
	readline(path_dir);

	printf(ENTER_IMAGES_PREFIX);
	readLine(img_prefix);

	printf(ENTER_NUM_IMAGES);
	readLine(tmp_input);
	num_images = atoi(tmp_input);
	if (num_images < INPUT_MIN_VALUE)
	{
		printf(ERROR_NUM_IMAGES);
		return -1;
		//free ???
	}

	printf(ENTER_IMAGE_SUFFIX);
	readLine(img_suffix);

	printf(ENTER_NUM_BINS);
	readLine(tmp_input);

	nBins = atoi(tmp_input);
	if (nBins < INPUT_MIN_VALUE)
	{
		printf(ERROR_NUM_BINS);
		return -1;
	}

	// allocating
	best_queue = spBPQueueCreate(NUM_IMAGES_RETURN + 1);
	img_rgb_hist = (int***) malloc(num_images * sizeof(int**));
	img_sift_descriptors = (double***) malloc(num_images * sizeof(double**));
	img_num_features = (int*) malloc(num_images * sizeof(int));
	img_sift_hits = (sift_hits*) malloc(num_images * sizeof(sift_hits));

	// if malloc failed:
	if (img_rgb_hist == NULL || img_sift_descriptors == NULL || img_num_features == NULL || img_sift_hits == NULL || best_queue == NULL)
	{
		// free all allocation that succeeded
		printf(ERROR_ALLOCATION);
		if (best_queue != NULL) {
			spBPQueueDestroy(best_queue);
		}
		if (img_rgb_hist != NULL) {
			free(img_rgb_hist);
		}
		if (img_sift_descriptors != NULL) {
			free(img_sift_descriptors);
		}
		if (img_num_features != NULL) {
			free(img_num_features);
		}
		if (img_sift_hits != NULL) {
			free(img_sift_hits);
		}
		return -1;
	}

	for (int i = 0; i < num_images; i++)
	{
		sprintf(tmp_image_path, "%s%s%d.%s", dirPath, img_prefix, i, img_suffix);
		img_rgb_hist[i] = spGetRGBHist(tmp_image_path, i, nBins);
		if (img_rgb_hist[i] == NULL)
		{
			printf(ERROR_ALLOCATION);
			free3DArray((void***)img_rgb_hist, num_images, 3);
			free(img_sift_descriptors);
			free(img_num_features);
			free(img_sift_hits);
			return -1;
		}
	}

	// Preprocessing the local features database
	for (int i = 0; i < num_images; i++)
	{
		sprintf(tmp_image_path, "%s%s%d.%s", dirPath, img_prefix, i, img_suffix);
		img_sift_descriptors[i] = spGetSiftDescriptors(tmp_image_path, i, num_features, img_num_features + i);
		if (img_sift_descriptors[i] == NULL)
		{
			printf(ERROR_ALLOCATION);
			free3DArray((void***)img_rgb_hist, num_images, 3);
			for (int j = 0; j < i; j++)
				free2DArray((void**)img_sift_descriptors[j], img_num_features[j]);
			free(img_sift_descriptors);
			free(img_num_features);
			free(img_sift_hits);
			return -1;
		}
	}

	int query_index;
	// Looping until # is entered

	while (1)
	{	

		printf(ENTER_QUERY_OR_TERMINATE);
		readLine(tmp_input);
		if (strcmp(tmp_input, "#") == 0) {
			break;
		}
		// remember to check if Nave returned NULL in alloc fail...
		query_rgb_hist = spGetRGBHist(tmp_input,query_index, nBins);
		queryImgSiftDescriptors = spGetSiftDescriptors(tmp_input, query_index, num_features, &query_num_features);
		if (query_rgb_hist == NULL || queryImgSiftDescriptors == NULL)
		{
			printf(ERROR_ALLOCATION);
			FREE_MAIN_DATA;
			FREE_QUERY_DATA;
			return -1;
		}

		// This part finds the best images using global features

		for (int i = 0; i < num_images; i++)
		{
			if (SP_BPQUEUE_SUCCESS == spBPQueueEnqueue(best_queue, i, spRGBHistL2Distance(query_rgb_hist, img_rgb_hist[i]))) {
				printf(ERROR_ALLOCATION);
				FREE_MAIN_DATA;
				FREE_QUERY_DATA;
				return -1;
			}
			if (spBPQueueIsFull(best_queue)) {
				if (SP_BPQUEUE_SUCCESS == spBPQueueDequeue(best_queue)) {
				printf(ERROR_ALLOCATION);
				FREE_MAIN_DATA;
				FREE_QUERY_DATA;
				return -1;
			}
		}

		// print results for global features
		printf(PRINT_GLOBAL);

		if (!(spBPQueueIsEmpty(best_queue))){
			if (SP_BPQUEUE_SUCCESS == spBPQueuePeek(best_queue, tmp_queue_element)) {
				printf(ERROR_ALLOCATION);
				FREE_MAIN_DATA;
				FREE_QUERY_DATA;
				return -1;
			}
			printf("%d", tmp_queue_element->index );
		}
		while (!(spBPQueueIsEmpty(best_queue))) {
			if (SP_BPQUEUE_SUCCESS == spBPQueueDequeue(best_queue)) {
				printf(ERROR_ALLOCATION);
				FREE_MAIN_DATA;
				FREE_QUERY_DATA;
				return -1;
			}
			if (SP_BPQUEUE_SUCCESS == spBPQueuePeek(best_queue, tmp_queue_element)) != 0) {
				printf(ERROR_ALLOCATION);
				FREE_MAIN_DATA;
				FREE_QUERY_DATA;
				return -1;
			}
			printf(", %d", tmp_queue_element->index );
		}
		printf("\n");

		 This part finds the best images using local features

		// Initializing the image hits array
		for (int i = 0; i < num_images; i++)
		{
			img_sift_hits[i].hits = 0;
			img_sift_hits[i].index = i;
		}

		// For each feature we find to NUM_IMAGES_RETURN best local features,
		// we count hits for every image
		for (int i = 0; i < num_features; i++)
		{
			best_sift_of_feature = spBestSIFTL2SquaredDistance(NUM_IMAGES_RETURN, queryImgSiftDescriptors[i], img_sift_descriptors, num_images, img_num_features);
			if (best_sift_of_feature == NULL)
			{
				FREE_MAIN_DATA;
				FREE_QUERY_DATA;
				return -1;
			}
			for (int j = 0; j < NUM_IMAGES_RETURN; j++)
			{
				img_sift_hits[best_sift_of_feature[j]].hits++;
			}

			free(best_sift_of_feature);
		}

		// sorting the images by their number of hits
		qsort(img_sift_hits, num_images, sizeof(sift_hits), sift_comparator);

		printf(PRINT_LOCAL);
		if(NUM_IMAGES_RETURN > 0)
			printf("%d", img_sift_hits[0].index);
		for(int i = 1; i < NUM_IMAGES_RETURN; i++)
			printf(", %d", img_sift_hits[i].index);
		printf("\n");

		// free query image descriptors
		FREE_QUERY_DATA;

	}

	printf(PRINT_EXIT);
	FREE_MAIN_DATA;
}

}
