#ifndef MAIN_AUX_H_
#define MAIN_AUX_H_

#include "sp_image_proc_util.h"

extern "C"
{
	#include "SPPoint.h"
	#include "SPBPriorityQueue.h"
}

#define MAX_LEN 1024

#define INPUT_DIR_MSG "Enter images directory path:\n"
#define INPUT_PREFIX_MSG "Enter images prefix:\n"
#define INPUT_IMG_NUM_MSG "Enter number of images:\n"
#define INPUT_IMG_NUM_ERR_MSG "An error occurred - invalid number of images\n"
#define INPUT_IMG_SUFFIX_MSG "Enter images suffix:\n"
#define INPUT_BIN_NUM_MSG "Enter number of bins:\n"
#define INPUT_BIN_NUM_ERR_MSG "An error occurred - invalid number of bins\n"
#define INPUT_FEAT_NUM_MSG "Enter number of features:\n"
#define INPUT_FEAT_NUM_ERR_MSG "An error occurred - invalid number of features\n"
#define ALLOC_ERR_MSG "An error occurred - allocation failure\n"
#define INPUT_QUERY_IMG_MSG "Enter a query image or # to terminate:\n"
#define NEAR_GLOBAL_DESC_MSG "Nearest images using global descriptors:\n"
#define NEAR_LOCAL_DESC_MSG "Nearest images using local descriptors:\n"
#define EXIT_MSG "Exiting...\n"

#define IMAGE_PATH_TEMPLATE "%s%s%d%s"
#define CHAR_NULL_TERMINATOR '\0'
#define STRING_NULL_TERMINATOR ""
#define CHAR_EXIT '#'
#define STRING_SLASH "/"
#define CLOSEST_K 5

/**
 * The function creates an array of histograms, one histogram for each of
 * the images in the images directory. Each image in the image directory needs
 * to start with the given prefix, then a number between 0 and num_of_images-1
 * and then end with the given suffix.
 *
 * @param images_dir - A string representing the path of the images directory
 * @param prefix - A string representing the prefix of the images in the images directory
 * @param suffix - A string representing the suffix of the images in the images directory
 * @param num_of_images - The total number of images in the images directory
 * @param num_of_bins - The number of bins which will be the total number of bins calculated for the RGB histogram
 *
 * @return
 * NULL if:
 * - One of the images don't start with prefix, a number and then the suffix.
 * - Memory allocation failure
 * Otherwise, an array which has: array[i]==histogram of the i-th image.
 */
SPPoint*** getAllRGBHistograms(char* images_dir, char* prefix, char* suffix, int num_of_images, int num_of_bins);

/*
 * This function returns an array of siftDescriptors of all the pictures in the path
 *
 * @param images_dir - a path to the folder of the pictures
 * @param prefix - the prefix of the images
 * @param suffix - the suffix of the images
 * @param num_of_images - the number of images
 * @param num_of_features - the number of features we wish to extract
 * @param n_features_per_image_array - an array that contains the number of feature that were extracted
 * @return if successful, the function returns a siftDescriptors array such that in the i'th place of
 *         the array the is the siftDescriptor of the picture with the index i, otherwise the function
 *         returns NULL
 */
SPPoint*** getAllSiftDescriptors(char* images_dir, char* prefix, char* suffix, int num_of_images, int num_of_features, int* n_features_per_image_array);

/**
 * Free all memory allocation associated with a given histogram,
 * @assert histogram != NULL
 */
void destroyHistogram(SPPoint** histogram);

/*
 * This function frees the memory of the siftDescreptor and all of its features
 *
 * @param siftDescreptor - a siftDescreptor to free
 * @param num_of_features - the number of features in the siftDescreptor
 */
void destroySiftDescreptor(SPPoint** siftDescreptor, int num_of_features);

/**
 * Checks whether the string consists of only CHAR_EXIT
 * and then a NULL terminator.
 *
 * @param string - The string we check
 *
 * @assert string != NULL
 *
 * @return
 * true if and only if string consists of only CHAR_EXIT and then a NULL terminator.
 */
int checkIfExit(char *string);

/*
 * This function returns the number of digits in n
 *
 * @param n - an integer
 * @return the number of digits in the decimal writing of n
 */
int numOfDigist(int n);

/**
 * Free all memory allocation associated with histogram_array,
 * if point is NULL nothing happens.
 *
 * @param histogram_array - An array of histograms that is going to be freed
 * @param array_length - The number of histograms in the array
 *
 * @assert NULL != histogram_array and 0 < array_length
 *
 * @return
 * true if and only if string consists of only CHAR_EXIT and then a NULL terminator.
 */
void destroyAllHistograms(SPPoint*** histogram_array, int array_length);

/*
 * This function frees the memory of the siftDescreptors int the arry and all of their features
 *
 * @param sift_descriptors_array - a siftDescreptor array to free
 * @param array_length - the length of the array
 * @param n_featuers_per_image - an integer array such that the i'th place of the array
 *                               there is the number of features in the i'th siftDescreptor
 *                               in the siftDescreptor array.
 */
void destroyAllSiftDescreptor(SPPoint*** sift_descriptors_array, int array_length, int* n_featuers_per_image);

/**
 * Computes the L2-Squared distances between the query image histogram and the histograms given in the array.
 * It will print the k closest image indexes based on the L2-squared distance with respect to the histograms.
 *
 * @param histogram_array - An array of histograms, one for each of the images
 * @param num_of_images - The length of the histograms array
 * @param query_histrogram - The query image histogram
 * @param k - The number of closest images that will be printed by the the function
 *
 * @assert NULL != histogram_array and 0 < num_of_images and NULL != query_histrogram and 0 < k
 *
 * @return
 * -1 if there was a memory allocation failure
 * 0 if everything worked as it should have
 */
int printBestHistogramK(SPPoint*** histogram_array, int num_of_images,SPPoint** query_histrogram, int k);

/*
 * This function prints the closest k indexes whose images are the closest to the query image
 *
 * @param sift_descriptors_array - an array of siftDescriptors such that in the i'th place of the
 *                                 array there is the siftDescriptor of the i'th picture
 * @param num_of_images - the number of images
 * @param n_features_per_image_array - an integer array which contains in the i'th place the number
 *                                     of features of the i'th image
 * @param query_features - an array of the features of the query image
 * @param num_of_query_featurs - number of features of the query image
 * @param k - the number of closest images to print
 *
 * @assert NULL != sift_descriptors_array and 0 < num_of_images and NULL != query_features and 0 < k and NULL != n_features_per_image_array
 *
 * @return 0 if successful, -1 otherwise
 */
int printBestSIFTDescriptosK(SPPoint*** sift_descriptors_array, int num_of_images , int* n_features_per_image_array, SPPoint** query_features, int num_of_query_featurs, int k);

/**
 * Tries to extract SIFT descriptors from the query image given by the string image_path (the number of features
 * to retain is given by the integer num_of_features_to_extract).
 * The function returns an array of points of size *num_of_extracted_features, each point represents a certain
 * SIFT feature in the image. Further all points have an index that is -1.
 *
 * @param image_path - A string representing the path of the query image
 * @param num_of_features_to_extract - The number of features to retain
 * @param num_of_extracted_features - A pointer in which the actual number of features retained will be stored.
 *
 * @return
 * NULL if:
 * - image_path is NULL
 * - the image given by str didn't open
 * - num_of_extracted_features is NULL
 * - Memory allocation failure
 * Otherwise, the total number of features retained will be stored in
 * num_of_extracted_features, and the actual features will be returned.
 */
SPPoint** getQueryFeatures(const char* image_path, int num_of_features_to_extract, int* num_of_extracted_features);

/*
 * This function returns the histogram of the image
 *
 * @param image_path - the path of the image
 * @param num_of_bins - the number of bins in the histogram
 *
 * @return if successful, the function returns a histogram of the image
 *         with the given number of bins, otherwise, NULL
 */
SPPoint** getQueryHistogram(const char* image_path, int num_of_bins);


#endif /* MAIN_AUX_H_ */
