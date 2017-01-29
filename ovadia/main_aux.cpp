extern "C" {
#include "SPPoint.h"
#include "SPBPriorityQueue.h"
}
#include <cstdio>
#include "sp_image_proc_util.h"
#include "main_aux.h"
#include <cstdlib>
#include <cstring>
#include <cassert>

int handleInput(char *path, char *pref
		,int *imageNum, char *suff, int *bin, int *feat) {
	char imagesNumberAsString[INPUT_MAXLEN], binsNumberAsString[INPUT_MAXLEN], featuresNumberAsString[INPUT_MAXLEN];
	int bins, features;
	memset(path, NULL_TERMINATOR, INPUT_MAXLEN);
	memset(pref, NULL_TERMINATOR, INPUT_MAXLEN);
	memset(suff, NULL_TERMINATOR, INPUT_MAXLEN);
	printf(PATH_IN);
	scanf("%s", path);

	printf(PREF_IN);
	scanf("%s", pref);

	printf(IMG_IN);
	scanf("%s", imagesNumberAsString);
	if ((*imageNum = atoi(imagesNumberAsString)) < MIN_RANGE) {
		printf(IMG_ERROR);
		return (ERR_RETVAL);
	}

	printf(SUFF_IN);
	scanf("%s", suff);

	printf(BIN_IN);
	scanf("%s", binsNumberAsString);
	bins = atoi(binsNumberAsString);
	if (bins < MIN_RANGE || bins > MAX_RANGE) {
		printf(BIN_ERROR);
		return (ERR_RETVAL);
	}
	*bin = bins;

	printf(FEAT_IN);
	scanf("%s", featuresNumberAsString);
	features = atoi(featuresNumberAsString);
	if (features < MIN_RANGE || features > MAX_RANGE) {
		printf(FEAT_ERROR);
		return (ERR_RETVAL);
	}
	*feat = features;
	return SUCCESS_RETVAL;
}

SPPoint ***generateHistArray(char *dirPath,
                             int imageNum, char *pref, char *suff, int bins) {
	//declaring variables
	SPPoint *** histArr;
	char imgName[INPUT_MAXLEN], index[MAXINT_LEN];
	int forLoops, i;
	//allocating memory
	if (!(histArr = (SPPoint***) malloc(imageNum * sizeof(SPPoint**)))) {
		printf(ALLOC_ERROR);
		return NULL;
	}
	//for each image, extracting the proper histogram
	for (forLoops = 0; forLoops < imageNum; ++forLoops) {
		strcpy(imgName,dirPath);
		strcat(imgName, pref);
		sprintf(index, "%d", forLoops);
		strcat(imgName, index);
		strcat(imgName, suff);
		if (!(histArr[forLoops] = spGetRGBHist(imgName, forLoops, bins))) {
			for (i = 0; i < forLoops; ++i) {
				destroyRGBHist(histArr[i]);
			}
			printf(ALLOC_ERROR);
			free(histArr);
			return NULL;
		}
	}
	//returning
	return histArr;
}

void destroyRGBHist(SPPoint** hist) {
	//declaring i variable (for loops)
	int i;
	//making sure hist exists
	assert(hist);
	//destroing the points of the histogram
	for (i = 0; i < 3; ++i) {
		spPointDestroy(hist[i]);
	}
	//and freeing the histogram
	free(hist);
}

void destroyAllRGB(SPPoint ***histArr, int len){
	//declaring variable
	int forLoops;
	//destroying all RGB's
	for(forLoops = 0; forLoops<len; ++ forLoops){
		destroyRGBHist(histArr[forLoops]);
	}
	//freeing the histogram array
	free(histArr);
}

SPPoint ***generateSIFTArray(char *dirPath, int imageNum,
                             char *pref, char *suff, int feats, int *nFeaturesPerImage) {
	//declaring variables
	SPPoint *** siftArr;
	char imgName[INPUT_MAXLEN], index[MAXINT_LEN];
	int forLoops, gorLoops;
	//allocating memory
	if (!(siftArr = (SPPoint***) malloc(imageNum * sizeof(SPPoint**)))) {
		printf(ALLOC_ERROR);
		return NULL;
	}
	//for each image 
	for (forLoops = 0; forLoops < imageNum; ++forLoops) {
		strcpy(imgName,dirPath);
		strcat(imgName, pref);
		sprintf(index, "%d", forLoops);
		strcat(imgName, index);
		strcat(imgName, suff);
		if (!(siftArr[forLoops] = spGetSiftDescriptors(imgName, forLoops, feats, nFeaturesPerImage+forLoops))) {
			for (gorLoops = 0; gorLoops < forLoops; ++gorLoops) {
				destroySIFT(siftArr[gorLoops], nFeaturesPerImage[gorLoops]);
			}
			printf(ALLOC_ERROR);
			free(siftArr);
			return NULL;
		}
	}
	return siftArr;
}
void destroySIFT(SPPoint **curSift, int nFeatures) {
	int forLoops;
	for (forLoops = 0; forLoops < nFeatures; ++forLoops) {
		spPointDestroy(curSift[forLoops]);
	}
	free(curSift);
}

void destroyAllSIFT(SPPoint ***siftArr, int len, int *nFeaturesPerImage){
	int forLoops;
	for(forLoops = 0; forLoops<len; ++ forLoops){
		destroySIFT(siftArr[forLoops], nFeaturesPerImage[forLoops]);
	}
	free(siftArr);
}

int* spBestRGBHistL2SquaredDistance(SPPoint** queryHist,
                                    SPPoint*** databaseHists, int numberOfImages) {
	int forLoops, *toRet;
	SP_BPQUEUE_MSG retVal;
	SPBPQueue *ourQueue;
	BPQueueElement res;
	if (!queryHist || !databaseHists) {
		return NULL;
	}
	
	if (!(ourQueue = spBPQueueCreate(K_CLOSES))) {
		printf(ALLOC_ERROR);
		return NULL;
	}
	for (forLoops = 0; forLoops < numberOfImages; ++forLoops) {
		retVal = spBPQueueEnqueue(ourQueue, forLoops, spRGBHistL2Distance(databaseHists[forLoops], queryHist));
		if (SP_BPQUEUE_INVALID_ARGUMENT == retVal) {
			return NULL;
		}
	}
	if(!(toRet = (int*)malloc(K_CLOSES*sizeof(int)))){
		printf(ALLOC_ERROR);
		spBPQueueDestroy(ourQueue);
		return NULL;
	}
	for (forLoops = 0; forLoops < K_CLOSES; ++forLoops) {
		if (spBPQueuePeek(ourQueue, &res) == SP_BPQUEUE_INVALID_ARGUMENT) {
			spBPQueueDestroy(ourQueue);
			free(toRet);
			return NULL;
		}
		spBPQueueDequeue(ourQueue);
		toRet[forLoops] = res.index;
	}
	//destroying
	spBPQueueDestroy(ourQueue);
	//returning :-)
	return toRet;
}

void printArr(int *arr, int len) {
	int forLoops;
	for (forLoops = 0; forLoops < len - 1; ++ forLoops) {
		printf("%d, ", *(arr+forLoops));
	}
	printf("%d\n", arr[len - 1]);
}

int* generateHitList(SPPoint **querySift, SPPoint ***siftArr, int featuresNumber, int numberOfImages, int *nFeaturesPerImage) {
	//declaring variables
	int forLoops, gorLoops, *curBest, *hitList, *toRet;
	SPBPQueue *ourQueue;
	BPQueueElement res;
	//creating queue for finding the best K_CLOSES images
	if(!(ourQueue = spBPQueueCreate(K_CLOSES))){
		return NULL;
	}
	//allocating memory
	if(!(toRet = (int*)malloc(K_CLOSES*sizeof(int)))){
		printf(ALLOC_ERROR);
		spBPQueueDestroy(ourQueue);
		return NULL;
	}
	if(!(hitList=(int*)malloc(numberOfImages*sizeof(int)))){
		printf(ALLOC_ERROR);
		spBPQueueDestroy(ourQueue);
		free(toRet);
		return NULL;
	}
	//setting memory
	memset(hitList,0,numberOfImages*sizeof(int));
	//going over the features of the query image and counting the number of appearances of each image in the bests-arrs
	for (forLoops = 0; forLoops < featuresNumber; ++ forLoops) {
		curBest = spBestSIFTL2SquaredDistance(K_CLOSES, querySift[forLoops], siftArr, numberOfImages, nFeaturesPerImage);
		for (gorLoops = 0; gorLoops < K_CLOSES; ++gorLoops) {
			++hitList[curBest[gorLoops]];
		}
		free(curBest);
	}
	//extracting the K_CLOSES best image hits using our priority queue
	for(forLoops = 0; forLoops<numberOfImages; ++forLoops){
		if(SP_BPQUEUE_INVALID_ARGUMENT == spBPQueueEnqueue(ourQueue, forLoops, -hitList[forLoops])){
			free(hitList);
			free(toRet);
			spBPQueueDestroy(ourQueue);
			return NULL;
		}
	}
	for (forLoops = 0; forLoops < K_CLOSES; ++forLoops){
		if (spBPQueuePeek(ourQueue, &res) == SP_BPQUEUE_INVALID_ARGUMENT) {
			spBPQueueDestroy(ourQueue);
			free(hitList);
			free(toRet);
			return NULL;
		}
		spBPQueueDequeue(ourQueue);
		toRet[forLoops] = res.index;
	}
	//freeing, destroying and returning :-)
	free(hitList);
	spBPQueueDestroy(ourQueue);
	return toRet;
}
