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
		printf(ERR_IMGS_NUM);
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
		printf(ERR_BINS_NUM);
		return RET_FAIL;
	}
	/* feature count */
	printf(INST_FEATURES_NUM);
	readCount = readInt(featureNum);
	if(readCount <= 0 || *INST_FEATURES_NUM < 1)
	{
		printf(ERR_IMGS_NUM);
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
int getImgData(char* queryPath, int index, int binNum, int featureNum, int* nFeatures,
				 SPPoint*** queryHist, SPPoint*** queryFeatures)
{
	*queryHist = spGetRGBHist(queryPath, index, binNum);
	if(*queryHist == NULL)
	{
		return RET_FAIL;
	}
	*queryFeatures = spGetSiftDescriptors(queryPath, index, featureNum, nFeatures);
	if(*queryFeatures == NULL)
	{
		destroyPointsArray(*queryHist, COLOR_NUM);
		return RET_FAIL;
	}
	return RET_SUCCESS;
}

int getImgDataMult(char* dirPath, char* imgPrefix, int imgNum, char* imgSuffix,
			   int binNum, int featureNum, int** featuresPerImage,
			   SPPoint**** imgsHists, SPPoint**** imgsFeatures)
{
	int index, retVal;
	char tempImgPath[INPUT_LEN+1];
	*imgsHists = (SPPoint***)malloc(imgNum * sizeof(SPPoint**));
	if(*imgsHists == NULL)
	{
		return RET_FAIL;
	}
	*imgsFeatures = (SPPoint***)malloc(imgNum * sizeof(SPPoint**));
	if(*imgsFeatures == NULL)
	{
		free(imgsHists);
		return RET_FAIL;
	}
	*featuresPerImage = (int*)malloc(imgNum * sizeof(int));
	if(*featuresPerImage == NULL)
	{
		free(imgsHists);
		free(imgsFeatures);
		return RET_FAIL;
	}
	for(index = 0; index < imgNum; index++)
	{
		sprintf(tempImgPath, IMG_PATH_TEMPLATE, dirPath, imgPrefix, index, imgSuffix);
		retVal = getImgData(tempImgPath, index, binNum, featureNum, &((*featuresPerImage)[index]), &((*imgsHists)[index]), &((*imgsFeatures)[index]));
		if(retVal != RET_SUCCESS)
		{
			retVal = destroyPointsArrayArray(*imgsHists, index, 0, NULL);
			retVal = destroyPointsArrayArray(*imgsFeatures, index, 1, *featuresPerImage);
			free(*featuresPerImage);
			return RET_FAIL;
		}
	}
	return RET_SUCCESS;
}

int getBestResults(SPPoint*** imgsHists, SPPoint*** imgsFeatures,
				  SPPoint** queryHist, SPPoint** queryFeatures,
				  int imgNum, int** featuresPerImage,
				  int** bestRGBImages, int** bestSIFTImages, int* bestRGBCount, int* bestSIFTCount)
{
	return RET_SUCCESS;
}
int destroyPointsArray(SPPoint** pointArray, int arrayLength)
{
	if(pointArray == NULL)
	{
		return RET_FAIL;
	}
	int i;
	for(i = 0; i < arrayLength; ++i)
	{
		spPointDestroy(pointArray[i]);
	}
	free(pointArray);
	return RET_SUCCESS;
}

int destroyPointsArrayArray(SPPoint*** arraysArray, int arrayLength, int isFeaturesArray, int* varyingLengths)
{
	int i, len;
	if(arraysArray == NULL || (isFeaturesArray && varyingLengths == NULL))
	{
		return RET_FAIL;
	}
	for(i = 0; i<arrayLength; i++)
	{
		len = (isFeaturesArray)?varyingLengths[i]:COLOR_NUM;
		destroyPointsArray(arraysArray[i], len);
	}
	free(arraysArray);
	return RET_SUCCESS;
}

int cleanUpMult(SPPoint*** hists, SPPoint***features)
{
	return RET_SUCCESS;
}