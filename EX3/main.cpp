#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sp_image_proc_util.h"
#include "main_aux.h"
extern "C" {
#include "SPBPriorityQueue.h"
#include "SPPoint.h"
}

int main()
{

	char path_dir[MAX_LINE_LEN];
	char img_prefix[MAX_LINE_LEN];
	char img_suffix[MAX_LINE_LEN];
	char tmp_input[MAX_LINE_LEN];

	int num_images = -1;
	int nBins = -1;
	int num_features = -1;
	int i, j;
	int query_index = -1;


	// Main data
	SPPoint*** img_rgb_hist;
	SPPoint*** img_sift_descriptors;
	int* img_num_features;
	sift_hits* img_sift_hits;

	// Query data
	SPPoint** query_rgb_hist;
	int query_num_features = -1;
	SPPoint** query_sift_descriptors;

	// Loop data
	SPBPQueue* best_queue;


	printf(ENTER_PATH_TO_DIR );
	readLine(path_dir);

	printf(ENTER_IMAGES_PREFIX);
	readLine(img_prefix);

	printf(ENTER_NUM_IMAGES);
	readLine(tmp_input);
	num_images = atoi(tmp_input);
	if (num_images < INPUT_MIN_VALUE)
	{
		printf(ERROR_NUM_IMAGES);
		return -1;
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
	printf(ENTER_NUM_FEATURES);
	readLine(tmp_input);
	num_features = atoi(tmp_input);
	if (num_features < INPUT_MIN_VALUE)
	{
		printf(ERROR_NUM_FEATURES);
		return -1;
	}

	// allocating
	img_rgb_hist = (SPPoint***)malloc(num_images * sizeof(SPPoint**));
	img_sift_descriptors = (SPPoint***)malloc(num_images * sizeof(SPPoint**));
	img_num_features = (int*) malloc(num_images * sizeof(int));
	img_sift_hits = (sift_hits*) malloc(num_images * sizeof(sift_hits));

	// if malloc failed:
	if (img_rgb_hist == NULL || img_sift_descriptors == NULL || img_num_features == NULL || img_sift_hits == NULL)
	{
		// free all allocation that succeeded
		printf(ERROR_ALLOCATION);
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

	// global features preproccessing
	img_rgb_hist = GetRGBHistograms(img_rgb_hist, path_dir, img_prefix, img_suffix, num_images, nBins);
	if (NULL == img_rgb_hist)
	{
		/////////////////////////////handle
		return -1;
	}

	// local features preproccessing
	img_sift_descriptors = getSiftDescriptors(img_sift_descriptors, path_dir, img_prefix, img_suffix, num_images, num_features, img_num_features);
	if (NULL == siftDescriptorsArray)
	{
		//////////////////////////////check if handled well
		free(img_num_features);
		free(img_rgb_hist);
		return -1;
	}
	// Looping until # is entered

	while (1)
	{

		printf(ENTER_QUERY_OR_TERMINATE);
		readLine(tmp_input);
		if (strcmp(tmp_input, "#") == 0) {
			break;
		}
		query_rgb_hist = spGetRGBHist(tmp_input, query_index, nBins);
		query_sift_descriptors = spGetSiftDescriptors(tmp_input, query_index, num_features, &query_num_features);

		if (query_rgb_hist == NULL || query_sift_descriptors == NULL)
		{
			printf(ERROR_ALLOCATION);
			FREE_MAIN_DATA;
			FREE_QUERY_DATA;
			return -1;
		}

		// This part finds the best images using global features
		best_queue = PutBestGlobalInQueue(query_rgb_hist, img_rgb_hist, num_images);
		if (best_queue == NULL) {
			//////////////////////////////////////handle
		}

		// print results for global features
		if ((PrintBestGlobalFromQueue(best_queue)) != 0) {
			//////////////////////////////////////handle
			return -1;
		}

		// For each feature we find to NUM_IMAGES_RETURN best local features,
		// we count hits for every image
		img_sift_hits = PutBestLocalInArray(img_sift_hits, img_sift_descriptors, query_sift_descriptors, num_images, img_num_features, query_num_features)
		if (img_sift_hits == NULL) {
			//////////////////////////////////////handle
		}

		if ((PrintBestLocalFromArray(img_sift_hits)) != 0) {
			//////////////////////////////////////handle
			return -1;
		}
		HistogramsAndSiftsDestroy(query_rgb_hist, 3);
		destroySiftDescreptor(query_sift_descriptors, query_num_features);
	}
	printf(PRINT_EXIT);
	DestroyAllHistogramsAndSifts(img_rgb_hist, img_sift_descriptors, num_images, img_num_features);
	return 0;
}