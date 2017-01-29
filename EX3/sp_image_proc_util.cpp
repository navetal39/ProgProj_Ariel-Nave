#include <opencv2/imgproc.hpp>//calcHist
#include <opencv2/core.hpp>//Mat
#include <opencv2/highgui.hpp>
#include <opencv2/xfeatures2d.hpp>//SiftDescriptorExtractor
#include <opencv2/features2d.hpp>
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
#define ALLOC_ERROR "An error occurred - allocation failure\n"

SPPoint** spGetRGBHist(const char* str,int imageIndex, int nBins)
{
	/* check if input is valid. */
	if(str==NULL || nBins<=0)
	{
		return NULL;
	}
	Mat src;
	Mat hists[COLOR_NUM];
	int i, j;
	double* data ;
	/* define range of colors. */
	float range[] = {COLOR_RANGE_MIN, COLOR_RANGE_MAX};
	const float* histRange = {range};
	/* define and allocate memory for returning array */
	SPPoint** retArr = (SPPoint**)malloc(COLOR_NUM*sizeof(SPPoint*));
	if(retArr == NULL)
	{
		printf(ALLOC_ERROR);
		return NULL;
	}
	/* obtain and split image data. */
	src = imread(str, CV_LOAD_IMAGE_COLOR);
	if(src.empty())
	{
		printf(UNLOADABLE_IMAGE, str);
		return NULL;
	}
	std::vector<Mat> bgr_planes; // TODO help please
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
		printf(ALLOC_ERROR);
		return NULL;
	}
	for(i=0; i<COLOR_NUM; i++)
	{
		for(j=0; j<hists[0].rows; j++)
		{
			data[j] = hists[i].at<double>(j,0);
		}
		retArr[i] = spPointCreate(data, hists[0].rows, imageIndex);
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
	int i;
	double distance;
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
	int i, j, siftDim;
	double* currentFeature;
	SPPoint** features;
	Mat src;
	std::vector<KeyPoint> kp1;
	Mat ds1;
	/* load image. */
	src = imread(str, CV_LOAD_IMAGE_GRAYSCALE);
	if (src.empty())
	{
		printf(UNLOADABLE_IMAGE, str);
		return NULL;
	}
	/* extract features. */
	Ptr<xfeatures2d::SiftDescriptorExtractor> detect = 
		xfeatures2d::SIFT::create(nFeaturesToExtract);
	detect->detect(src, kp1, Mat());
	detect->compute(src, kp1, ds1);
	/* update information about extracted features. */
	*nFeatures = ds1.rows;
	siftDim = ds1.cols;
	/* allocate memory for the returning buffer. */
	features = (SPPoint**)malloc(*nFeatures * sizeof(SPPoint*));
	if (features == NULL)
	{
		printf(ALLOC_ERROR);
		return NULL;
	}
	/* convert features from matrix form to SPPoint form. */
	currentFeature = (double*)malloc(siftDim*sizeof(double));
	if(currentFeature == NULL)
	{
		printf(ALLOC_ERROR);
		return NULL;
	}
	for (i = 0; i < *nFeatures; i++)
	{
		for (j = 0; j < siftDim; j++)
		{
			currentFeature[j] = ds1.at<double>(i,j);
		}
		features[i] = spPointCreate(currentFeature, siftDim, imageIndex);
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
	int* bestIndexes;
	double currentDistance;
	BPQueueElement* currElement;
	SPPoint* currentPoint;
	SPBPQueue* queue;
	SP_BPQUEUE_MSG msg;

	/* create queue to be used later. */
	queue = spBPQueueCreate(kClosest);
	if(queue == NULL) 	// This check allows us to not have to check for "bad argument" return
						//message with Enqueue, Dequeue and Peek.
	{
		return NULL;
	}
	/* enqueue image indexes according to calculated distance. */
	for(i=0; i<numberOfImages; i++)
	{
		for(j=0; j<nFeaturesPerImage[i]; j++)
		{
			currentPoint = databaseFeatures[i][j];
			currentDistance = spPointL2SquaredDistance(currentPoint, queryFeature);
			currentIndex = spPointGetIndex(currentPoint);
			msg = spBPQueueEnqueue(queue, currentIndex, currentDistance);
		}
	}
	free(currentPoint);

	/* extract the best images from queue into returning array and get rid of it. */
	finalSize = spBPQueueSize(queue);
	bestIndexes = (int*)malloc(finalSize*sizeof(int));
	if(bestIndexes != NULL)
	{
		currElement = (BPQueueElement*) malloc(sizeof(BPQueueElement));
		if(currElement != NULL)
		{
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
			free(currElement);
		}else{
			printf(ALLOC_ERROR);
		}
	}else{
		printf(ALLOC_ERROR);
	}
	spBPQueueDestroy(queue);
	/* return. */
	return bestIndexes;
}
