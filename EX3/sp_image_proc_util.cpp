#include <opencv2/imgproc.hpp>//calcHist
#include <opencv2/core.hpp>//Mat
#include <opencv2/xfeatures2d.hpp>//SiftDescriptorExtractor
#include <opencv2/opencv.hpp>
#include <cstdio>
#include <cstdlib>
#include "sp_image_proc_util.h"
extern "C"{
	#include "SPBPriorityQueue.h"
	#include "SPPoint.h"
}
using namespace cv;

/* Error messages */
#define ERR_IMG_LOAD "Image cannot be loaded - %s\n"
#define ERR_ALLOC "An error occurred - allocation failure\n"
/* Constants */
#define COLOR_NUM 3
#define COLOR_RANGE_MAX 256
#define COLOR_RANGE_MIN 0
/* Macros */
#define ERASE_QUEUE(queue) do{\
		while(!spBPQueueIsEmpty((queue)))\
		{\
			spBPQueueDequeue((queue));\
		}\
		spBPQueueDestroy((queue));\
	}while(0);



SPPoint** spGetRGBHist(const char* str,int imageIndex, int nBins)
{
	/* check if input is valid. */
	if(str==NULL || nBins<=0)
	{
		return NULL;
	}
	/* variable decleration. */
	Mat src;
	Mat bgr_planes[COLOR_NUM];
	Mat hists[COLOR_NUM];
	int i, j, k;
	double* data ;
	/* define range of colors. */
	float range[] = {COLOR_RANGE_MIN, COLOR_RANGE_MAX};
	const float* histRange = {range};
	/* define and allocate memory for returning array */
	SPPoint** retArr = (SPPoint**)malloc(COLOR_NUM*sizeof(SPPoint*));
	if(retArr == NULL)
	{
		printf(ERR_ALLOC);
		return NULL;
	}
	/* obtain and split image data. */
	src = imread(str, CV_LOAD_IMAGE_COLOR);
	if(src.empty())
	{
		printf(ERR_IMG_LOAD, str);
		return NULL;
	}
	split(src, bgr_planes);
	/* calculate histograms for each color */
	for(i = 0; i<COLOR_NUM; i++)
	{
		calcHist(&bgr_planes[i], 1, 0, Mat(), hists[i], 1, &nBins, &histRange);
	}
	/* convert data from matrix form to SPPoints form */
	data = (double*)malloc(hists[0].rows * sizeof(double));
	if(data == NULL)
	{
		free(retArr);
		printf(ERR_ALLOC);
		return NULL;
	}
	for(i=0; i<COLOR_NUM; i++)
	{
		for(j=0; j<hists[i].rows; j++)
		{
			data[j] = (double)hists[i].at<float>(j); //, 0);
		}
		retArr[i] = spPointCreate(data, hists[0].rows, imageIndex);
		if(retArr[i] == NULL)
		{
			for(k=0; k<i; k++)
			{
				spPointDestroy(retArr[k]);
			}
			free(data);
			free(retArr);
			return NULL;
		}
	}
	free(data);
	/* return. */
	return retArr;
}

double spRGBHistL2Distance(SPPoint** rgbHistA, SPPoint** rgbHistB)
{
	/* check if input is valid. */
	if(rgbHistA == NULL || rgbHistB == NULL)
	{
		return -1;
	}
	/* variable decleration. */
	int i;
	double distance = 0.0;
	/* calculate the distance. */
	for(i=0;i<COLOR_NUM;i++)
	{
		distance += 0.33*spPointL2SquaredDistance(rgbHistA[i], rgbHistB[i]);
	}
	/* return. */
	return distance;
}

SPPoint** spGetSiftDescriptors(const char* str, int imageIndex, int nFeaturesToExtract, int* nFeatures)
{
	/* check if input is valid. */
	if(str == NULL || nFeaturesToExtract<=0 || nFeatures == NULL)
	{
		return NULL;
	}
	/* variable decleration. */
	int i, j, k, siftDim;
	double* currentFeature;
	SPPoint** features;
	Mat src;
	std::vector<KeyPoint> kp1;
	Mat ds1;
	Ptr<xfeatures2d::SiftDescriptorExtractor> detect;
	/* load image. */
	src = imread(str, CV_LOAD_IMAGE_GRAYSCALE);
	if (src.empty())
	{
		printf(ERR_IMG_LOAD, str);
		return NULL;
	}
	/* extract features. */
	detect = xfeatures2d::SIFT::create(nFeaturesToExtract);
	detect->detect(src, kp1, Mat());
	detect->compute(src, kp1, ds1);
	/* update information about extracted features. */
	*nFeatures = ds1.rows;
	siftDim = ds1.cols;
	/* allocate memory for the returning buffer. */
	features = (SPPoint**)malloc(*nFeatures * sizeof(SPPoint*));
	if (features == NULL)
	{
		printf(ERR_ALLOC);
		return NULL;
	}
	/* convert features from matrix form to SPPoint form. */
	currentFeature = (double*)malloc(siftDim*sizeof(double));
	if(currentFeature == NULL)
	{
		printf(ERR_ALLOC);
		free(features);
		return NULL;
	}
	for (i = 0; i < *nFeatures; i++)
	{
		for (j = 0; j < siftDim; j++)
		{
			currentFeature[j] = (double)ds1.at<float>(i,j);
		}
		features[i] = spPointCreate(currentFeature, siftDim, imageIndex);
		if(features[i] == NULL)
		{
			printf(ERR_ALLOC);
			for(k=0;k<i;k++)
			{
				spPointDestroy(features[i]);
			}
			free(features);
			free(currentFeature);
			return NULL;
		}
	}
	free(currentFeature);
	/* return. */
	return features;
}

int* spBestSIFTL2SquaredDistance(int kClosest, SPPoint* queryFeature, SPPoint*** databaseFeatures,
		int numberOfImages, int* nFeaturesPerImage)
{
	/* check if input is valid. */
	if(queryFeature == NULL || databaseFeatures == NULL || 
		nFeaturesPerImage == NULL || numberOfImages <= 1)
	{
		return NULL;
	}
	int i, j, finalSize, currentIndex;
	int* bestIndices;
	double currentDistance;
	BPQueueElement currElement;
	SPPoint* currentPoint;
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
		for(j=0; j<nFeaturesPerImage[i]; j++)
		{
			currentPoint = databaseFeatures[i][j];
			currentDistance = spPointL2SquaredDistance(currentPoint, queryFeature);
			currentIndex = spPointGetIndex(currentPoint);
			msg = spBPQueueEnqueue(queue, currentIndex, currentDistance);
			if(msg == SP_BPQUEUE_INVALID_ARGUMENT)
			{
				ERASE_QUEUE(queue);
				return NULL;
			}
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