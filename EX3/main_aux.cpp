#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "main_aux.h"
#include "sp_image_proc_util.h"
extern "C" {
	#include "SPPoint.h"
	#include "SPBPriorityQueue.h"
}
int readStr(char* buff)
{
	return scanf("%s", buff);
}
int readInt(int* buff)
{
	return scanf("%d", buff);
}
int getInputFromUser(char* dirPath, char* imgPrefix, int* imgNum, char* imgSuffix,
					 int* binNum, int* featureNum)
{
	int readCount = 0;
	/* path */
	printf(INST_DIR_PATH);
	memset(dirPath, NULL_CHAR, INPUT_LEN+1);
	readCount = readStr(dirPath);
	/* prefix */
	printf(INST_IMGS_PREFIX);
	memset(imgPrefix, NULL_CHAR, INPUT_LEN+1);
	readCount = readStr(dirPath);
	/* image count */
	printf(INST_IMGS_NUM);
	readCount = readInt(imgNum);
	if(readCount <= 0 || *imgNum < 1)
	{
		printf(ERR_ERR_IMGS_NUM);
		return RET_FAIL;
	}
	/* suffix */
	printf(INST_IMGS_SUFFIX);
	memset(imgSuffix, NULL_CHAR, INPUT_LEN+1);
	readCount = readStr(imgSuffix);
	/* bin count */
	printf(INST_BINS_NUM);
	readCount = readInt(binNum);
	if(readCount <= 0 || *binNum < 1 || *binNum > 255)
	{
		printf(ERR_ERR_BINS_NUM);
		return RET_FAIL;
	}
	/* feature count */
	printf(INST_FEATURES_NUM);
	readCount = readInt(featureNum);
	if(readCount <= 0 || *INST_FEATURES_NUM < 1)
	{
		printf(ERR_ERR_IMGS_NUM);
		return RET_FAIL;
	}
	/* return */
	return RET_SUCCESS;
}
int getQueryPath(char* queryPath)
{
	printf(INST_QUERY_OR_TERM);
	readStr(queryPath);
	return RET_SUCCESS;
}
int getImgData(char* queryPath, int binNum, int featureNum,
				 SPPoint** queryHist, SPPoint** queryFeatures)
{
	return RET_SUCCESS;
}

int getImgDataMult(char* dirPath, char* imgPrefix, int* imgNum, char* imgSuffix,
			   int binNum, int featureNum, int* featuresPerImage,
			   SPPoint*** imgsHists, SPPoint*** imgsFeatures)
{
	return RET_SUCCESS;
}

int getBestImages(SPPoint*** imgsHists, SPPoint*** imgsFeatures,
				  SPPoint** queryHist, SPPoint** queryFeatures,
				  int imgNum, int* featuresPerImage,
				  int* bestRGBImages, int* bestSIFTImages, int* bestRGBCount, int* bestSIFTCount);
{
	return RET_SUCCESS;
}
int printResults(int* bestRGBImages, int* bestSIFTImages, int bestRGBCount, int bestSIFTCount);
{
	return RET_SUCCESS;
}
int cleanUp(SPPoint** hist, SPPoint** features);
{
	return RET_SUCCESS;
}
int cleanUpMult(SPPoint*** hists, SPPoint***features);
{
	return RET_SUCCESS;
}