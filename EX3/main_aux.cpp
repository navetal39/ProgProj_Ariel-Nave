#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "main_aux.h"
#include "sp_image_proc_util.h"
extern "C" {
	#include "SPPoint.h"
	#include "SPBPriorityQueue.h"
}

int getInputFromUser(char* dirPath, char* imgPrefix, int* imgNum, char* imgSuffix,
					 int* binNum, int* featureNum)
{
	return 1;
}

int getImgData(char* queryPath, int binNum, int featureNum,
				 SPPoint** queryHist, SPPoint** queryFeatures)
{
	return 1;
}

int getImgDataMult(char* dirPath, char* imgPrefix, int* imgNum, char* imgSuffix,
			   int binNum, int featureNum, int* featuresPerImage,
			   SPPoint*** imgsHists, SPPoint*** imgsFeatures)
{
	return 1;
}

int getQueryPath(char* queryPath);
{
	return 1;
}

int getBestImages(SPPoint*** imgsHists, SPPoint*** imgsFeatures,
				  SPPoint** queryHist, SPPoint** queryFeatures,
				  int imgNum, int* featuresPerImage,
				  int* bestRGBImages, int* bestSIFTImages, int* bestRGBCount, int* bestSIFTCount);
{
	return 1;
}
int printResults(int* bestRGBImages, int* bestSIFTImages, int bestRGBCount, int bestSIFTCount);
{
	return 1;
}
int cleanUp(SPPoint** hist, SPPoint** features);
{
	return 1;
}
int cleanUpMult(SPPoint*** hists, SPPoint***features);
{
	return 1;
}