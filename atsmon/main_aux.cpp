#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cassert>
#include "main_aux.h"


SPPoint*** getAllRGBHistograms(char* images_dir, char* prefix, char* suffix, int num_of_images, int num_of_bins)
{
	int i, j;
	char current_image_path[MAX_LEN+1];
	SPPoint *** histogram_array;

	if(NULL == (histogram_array = (SPPoint***)malloc(num_of_images*sizeof(SPPoint**))))
	{
		printf(ALLOC_ERR_MSG);
		return NULL;
	}

	memset(current_image_path, CHAR_NULL_TERMINATOR, MAX_LEN+1);

	for (i = 0; i < num_of_images; ++i)
	{
		sprintf(current_image_path, IMAGE_PATH_TEMPLATE, images_dir, prefix, i, suffix);
		histogram_array[i] = spGetRGBHist(current_image_path, i, num_of_bins);
		if(NULL == histogram_array[i])
		{
			printf(ALLOC_ERR_MSG);
			for (j = 0; j < i; ++j)
			{
				destroyHistogram(histogram_array[j]);
			}
			free(histogram_array);
			return NULL;
		}
	}
	return histogram_array;

}

int numOfDigist(int n)
{
	int num_of_dig = 0;
	if(n==0)
	{
		return 1;
	}
	while(n>0)
	{
		++num_of_dig;
		n = n/10;
	}
	return num_of_dig;
}

void destroyHistogram(SPPoint** histogram)
{
	int i;

	assert(NULL != histogram);

	for (i = 0; i < 3; ++i)
	{
		spPointDestroy(histogram[i]);
	}
	free(histogram);
}

SPPoint*** getAllSiftDescriptors(char* images_dir, char* prefix, char* suffix, int num_of_images, int num_of_features, int* n_features_per_image_array)
{
	int i, j;
	char current_image_path[MAX_LEN+1];
	SPPoint *** siftDescriptorsArray;

	if(NULL == (siftDescriptorsArray = (SPPoint***)malloc(num_of_images*sizeof(SPPoint**))))
	{
		printf(ALLOC_ERR_MSG);
		return NULL;
	}

	memset(current_image_path, CHAR_NULL_TERMINATOR, MAX_LEN+1);

	for (i = 0; i < num_of_images; ++i)
	{
		sprintf(current_image_path, IMAGE_PATH_TEMPLATE, images_dir, prefix, i, suffix);
		siftDescriptorsArray[i] = spGetSiftDescriptors(current_image_path, i, num_of_features, n_features_per_image_array+i);
		if(NULL == siftDescriptorsArray[i])
		{
			printf(ALLOC_ERR_MSG);
			for (j = 0; j < i; ++j)
			{
				destroySiftDescreptor(siftDescriptorsArray[j], n_features_per_image_array[i]);
			}
			free(siftDescriptorsArray);
			return NULL;
		}
	}
	return siftDescriptorsArray;
}

void destroySiftDescreptor(SPPoint** siftDescreptor, int num_of_features)
{
	int i;

	assert(NULL != siftDescreptor);

	for (i = 0; i < num_of_features; ++i)
	{
		spPointDestroy(siftDescreptor[i]);
	}
	free(siftDescreptor);
}

int checkIfExit(char *string)
{
	assert(NULL != string);

	if(string[0] == CHAR_EXIT && string[1] == CHAR_NULL_TERMINATOR)
	{
		return 1;
	}
	return 0;
}

void destroyAllHistograms(SPPoint*** histogram_array, int array_length)
{
	int i;
	assert(NULL != histogram_array && 0 < array_length);

	for (i = 0; i < array_length; ++i)
	{
		destroyHistogram(histogram_array[i]);
	}
	free(histogram_array);
}

void destroyAllSiftDescreptor(SPPoint*** sift_descriptors_array, int array_length, int* n_featuers_per_image)
{
	int i;
	assert(NULL != sift_descriptors_array && 0 < array_length && NULL != n_featuers_per_image);

	for (i = 0; i < array_length; ++i)
	{
		destroySiftDescreptor(sift_descriptors_array[i], n_featuers_per_image[i]);
	}
	free(n_featuers_per_image);
	free(sift_descriptors_array);
}

SPPoint** getQueryHistogram(const char* image_path, int num_of_bins)
{
	return spGetRGBHist(image_path, -1, num_of_bins);
}

SPPoint** getQueryFeatures(const char* image_path, int num_of_features_to_extract, int* num_of_extracted_features)
{
	return spGetSiftDescriptors(image_path, -1, num_of_features_to_extract, num_of_extracted_features);
}

int printBestHistogramK(SPPoint*** histogram_array, int num_of_images, SPPoint** query_histrogram, int k)
{
	int i;
	double current_dist;
	SPBPQueue* queue_closest_k;
	int min_value;
	BPQueueElement current_element;

	assert(NULL != histogram_array && 0 < num_of_images && NULL != query_histrogram && 0 < k);

	min_value = k<num_of_images ? k : num_of_images;
	queue_closest_k = spBPQueueCreate(min_value);
	if(NULL == queue_closest_k)
	{
		printf(ALLOC_ERR_MSG);
		return -1;
	}
	for (i = 0; i < num_of_images; ++i)
	{
		current_dist = spRGBHistL2Distance(histogram_array[i], query_histrogram);
		spBPQueueEnqueue(queue_closest_k, histogram_array[i][0]->index, current_dist);
	}
	printf(NEAR_GLOBAL_DESC_MSG);
	for (i = 0; i < min_value-1; ++i)
	{
		spBPQueuePeek(queue_closest_k, &current_element);
		spBPQueueDequeue(queue_closest_k);
		printf("%d, ", current_element.index);
	}
	spBPQueuePeek(queue_closest_k, &current_element);
	spBPQueueDequeue(queue_closest_k);
	printf("%d\n", current_element.index);
	spBPQueueDestroy(queue_closest_k);
	return 0;
}

int printBestSIFTDescriptosK(SPPoint*** sift_descriptors_array, int num_of_images , int* n_features_per_image_array, SPPoint** query_features, int num_of_query_featurs, int k)
{
	int i, j;
	int* hits_per_image;
	int* current_best_dist;
	SPBPQueue* queue_closest_k;
	BPQueueElement current_element;

	assert(NULL != sift_descriptors_array && 0 < num_of_images && NULL != query_features && 0 < k && NULL != n_features_per_image_array);

	if(NULL == (hits_per_image = (int*)malloc(sizeof(int)*num_of_images)))
	{
		printf(ALLOC_ERR_MSG);
		return -1;
	}
	memset(hits_per_image, 0, sizeof(int)*num_of_images);
	for (i = 0; i < num_of_query_featurs; ++i)
	{
		current_best_dist = spBestSIFTL2SquaredDistance(k, query_features[i] , sift_descriptors_array, num_of_images, n_features_per_image_array);
		for (j = 0; j < k; ++j)
		{
			++hits_per_image[current_best_dist[j]];
		}
		free(current_best_dist);
	}

	queue_closest_k = spBPQueueCreate(k);
	if(NULL == queue_closest_k)
	{
		printf(ALLOC_ERR_MSG);
		return -1;
	}
	for (i = 0; i < num_of_images; ++i)
	{
		spBPQueueEnqueue(queue_closest_k, i, -(double)(hits_per_image[i])); //The minus is because the queue keeps the minimal value, and we need the maximal value.
	}
	printf(NEAR_LOCAL_DESC_MSG);
	for (i = 0; i < k-1; ++i)
	{
		spBPQueuePeek(queue_closest_k, &current_element);
		spBPQueueDequeue(queue_closest_k);
		printf("%d, ", current_element.index);
	}
	spBPQueuePeek(queue_closest_k, &current_element);
	spBPQueueDequeue(queue_closest_k);
	printf("%d\n", current_element.index);
	spBPQueueDestroy(queue_closest_k);
	free(hits_per_image);
	return 0;
}




