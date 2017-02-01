
#include <cstdio>
#include <cstring>
#include <cstdlib>

#include "main_aux.h"

int main()
{
	char in_images_dir_path[MAX_LEN + 1];
	char in_images_prefix[MAX_LEN + 1];
	char in_images_suffix[MAX_LEN + 1];
	char input_query_image[MAX_LEN + 1];
	int in_num_of_images, in_num_of_bins, in_number_of_feat, num_of_query_features;
	int error_code;
	int* n_features_per_image_array;
	SPPoint*** histogram_array;
	SPPoint*** siftDescriptorsArray;
	SPPoint** query_histogram;
	SPPoint** query_fetures;

	memset(in_images_dir_path, CHAR_NULL_TERMINATOR, MAX_LEN + 1);
	memset(in_images_prefix, CHAR_NULL_TERMINATOR, MAX_LEN + 1);
	memset(in_images_suffix, CHAR_NULL_TERMINATOR, MAX_LEN + 1);
	memset(input_query_image, CHAR_NULL_TERMINATOR, MAX_LEN + 1);

	printf(INPUT_DIR_MSG);
	scanf("%s", in_images_dir_path);
	printf(INPUT_PREFIX_MSG);
	scanf("%s", in_images_prefix);
	printf(INPUT_IMG_NUM_MSG);
	scanf("%d", &in_num_of_images);
	if(1>in_num_of_images)
	{
		printf(INPUT_IMG_NUM_ERR_MSG);
		return -1;
	}
	printf(INPUT_IMG_SUFFIX_MSG);
	scanf("%s", in_images_suffix);
	printf(INPUT_BIN_NUM_MSG);
	scanf("%d", &in_num_of_bins);
	if(1>in_num_of_bins)
	{
		printf(INPUT_BIN_NUM_ERR_MSG);
		return -1;
	}
	printf(INPUT_FEAT_NUM_MSG);
	scanf("%d", &in_number_of_feat);
	if(1>in_num_of_bins)
	{
		printf(INPUT_FEAT_NUM_ERR_MSG);
		return -1;
	}

	histogram_array = getAllRGBHistograms(in_images_dir_path, in_images_prefix, in_images_suffix, in_num_of_images, in_num_of_bins);
	if(NULL == histogram_array)
	{
		return -1;
	}

	if(NULL == (n_features_per_image_array = (int*)malloc(sizeof(int)*in_num_of_images)))
	{
		printf(ALLOC_ERR_MSG);
		return -1;
	}
	siftDescriptorsArray = getAllSiftDescriptors(in_images_dir_path, in_images_prefix, in_images_suffix, in_num_of_images, in_number_of_feat, n_features_per_image_array);
	if(NULL == siftDescriptorsArray)
	{
		free(n_features_per_image_array);
		free(histogram_array);
		return -1;
	}

	printf(INPUT_QUERY_IMG_MSG);
	scanf("%s", input_query_image);

	while(!(checkIfExit(input_query_image)))
	{
		query_histogram = getQueryHistogram(input_query_image, in_num_of_bins);
		if(NULL == query_histogram)
		{
			printf(ALLOC_ERR_MSG);
			return -1;
		}
		error_code = printBestHistogramK(histogram_array, in_num_of_images,query_histogram, CLOSEST_K);
		if(0 > error_code)
		{
			return -1;
		}

		query_fetures = getQueryFeatures(input_query_image, in_number_of_feat, &num_of_query_features);
		error_code = printBestSIFTDescriptosK(siftDescriptorsArray, in_num_of_images, n_features_per_image_array, query_fetures, num_of_query_features, CLOSEST_K);
		if(0 > error_code)
		{
			return -1;
		}
		destroyHistogram(query_histogram);
		destroySiftDescreptor(query_fetures, num_of_query_features);
		printf(INPUT_QUERY_IMG_MSG);
		scanf("%s", input_query_image);
	}
	printf(EXIT_MSG);
	destroyAllHistograms(histogram_array, in_num_of_images);
	destroyAllSiftDescreptor(siftDescriptorsArray, in_num_of_images, n_features_per_image_array);
	return 0;
}


