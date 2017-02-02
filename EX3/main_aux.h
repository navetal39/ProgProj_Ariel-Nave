#include <cstring>
extern "C" {
	#include "SPPoint.h"
}
/* Error messages */
#define ERR_IMGS_NUM "An error occurred - invalid number of images\n"
#define ERR_BINS_NUM "An error occurred - invalid number of bins\n"
#define ERR_FEATURES_NUM "An error occurred - invalid number of features\n"
#define ERR_ALOC "An error occurred - allocation failure\n"
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
#define NULL_CHAR '\0'
#define TERM_SIGN "#"
#define IMG_PATH_TEMPLATE "%s%s%d%s"
/* Macros */
#define SHOULD_RUN(qp) strcmp((qp), TERM_SIGN)

int readStr(char* buff);
int readInt(int* buff);

int getInputFromUser(char* dirPath, char* imgPrefix, int* imgNum, char* imgSuffix,
					 int* binNum, int* featureNum);

int getQueryPath(char* queryPath);

int getImgData(char* queryPath, int index, int binNum, int featureNum, int* nFeatures,
				 SPPoint*** queryHist, SPPoint*** queryFeatures);

int getImgDataMult(char* dirPath, char* imgPrefix, int imgNum, char* imgSuffix,
			   int binNum, int featureNum, int** featuresPerImage,
			   SPPoint**** imgsHists, SPPoint**** imgsFeatures);

int getBestResults(SPPoint*** imgsHists, SPPoint*** imgsFeatures,
				  SPPoint** queryHist, SPPoint** queryFeatures,
				  int imgNum, int** featuresPerImage,
				  int** bestRGBImages, int** bestSIFTImages, int* bestRGBCount, int* bestSIFTCount);

/**
 * Destroys all the points pointed at by a pointer in the array of point pointers "pointArray",
 * then frees pointArray
 * 
 * Given an array of pointers to points and the length of said array, the function will destroy
 * all the points pointed at by a pointer in the points pointer array using spPointDestroy()
 *
 * @param pointArray - the array of pointers to the points to be destroyed
 * @param arraylength - the length of the point pointers array
 * @return
 * RET_FAIL if pointArray is NULL, else RET_SUCCESS
 */
int destroyPointsArray(SPPoint** pointArray, int arrayLength);

int destroyPointsArrayArray(SPPoint*** featuresArray, int arrayLength, int isFeaturesArray, int* arraysLengths);

int cleanUpMult(SPPoint*** hists, SPPoint*** features);