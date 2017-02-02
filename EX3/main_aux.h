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
#define RET_SUCCESS 0
#define RET_FAIL 1
#define INPUT_LEN 1024
#define TERM_SIGN "#"
/* Macros */
#define SHOULD_RUN(qp) strcmp((qp), TERM_SIGN)

int getInputFromUser(char* dirPath, char* imgPrefix, int* imgNum, char* imgSuffix,
					 int* binNum, int* featureNum);

int getImgData(char* queryPath, int binNum, int featureNum,
				 SPPoint** queryHist, SPPoint** queryFeatures);

int getImgDataMult(char* dirPath, char* imgPrefix, int* imgNum, char* imgSuffix,
			   int binNum, int featureNum, int* featuresPerImage,
			   SPPoint*** imgsHists, SPPoint*** imgsFeatures);

int getQueryPath(char* queryPath);

int getBestImages(SPPoint*** imgsHists, SPPoint*** imgsFeatures,
				  SPPoint** queryHist, SPPoint** queryFeatures,
				  int imgNum, int* featuresPerImage,
				  int* bestRGBImages, int* bestSIFTImages, int* bestRGBCount, int* bestSIFTCount);
int printResults(int* bestRGBImages, int* bestSIFTImages, int bestRGBCount, int bestSIFTCount);
int cleanUp(SPPoint** hist, SPPoint** features);
int cleanUpMult(SPPoint*** hists, SPPoint*** features);