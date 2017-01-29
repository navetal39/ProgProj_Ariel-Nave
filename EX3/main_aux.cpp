#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "main_aux.h"

/*
   Recursively frees all allocated memory in a 3d array
 */
SPPoint*** GetRGBHistograms(SPPoint*** result_hist, char* path_dir, char* img_prefix, char* img_suffix, int num_images, int nBins) {
	int i , j;
	char tmp_image_path[MAX_IMAGE_PATH_LEN];

	for (i = 0; i < num_images; i++)
	{
		sprintf(tmp_image_path, "%s%s%d%s", path_dir, img_prefix, i, img_suffix);
		result_hist[i] = spGetRGBHist(tmp_image_path, i, nBins);
		if (result_hist[i] == NULL)
		{
			printf(ERROR_ALLOCATION);
			for (j = 0; j < i; j++)
			{
				// need to free all histograms that were already allocated
				RGBHistogramDestroy(result_hist[j], 3);
			}
			free(result_hist);
			return NULL;
		}
	}
	return result_hist;
}

}

SPPoint*** getSiftDescriptors(SPPoint*** result_hist, char* path_dir, char* img_prefix, char* img_suffix, int num_images, int num_features, int* img_num_features) {

	int i , j;
	char tmp_image_path[MAX_IMAGE_PATH_LEN];

	for (i = 0; i < num_images; i++)
	{
		sprintf(tmp_image_path, "%s%s%d%s", path_dir, img_prefix, i, img_suffix);
		result_hist[i] = spGetSiftDescriptors(tmp_image_path, i, num_features, img_num_features + i);
		if (img_sift_descriptors[i] == NULL)
		{
			printf(ERROR_ALLOCATION);
			if (NULL == result_hist[i])
			{
				printf(ERROR_ALLOCATION);
				for (j = 0; j < i; j++)
				{
					HistogOrsAndSiftsDestroy(result_hist[j], img_num_features[i]);
				}
				free(result_hist);
				return NULL;
			}
		}
		return result_hist;
	}
}
}

void HistogramsOrSiftsDestroy(SPPoint** hist_or_sift, int num_histograms)
{
	int i;

	assert(NULL != hist_or_sift);

	for (i = 0; i < num_histograms; ++i)
	{
		spPointDestroy(hist_or_sift[i]);
	}
	free(hist_or_sift);
}


int sift_comparator(const void * a, const void * b)
{
	ImageSiftHits *elem1 = (ImageSiftHits *)a;
	ImageSiftHits *elem2 = (ImageSiftHits *)b;

	if (elem1->hits == elem2->hits)
		return elem1->index - elem2->index;

	if (elem1->hits > elem2->hits) {
		return -1;
	}
	else {
		return 1;
	}
}
/// check
SPBPQueue* PutBestGlobalInQueue(SPPoint** query_rgb_hist, SPPoint*** img_rgb_hist, int num_images) {

	int i;
	SP_BPQUEUE_MSG massage;
	SPBPQueue* best_queue;

	best_queue = spBPQueueCreate(NUM_IMAGES_RETURN + 1);
	// if allocating the queue failed
	if (NULL == best_queue)
	{
		printf(ERROR_ALLOCATION);
		return NULL;
	}

	for (i = 0; i < num_images; i++)
	{
		massage = spBPQueueEnqueue(best_queue, i, spRGBHistL2Distance(query_rgb_hist, img_rgb_hist[i]));
		if (!((SP_BPQUEUE_SUCCESS == massage) || (SP_BPQUEUE_FULL == massage))) {
			printf(ERROR_ALLOCATION);
			return NULL;
		}
	}
	return best_queue;
}
int PrintBestGlobalFromQueue(SPBPQueue* best_queue) {

	BPQueueElement* tmp_queue_element;



	tmp_queue_element = (BPQueueElement*) malloc(sizeof(BPQueueElement));
	// if allocation fails
	if (tmp_queue_element == NULL) {
		return -1;
	}

	printf(PRINT_GLOBAL);

	//prints the first index
	if (!(spBPQueueIsEmpty(best_queue))) {
		if (SP_BPQUEUE_SUCCESS == spBPQueuePeek(best_queue, tmp_queue_element)) {
			printf(ERROR_ALLOCATION);
			FREE_MAIN_DATA;
			FREE_QUERY_DATA;
			free(tmp_queue_element);
			return -1;
		}
		printf("%d", tmp_queue_element->index );
	}
	//prints the next indices
	while (!(spBPQueueIsEmpty(best_queue))) {
		if (SP_BPQUEUE_SUCCESS == spBPQueueDequeue(best_queue)) {
			printf(ERROR_ALLOCATION);
			FREE_MAIN_DATA;
			FREE_QUERY_DATA;
			free(tmp_queue_element);
			return -1;
		}
		if (SP_BPQUEUE_SUCCESS == spBPQueuePeek(best_queue, tmp_queue_element)) {
			printf(ERROR_ALLOCATION);
			FREE_MAIN_DATA;
			FREE_QUERY_DATA;
			free(tmp_queue_element);
			return -1;
		}
		printf(", %d", tmp_queue_element->index );
	}
	free(tmp_queue_element);
	printf("\n");
	return 0;

}

int* PutBestLocalInArray(sift_hits* img_sift_hits, SPPoint*** img_sift_descriptors, SPPoint** query_sift_descriptors, int num_images, int* img_num_features, int query_num_features ) {

	int i, j;
	int best_sift_of_feature[NUM_IMAGES_RETURN];

	// initializing the array
	for (i = 0; i < num_images; i++)
	{
		img_sift_hits[i].hits = 0;
		img_sift_hits[i].index = i;
	}

	for (i = 0; i < num_features; i++)
	{
		best_sift_of_feature = spBestSIFTL2SquaredDistance(NUM_IMAGES_RETURN, query_sift_descriptors[i], img_sift_descriptors, num_images, img_num_features);
		if (best_sift_of_feature == NULL)
		{
			return NULL;
		}
		for (j = 0; j < NUM_IMAGES_RETURN; j++)
		{
			img_sift_hits[best_sift_of_feature[j]].hits++;
		}
	}

	// sorting the images by their number of hits
	qsort(img_sift_hits, num_images, sizeof(sift_hits), sift_comparator);
	return img_sift_hits;

}

int PrintBestLocalFromArray(sift_hits* img_sift_hits) {

	int i;

	printf(PRINT_LOCAL);

	if (NUM_IMAGES_RETURN > 0)
		printf("%d", img_sift_hits[0].index);
	for (i = 1; i < NUM_IMAGES_RETURN; i++)
		printf(", %d", img_sift_hits[i].index);
	printf("\n");

	free(img_sift_hits);

	return 0;
}

DestroyAllHistogramsAndSifts(SPPoint*** img_rgb_hist, SPPoint*** img_sift_descriptors,  int num_images, int* img_num_features) {

	int i;

	assert(NULL != img_rgb_hist && 0 < num_images && NULL != img_sift_descriptors && NULL != img_num_features);

	for (i = 0; i < num_images; ++i)
	{
		HistogramsOrSiftsDestroy(img_rgb_hist[i], 3);
	}

	for (i = 0; i < num_images; ++i)
	{
		destroySiftDescreptor(img_sift_descriptors[i], img_num_features[i]);
	}
	free(img_rgb_hist);
	free(img_sift_descriptors);
	free(img_num_features);
}
/*
   Reads input from the user, and removes '\n' inserted by fgets
 */
void readLine(char* line)
{
	fgets(line, MAX_LINE_LEN, stdin);
	char *pos;
	if ((pos = strchr(line, '\n')) != NULL)
		* pos = '\0';
}
