#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "main_aux.h"
extern "C" {
	#include "spConfig.h"
	#include "SPBPriorityQueue.h"
}

int scoreComp(const void* x, const void* y)
{
	return (((siftScore*)y)->score-((siftScore*)x)->score);
}

SPConfig initConfig(char* cfgPath)
{
	SP_CONFIG_MSG configMsg;
	SPConfig cfg;
	cfg = spConfigCreate(cfgPath, &configMsg);
	if(configMsg == SP_CONFIG_CANNOT_OPEN_FILE)
	{
		printf(ERR_CFG_OPEN, (argc>1)?ERR_CFG_OPEN_D:ERR_CFG_OPEN_C, cfgFileName);
	}
	return cfg;
}
SP_LOGGER_MSG initLog(SPConfig config)
{
	int lvlInt;
	char* filePath;
	SP_CONFIG_MSG configMsg;
	SP_LOGGER_LEVEL lvl = NULL;
	lvlInt = spConfigGetLogLevel(config, &configMsg);
	filePath = spConfigGetLogFile(config, &configMsg);
	switch lvlInt
	{
		case 1:
			lvl = SP_LOGGER_ERROR_LEVEL;
			break;
		case 2:
			lvl = SP_LOGGER_WARNING_ERROR_LEVEL;
			break;
		case 3:
			lvl = SP_LOGGER_INFO_WARNING_ERROR_LEVEL;
			break;
		case 4:
			lvl = SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL;
			break;
		default:
			/* Unreachable, only here to make the code prettier */
			break;
	}
	return spLoggerCreate(filePath, lvl);

}
void getQueryPath(char* queryPath)
{
	printf(INST_QUERY_OR_TERM);
	/* Anything after strlen should already be 0: */
	memset(queryPath, NULL_CHAR, strlen(queryPath));
	READ_STR(queryPath);
}

void printNearestIndexes(SPConfig config, int* indexes)
{
	SP_CONFIG_MSG configMsg;
	char bestPath[MAX_QUERY_LENGTH+1];
	bool useMinGui = spConfigMinimalGui(config, &configMsg);
	/* TODO check message? */
	if(!useMinGui)
	{
		printf(OUT_TITLE);
	}
	while(*indexes>=0)
	{
		memset(bestPath, NULL_BYTE, MAX_QUERY_LENGTH+1);
		configMsg = spConfigGetImagePath(bestPath, config, *indexes);
		/* TODO check message? */
		if(useMinGui)
		{
			showImage(bestPath);
		}else{
			printf("%s\n", bestPath);
		}
		indexes++;
	}
}

int getBestResults(SPPoint*** imgsHists, SPPoint*** imgsFeatures,
				  SPPoint** queryHist, SPPoint** queryFeatures,
				  int imgNum, int* featuresPerImage, int featuresInQuery)
{
	int i, j;
	int* bestIndices, *bestCurrentIndices;
	siftScore* imgScores;
	/* get best results for histograms */
	bestIndices = bestHISTL2SquaredDistance(K_CLOSEST, queryHist, imgsHists, imgNum);
	if(bestIndices == NULL)
	{
		return RET_FAIL;
	}
	/* print best results for histograms */
	printBestResults(MSG_BEST_GLOBALS, bestIndices, K_CLOSEST);
	/* get best results for SIFT features */
	imgScores = (siftScore*)malloc(imgNum*sizeof(siftScore));
	if(imgScores == NULL)
	{
		printf(ERR_ALLOC);
		return RET_FAIL;
	}
	for(i = 0; i<imgNum; i++) // initializing the array of scores
	{
		imgScores[i].index = spPointGetIndex(imgsHists[i][0]);
		imgScores[i].score = 0;
	}
	for(i = 0; i<featuresInQuery; i++)
	{
		bestCurrentIndices = spBestSIFTL2SquaredDistance(K_CLOSEST, queryFeatures[i],
														 imgsFeatures, imgNum, featuresPerImage);
		if(bestCurrentIndices == NULL)
		{
			free(imgScores);
			free(bestIndices);
			return RET_FAIL;
		}
		for(j=0; j<K_CLOSEST; j++) // update the scores
		{
			imgScores[bestCurrentIndices[j]].score++;
		}
		free(bestCurrentIndices);
	}
	/* sort and print best results for SIFT features */
	qsort(imgScores, imgNum, sizeof(siftScore), scoreComp);
	for(i=0; i<K_CLOSEST; i++)
	{
		bestIndices[i] = imgScores[i].index;
	}
	printBestResults(MSG_BEST_LOCALS, bestIndices, K_CLOSEST);
	/* free used memory and return */
	free(imgScores);
	free(bestIndices);
	return RET_SUCCESS;
}

void destroyPointsArray(SPPoint** pointArray, int arrayLength)
{
	int i;
	for(i = 0; i < arrayLength; ++i)
	{
		spPointDestroy(pointArray[i]);
	}
	free(pointArray);
}

void destroyPointsArrayArray(SPPoint*** arraysArray, int arrayLength,
							int isFeaturesArrays, int* varyingLengths)
{
	int i, len;
	for(i = 0; i<arrayLength; i++)
	{
		len = (isFeaturesArrays)?varyingLengths[i]:COLOR_NUM;
		destroyPointsArray(arraysArray[i], len);
	}
	free(arraysArray);
}