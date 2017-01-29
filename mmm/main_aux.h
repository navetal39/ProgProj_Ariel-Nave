extern "C" {
#include "SPPoint.h"
}
#define IMG_ERROR "An error occurred - invalid number of images\n"
#define BIN_ERROR "An error occurred - invalid number of bins\n"
#define FEAT_ERROR "An error occurred - invalid number of features\n"
#define ALLOC_ERROR "An error occurred - allocation failure\n"
#define INPUT_ERROR "An error occurred - bad input\n"

#define PATH_IN "Enter images directory path:\n"
#define PREF_IN "Enter images prefix:\n"
#define IMG_IN "Enter number of images:\n"
#define SUFF_IN "Enter imgaes suffix:\n"
#define BIN_IN "Enter number of bins:\n"
#define FEAT_IN "Enter number of features:\n"
#define QUERY_IN "Enter a query image or # to terminate:\n"
#define RGB_OUT "Nearest images using global descriptors:\n"
#define SIFT_OUT "Nearest images using local descriptors:\n"
#define EXITING "Exiting...\n"
#define TERMINATE "#"
#define NULL_TERMINATOR '\0'

#define ERR_RETVAL 1
#define SUCCESS_RETVAL 0
#define MIN_RANGE 1
#define MAX_RANGE 255
#define MAXINT_LEN 15
#define K_CLOSES 5
#define INPUT_MAXLEN 1025



int handleInput(char *path, char *pref, int *imgnum, char *suff, int *bin, int *feat);

SPPoint ***generateHistArray(char *dirPath, int imageNum, char *pref, char *suff, int bins);

void destroyRGBHist(SPPoint** hist);

void destroyAllRGB(SPPoint ***histArr, int len);

SPPoint ***generateSIFTArray(char *dirPath, int imageNum, char *pref, char *suff, int feats, int *nFeaturesPerImage);

void destroySIFT(SPPoint **curSift, int nFeatures);

void destroyAllSIFT(SPPoint ***siftArr, int len, int *nFeaturesPerImage);

int* spBestRGBHistL2SquaredDistance(SPPoint** queryHist, SPPoint*** databaseHists, int numberOfImages);

void printArr(int *arr, int len);

int* generateHitList(SPPoint **querySift, SPPoint ***siftArr
	,int featuresNumber, int numberOfImages, int *nFeaturesPerImage);
