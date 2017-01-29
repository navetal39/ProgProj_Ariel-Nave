#include <cstdio>
#include <cstdlib>
#include <cstring>
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
	int query_index = 0;
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
	printf(ENTER_PATH_TO_DIR);
	readLine(path_dir);
	printf(ENTER_IMAGES_PREFIX);
	readLine(img_prefix);
	printf(ENTER_NUM_IMAGES);
	readLine(tmp_input);
	num_images = atoi(tmp_input);
	if (num_images < INPUT_MIN_VALUE){
		printf(ERROR_NUM_IMAGES);
		return -1;
	}
	printf(ENTER_IMAGE_SUFFIX);
	readLine(img_suffix);
	printf(ENTER_NUM_BINS);
	readLine(tmp_input);
	nBins = atoi(tmp_input);
	if (nBins < INPUT_MIN_VALUE){
		printf(ERROR_NUM_BINS);
		return -1;
	}
	printf(ENTER_NUM_FEATURES);
	readLine(tmp_input);
	num_features = atoi(tmp_input);
	if (num_features < INPUT_MIN_VALUE){
		printf(ERROR_NUM_FEATURES);
		return -1;
	}
	ALLOCATE_BUFFERS;
	// global features preproccessing
	img_rgb_hist = GetRGBHistograms(img_rgb_hist, path_dir, img_prefix, img_suffix, num_images, nBins);
	if (NULL == img_rgb_hist){
		// TODO: handle
		return -1;
	}
	// local features preproccessing
	img_sift_descriptors = getSiftDescriptors(img_sift_descriptors, path_dir, img_prefix, img_suffix, num_images, num_features, img_num_features);
	if (img_sift_descriptors == NULL){
		// TODO: check if handled well
		free(img_num_features);
		free(img_rgb_hist);
		return -1;
	}
	// Looping until # is entered
	int debug = 0;
	while (1){
		printf(ENTER_QUERY_OR_TERMINATE);
		readLine(tmp_input);
		printf("main: at at test %d\n", debug++);
		if (strcmp(tmp_input, "#") == 0){
			break;
		}
		printf("main: at at test %d\n", debug++);
		query_rgb_hist = spGetRGBHist(tmp_input, query_index, nBins);
		printf("main: at at test %d\n", debug++);
		if (query_rgb_hist == NULL){
			// TODO: handle;
			return -1;
		}
		printf("main: at at test %d\n", debug++);
		query_sift_descriptors = spGetSiftDescriptors(tmp_input, query_index, num_features, &query_num_features);
		printf("main: at at test %d\n", debug++);
		if (query_sift_descriptors == NULL){
			// TODO: handle;
			return -1;
		}
		printf("main: at at test %d\n", debug++);
		// This part finds the best images using global features
		best_queue = PutBestGlobalInQueue(query_rgb_hist, img_rgb_hist, num_images);
		printf("main: at at test %d\n", debug++);
		if (best_queue == NULL){
			// TODO: handle
		}
		printf("main: at at test %d\n", debug++);
		// print results for global features
		if ((PrintBestGlobalFromQueue(best_queue)) != 0){
			// TODO: handle
			return -1;
		}
		printf("main: at at test %d\n", debug++);
		// For each feature we find to NUM_IMAGES_RETURN best local features,
		// we count hits for every image
		img_sift_hits = PutBestLocalInArray(img_sift_hits, img_sift_descriptors, query_sift_descriptors, num_images, img_num_features, query_num_features);
		printf("main: at at test %d\n", debug++);
		if (img_sift_hits == NULL){
			// TODO: handle
		}
		printf("main: at at test %d\n", debug++);
		if ((PrintBestLocalFromArray(img_sift_hits)) != 0){
			// TODO: handle
			return -1;
		}
		printf("main: at at test %d\n", debug++);
		HistogramsOrSiftsDestroy(query_rgb_hist, 3);
		printf("main: at at test %d\n", debug++);
		HistogramsOrSiftsDestroy(query_sift_descriptors, query_num_features);
		printf("main: at at test %d\n", debug++);
	}
	printf(PRINT_EXIT);
	DestroyAllHistogramsAndSifts(img_rgb_hist, img_sift_descriptors, num_images, img_num_features);
	return 0;
}