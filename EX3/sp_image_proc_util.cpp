#include <opencv2/imgproc.hpp>//calcHist
#include <opencv2/core.hpp>//Mat
#include <vector>
#include <cstdio>
#include <cstdlib>
#include "sp_image_proc_util.h"
extern "C"{
	#include "SPBPriorityQueue.h"
}
using namespace cv;

#define COLOR_NUM 3
#define COLOR_RANGE_MAX 256
#define COLOR_RANGE_MIN 0
#define UNLOADABLE_IMAGE "Image cannot be loaded - %s\n"

SPPoint** spGetRGBHist(const char* str,int imageIndex, int nBins)
{
	if(str==NULL || nBins<=0)
	{
		return NULL;
	}
	Mat src;
	Mat hists[COLOR_NUM];
	int i, j;
	float range[] = {COLOR_RANGE_MIN, COLOR_RANGE_MAX};
	const float* histRange = {range};
	SPPoint** retArr = (SPPoint**)malloc(COLOR_NUM*sizeof(SPPoint*));
	if(retArr == NULL)
	{
		return NULL;
	}
	float* data ;

	src = imread(str, CV_LOAD_IMAGE_COLOR);
	if(src.empty())
	{
		printf(UNLOADABLE_IMAGE, str);
		return NULL;
	}
	std::vector<Mat> bgr_planes; // TODO help please
	split(src, bgr_planes);
	calcHist(&bgr_planes[0], 1, 0, Mat(), hists[0], 1, &nBins, &histRange);
	calcHist(&bgr_planes[1], 1, 0, Mat(), hists[1], 1, &nBins, &histRange);
	calcHist(&bgr_planes[2], 1, 0, Mat(), hists[2], 1, &nBins, &histRange);
	data = (float*)malloc(hists[0].rows * sizeof(float))
	for(i=0; i<COLOR_NUM; i++)
	{
		for(j=0; j<hists[0].rows; j++)
		{
			data[j] = hists[i].at<float>(j,0);
		}
		retArr[i] = spPointCreate(data, hists[0].rows, imageIndex);
	}
	free(data);
	return retArr;
}
double spRGBHistL2Distance(SPPoint** rgbHistA, SPPoint** rgbHistB)
{
	if(rgbHistA = NULL || rgbHistB == NULL)
	{
		return -1;
	}
	double distance;
	for(i=0;i<COLOR_NUM;i++)
	{
		distance += 0.33*spPointL2SquaredDistance(rgbHistA[i], rgbHistB[i]);
	}
	return distance;
}
SPPoint** spGetSiftDescriptors(const char* str, int imageIndex, int nFeaturesToExtract, int* nFeatures)
{
	if(str == NULL || nFeaturesToExtract<=0 || nFeatures == NULL)
	{
		return NULL;
	}
	int i, j, siftDim;
	double* currentFeature;
	SPPoint** features;
	Mat src;
	std::vector<KeyPoint> kp1;
	Mat ds1;
	Ptr<xfeatures2d::SiftDescriptorExtractor> detect =
			xfeatures2d::SIFT::create(nFeaturesToExtract)
	src = imread(str, CV_LOAD_IMAGE_GRAYSCALE);
	if (src.empty())
	{
		printf(UNLOADABLE_IMAGE, str);
		return NULL;
	}
	detect->detect(src, kp1, Mat());
	detect->compute(src, kp1, ds1);

	*nFeatures = ds1.rows;
	siftDim = ds1.cols;

	features = (SPPoint**)malloc(*nFeatures * sizeof(SPPoint*));
	if (features == NULL)
	{
		return NULL;
	}

	currentFeature = (double*)malloc(siftDim*sizeof(double));
	if(currentFeature == NULL)
	{
		return NULL;
	}
	for (i = 0; i < *nFeatures; i++)
	{
		for (j = 0; j < siftDim; j++)
		{
			currentFeature[j] = ds1.at<float>(i,j);
		}
		features[i] = spPointCreate(currentFeature, siftDim, imageIndex);
	}
	free(currentFeature);
	return features;
}
int* spBestSIFTL2SquaredDistance(int kClosest, SPPoint* queryFeature, SPPoint*** databaseFeatures,
		int numberOfImages, int* nFeaturesPerImage)
{
	if(queryFeature == NULL || databaseFeatures == NULL || 
		nFeaturesPerImage == NULL || numberOfImages <= 1)
	{
		return NULL;
	}
	int i, j, finalSize, currentIndex;
	int* bestIndexes;
	double currentDistance;
	BPQueueElement* currElement;
	spPoint* currentPoint;
	SPBPQueue* queue;
	SP_BPQUEUE_MSG msg;

	queue = spBPQueueCreate(kClosest);
	if(queue == NULL) 	// This check allows us to not have to check for "bad argument" return
						//message with Enqueue, Dequeue and Peek.
	{
		return NULL;
	}
	for(i=0; i<numberOfImages; i++)
	{
		for(j=0; j<nFeaturesPerImage; j++)
		{
			currentPoint = queryFeature[i][j];
			currentDistance = spPointL2SquaredDistance(currentDistance, queryFeature);
			currentIndex = spPointGetIndex(currentPoint);
			msg = spBPQueueEnqueue(queue, currentIndex, currentDistance);
		}
	}

	finalSize = spBPQueueSize(queue);
	bestIndexes = (int*)malloc(finalSize*sizeof(int));
	for(i=0; i<finalSize; i++)
	{
		msg = spBPQueuePeek(queue, currElement);
		if(msg == SP_BPQUEUE_EMPTY)	// Prevents us from having to check if msg is "empty" at dequeue.
									// Should never happen aniway, but just in case...
		{
			spBPQueueDestroy(queue);
			return NULL;
		}
		bestIndexes[i] = currElement->index;
		msg = spBPQueueDequeue(queue);
	}
	spBPQueueDestroy(queue);
	return bestIndexes;
}
