#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "main_aux.h"
extern "C" {
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

void getQueryPath(char* queryPath)
{
	printf(INST_QUERY_OR_TERM);
	/* Anything after strlen should already be 0: */
	memset(queryPath, NULL_CHAR, strlen(queryPath));
	READ_STR(queryPath);
}
int getImgData(char* imgPath, int index, int binNum, int featureNum, int* nFeatures,
				 SPPoint*** imgHist, SPPoint*** imgFeatures)
{
	/* get histogram */
	*imgHist = spGetRGBHist(imgPath, index, binNum);
	if(*imgHist == NULL)
	{
		return RET_FAIL;
	}
	/* get features */
	*imgFeatures = spGetSiftDescriptors(imgPath, index, featureNum, nFeatures);
	if(*imgFeatures == NULL)
	{
		destroyPointsArray(*imgHist, COLOR_NUM);
		return RET_FAIL;
	}
	/* return */
	return RET_SUCCESS;
}

int getImgDataMult(char* dirPath, char* imgPrefix, int imgNum, char* imgSuffix,
			   int binNum, int featureNum, int** featuresPerImage,
			   SPPoint**** imgsHists, SPPoint**** imgsFeatures)
{
	int index, retVal;
	char tempImgPath[INPUT_LEN+1];
	/* allocate memory */
	*imgsHists = (SPPoint***)malloc(imgNum * sizeof(SPPoint**));
	if(*imgsHists == NULL)
	{
		printf(ERR_ALLOC);
		return RET_FAIL;
	}
	*imgsFeatures = (SPPoint***)malloc(imgNum * sizeof(SPPoint**));
	if(*imgsFeatures == NULL)
	{
		printf(ERR_ALLOC);
		free(imgsHists);
		return RET_FAIL;
	}
	*featuresPerImage = (int*)malloc(imgNum * sizeof(int));
	if(*featuresPerImage == NULL)
	{
		printf(ERR_ALLOC);
		free(imgsHists);
		free(imgsFeatures);
		return RET_FAIL;
	}
	/* extract data from each image */
	for(index = 0; index < imgNum; index++)
	{
		sprintf(tempImgPath, IMG_PATH_TEMPLATE, dirPath, imgPrefix, index, imgSuffix);
		retVal = getImgData(tempImgPath, index, binNum, featureNum,
							&((*featuresPerImage)[index]), &((*imgsHists)[index]), &((*imgsFeatures)[index]));
		if(retVal != RET_SUCCESS)
		{
			destroyPointsArrayArray(*imgsHists, index, 0, NULL);
			destroyPointsArrayArray(*imgsFeatures, index, 1, *featuresPerImage);
			free(*featuresPerImage);
			return RET_FAIL;
		}
	}
	/* return */
	return RET_SUCCESS;
}

int* bestHISTL2SquaredDistance(int kClosest, SPPoint** queryHist,
							   SPPoint*** databaseHists, int numberOfImages)
{
	/* check if input is valid. */
	if(queryHist == NULL || databaseHists == NULL || numberOfImages <= 1)
	{
		return NULL;
	}
	int i, finalSize, currentIndex;
	int* bestIndices;
	double currentDistance;
	BPQueueElement currElement;
	SPPoint** currentHist;
	SPBPQueue* queue;
	SP_BPQUEUE_MSG msg;
	/* create queue to be used later. */
	queue = spBPQueueCreate(kClosest);
	if(queue == NULL)	// This check allows us to not have to check for "bad argument" return
						//message with Enqueue, Dequeue and Peek.
	{
		printf(ERR_ALLOC);
		return NULL;
	}
	/* enqueue image Indices according to calculated distance. */
	for(i=0; i<numberOfImages; i++)
	{
		currentHist = databaseHists[i];
		currentDistance = spRGBHistL2Distance(currentHist, queryHist);
		currentIndex = spPointGetIndex(currentHist[0]);
		msg = spBPQueueEnqueue(queue, currentIndex, currentDistance);
		if(msg == SP_BPQUEUE_INVALID_ARGUMENT)
		{
			ERASE_QUEUE(queue);
			return NULL;
		}
	}
	/* extract the best images from queue into returning array and get rid of it. */
	finalSize = spBPQueueSize(queue);
	bestIndices = (int*)malloc(finalSize*sizeof(int));
	if(bestIndices != NULL)
	{
		for(i=0; i<finalSize; i++)
		{
			msg = spBPQueuePeek(queue, &currElement);
			if(msg != SP_BPQUEUE_SUCCESS)
			{
				ERASE_QUEUE(queue)
				free(bestIndices);
				return NULL;
			}
			bestIndices[i] = currElement.index;
			msg = spBPQueueDequeue(queue);
		}
	}else{
		printf(ERR_ALLOC);
		// No need to return, since the last 2 line are all that
		// we need so we might as well just let them execute.
	}
	ERASE_QUEUE(queue);
	/* return. */
	return bestIndices;
}
void printBestResults(const char* msg, int* results, int resultsLen)
{
	int i;
	const char* sep;
	printf("%s", msg);
	for(i=0; i<resultsLen; i++)
	{
		sep = (i<resultsLen-1)?SEP_RES:SEP_LINE;
		printf("%d%s", results[i], sep);
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