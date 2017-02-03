#include <cstring>
extern "C" {
	#include "SPPoint.h"
}
/* Error messages */
#define ERR_IMGS_NUM "An error occurred - invalid number of images\n"
#define ERR_BINS_NUM "An error occurred - invalid number of bins\n"
#define ERR_FEATURES_NUM "An error occurred - invalid number of features\n"
#define ERR_ALLOC "An error occurred - allocation failure\n"
/* User instructions */
#define INST_DIR_PATH "Enter images directory path:\n"
#define INST_IMGS_PREFIX "Enter images prefix:\n"
#define INST_IMGS_NUM "Enter number of images:\n"
#define INST_IMGS_SUFFIX "Enter images suffix:\n"
#define INST_BINS_NUM "Enter number of bins:\n"
#define INST_FEATURES_NUM "Enter number of features:\n"
#define INST_QUERY_OR_TERM "Enter a query image or # to terminate:\n"
/* Other printed messages */
#define MSG_BEST_GLOBALS "Nearest images using global descriptors:\n"
#define MSG_BEST_LOCALS "Nearest images using local descriptors:\n"
#define MSG_EXIT "Exiting...\n"
/* Constants */
#define QUERY_INDEX 0
#define RET_SUCCESS 0
#define RET_FAIL -1
#define INPUT_LEN 1024
#define COLOR_NUM 3
#define K_CLOSEST 5
#define NULL_CHAR '\0'
#define SEP_RES ", "
#define SEP_LINE "\n"
#define TERM_SIGN "#"
#define IMG_PATH_TEMPLATE "%s%s%d%s"
/* Macros */
#define SHOULD_RUN(qp) strcmp((qp), TERM_SIGN)

#define ERASE_QUEUE(queue) do{\
		while(!spBPQueueIsEmpty((queue))){\
			spBPQueueDequeue((queue));\
		}\
		spBPQueueDestroy((queue));\
	}while(0);

#define ASSERT_GOOD(val) do{\
		if((val)!=RET_SUCCESS){\
			return RET_FAIL;\
		}\
	}while(0);

#define READ_STR(buff) scanf("%s", (buff));
#define READ_INT(buff) scanf("%d", (buff));
/* Structs */
typedef struct sp_img_sift_score{
	int index;
	int score;
}siftScore;

/**
 * A function to be used for sorting image scores in order to fins closest images using local features.
 * 
 * @param x - a pointer to a siftScore struct
 * @param y - a poitner to a siftScore struct
 * 
 * returns the difference between y's score and x's score
 */
int scoreComp(const void* x, const void* y);

/**
 * Given a character buffer dirPath, a character buffer imgPrefix, an integer buffer imgNum, an character buffer imgSuffex,
 * an integer buffer binNum and an integer buffer featureNum, the function prints instructions to the user, recives input
 * from him/her after each instruction and stores i tin the right buffer. The order of instructions are:
 * 		- enter directory path (stored in dirPath)
 * 		- enter image prefix (stored in imgPrefix)
 * 		- enter number ofimages (stored in imgNum)
 * 		- enter image suffix (stored in imgSuffex)
 * 		- enter number of bins (stored in binNum)
 * 		- enter number of features (stored in featureNum)
 * in the case of an error, the function prints an error message and exists.
 * 
 * @param dirPath - a buffer to store the given path
 * @param imgPrefix - a buffer to store the given prefix
 * @param imgNum - a buffer to store the given nubmer of images
 * @param imgSuffix - a buffer to store the given suffix
 * @param binNum - a buffer to store the given number of bins
 * @param featureNum - a buffer to store the given number of features
 * 
 * return
 * RET_SUCCESS if the operation was successfull, RET_FAIL otherwise.
 * Errors can be caused by:
 * 		- any of the numerical values was not given, was not an integer or was non-positive.
 * 		- number of bins was greater than 255
 */
int getInputFromUser(char* dirPath, char* imgPrefix, int* imgNum, char* imgSuffix,
					 int* binNum, int* featureNum);

/**
 * Given a buffer of characters, the function prints instructions to the user, asking him/her to enter a path to
 * a query image, and then reads it's input into said buffer.
 * 
 * @param queryPath - a buffer to store the user's input.
 */
void getQueryPath(char* queryPath);

/**
 * Given strings representing path to an image, an image index, ana mmount of bins, an ammount of features,
 * and pointers to an integer nFeatures and 2 arrays of arrays of pointers
 * to points imgHists and imgFeatures, the function extracts the color histogram and specified ammount
 * of SIFT features from the image at the given path using spGetRGBHist and spGetSiftDescriptors.
 * It then saves the histogram in the array pointed at by imgHists, the features in the array poitned at by
 * imgFeatures and the number of features extracted nFeatures.
 * 
 * @param imgPath - the path to the image
 * @param binNum - the number of bins in the histogram
 * @param featureNum - the number of features to extract
 * @param nFeatures - a buffer to store the actuall ammount of features that were extracted
 * @param imgsHists - a buffer to store the extracted histogram
 * @param imgsFeatures - a buffer to store the arrays of extracted feature.
 * 
 */
int getImgData(char* imgPath, int index, int binNum, int featureNum, int* nFeatures,
				 SPPoint*** imgHist, SPPoint*** imgFeatures);

/**
 * Given strings representing a directory path, a prefix and a suffix, an ammount of images,
 * and ammount of bins, an ammount of features and pointers to an array of integers featuresPerImage and 2 arrays of arrays of pointers
 * to points imgsHists and imgsFeatures, the function, using getImgData, extracts the color histograms and specified ammount
 * of SIFT features from every image who's path is of the form DPNS such that D is the given directory path, P
 * is the given prefix, S is the given suffix and N is a non-negative integer smaller than the given
 * image ammount. The image indexes of each image read is set to be this number N.
 * It then saves the histograms in the array pointed at by imgsHists, the arrays of features in the array poitned at by
 * imgsFeatures and the features array length in the array pointed at by featuresPerImage, all 3 of which are malloced inside this
 * function.
 * 
 * @param dirPath - the path to the directory of images
 * @param imgPrefix - a prefix of all images names
 * @param imgNum - the number of images
 * @param imgSuffix - a suffix for all images names
 * @param binNum - the number of bins in the histograms
 * @param featureNum - the number of features to extract for each image
 * @param featuresPerImage - a buffer to store the actuall ammount of features that were extracted for each image
 * @param imgsHists - a buffer to store the extracted histograms
 * @param imgsFeatures - a buffer to store the arrays of extracted features.
 * 
 * return
 * RET_SUCCESS if the operation was successfull, RET_FAIL in the case of an allocation error or if getImgData resulted in an error
 * (see it's documentation for more info.)
 */
int getImgDataMult(char* dirPath, char* imgPrefix, int imgNum, char* imgSuffix,
			   int binNum, int featureNum, int** featuresPerImage,
			   SPPoint**** imgsHists, SPPoint**** imgsFeatures);

/**
 * Given color histograms of the images in the database (databaseHists), finds the
 * closest kClosest to a given histogram (queryHist). The function returns the
 * INDEXES of the images to which the closest features belong, stored in ascending order
 * (Closest histogram image index is first, second closest histogram image index is second, etc...).
 * Assumptions:
 *   - Tie break - In case queryHist has the same distance (L2Squared distance) from two histograms,
 *     then the histogram that corresponds to the smallest image
 *     index in the database is closer.
 *
 *   - databaseHists is an array of histograms , it's length is given by numberOfImages. 
 *
 * @param kClosest - The kClosest histograms to the queryHist
 * @param queryHist - A histogram which will be compared with the other histograms
 * @param databaseHists - All histograms in the database. 
 * @param numberOfImages - The number of images in the database. (Number of entries in databaseFeatures)
 *
 * @return - NULL if either the following occurs:
 * 			   * queryHist is NULL
 * 			   * databaseHists is NULL
 * 			   * numberOfImages <= 1
 * 			   * allocation error occurred
 * 			 
 *         - Otherwise- an array of size kClosest is returned such that:
 * 			 * Given that h1, h2, ... the closest histograms to queryHist (i.e d(queryHist,h1) <= d(queryHist,h2) <= ...)
 * 			 * i1, i2, .... are the indexes of the images to which fi belongs (i.e h1 is a histogram of image i1,
 * 			   h2 is a histogram of image i2 etc..)
 * 			 Then the array returned is {i1,i2,...,i_kClosest}
 */
int* bestHISTL2SquaredDistance(int kClosest, SPPoint** queryHists, SPPoint*** databaseHists, int numberOfImages);

/**
 * Given a message (String), an array of integers and the number of integers to be printed the function prints the message
 * and then, in a new line, the specified number of integers from the array, starting from the 1st, seperating the integers
 * with a comma and a space (with a new line at the end).
 * 
 * @param msg - the message to be pritned first
 * @param results - the array of integers to be printed
 * @param resultsLen - the ammount of integers from results to be printed
 */
void printBestResults(char* msg, int* results, int resultsLen);

/**
 * Given an array feature arrays and an array of histograms of database images, a feature array and a histogram
 * of a query image, the number of the database images, the ammount of features in each feature array given and
 * the number of features in the query feature array, the function computes the closest images to the query image
 * using bestHISTL2SquaredDistance and spGetSiftDescriptors and prints the results using printBestResults.
 * 
 * @param imgsHists - an array of histograms for database images
 * @param imgsFeatures - an array of arrays of SIFT features for databse images
 * @param queryHist - a histogram for query image
 * @param queryFeatures - an array of SIFT features for query image
 * @param imgNum - the ammount of database images
 * @param featuresPerImage - an array of integers such that the ith integer is the length of the ith array in imgsFeatures
 * @param featuresInQuery - the length of queryFeatures
 * 
 * return:
 * RET_SUCCESS if the operation was successful, RET_FAIL if an error occured (allocation error or if one of the specified
 * used functions returns an error. See their documentation for more info.)
 */
int getBestResults(SPPoint*** imgsHists, SPPoint*** imgsFeatures,
				  SPPoint** queryHist, SPPoint** queryFeatures,
				  int imgNum, int* featuresPerImage, int featuresInQuery);

/**
 * Destroys all the points pointed at by a pointer in the array of point pointers "pointArray",
 * then frees pointArray
 * 
 * Given an array of pointers to points and the length of said array, the function will destroy
 * all the points pointed at by a pointer in the points pointer array using spPointDestroy()
 * and then free the array itself.
 * 
 * @param pointArray - the array of pointers to the points to be destroyed
 * @param arraylength - the length of the point pointers array
 */
void destroyPointsArray(SPPoint** pointArray, int arrayLength);

/**
 * For each array pointed to by a member of arraysArray, this function destroys all points pointed to by
 * a pointer in said array, then frees it. Finally, it frees arraysArray itself.
 * 
 * Given an array of arrays of pointers to points, the length of said array, a boolean flag that says wether
 * the given array is an array of features arrays (1) or histograms (0) and the lengths of the feature arrays
 * (if the flag is 1), the function will destroy each of the points array using destroyPointsArray, then free
 * the "main" array itself
 * 
 * @param arraysArray - the array of arrays of pointers to the points to be destroyed
 * @param arraylength - the length of the array array
 * @param isFeaturesArray - a flag saying wether arraysArray contains features array (1) or histograms (0)
 * @param arraysLengths - if isFeaturesArray == 1, arraysLengths should contain the lengths of the features arrays.
 */
void destroyPointsArrayArray(SPPoint*** arraysArray, int arrayLength, int isFeaturesArrays, int* arraysLengths);