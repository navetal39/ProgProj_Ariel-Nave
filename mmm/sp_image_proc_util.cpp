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
SPPoint** spGetSiftDescriptors(const char* str, int imageIndex, int nFeaturesToExtract, int *nFeatures) {
	//loading image in grayscale format for feature extraction
	Mat src = imread(str, CV_LOAD_IMAGE_GRAYSCALE), desc;

	//declaring variables
	int featDim, i, j;
	std::vector<KeyPoint> kp1;
	SPPoint **retFeatures;
	double *data_arr;
	Ptr<xfeatures2d::SiftDescriptorExtractor> desExt;
	//making sure source is not empty
	if (src.empty())
		return NULL;
	//creating a sift descriptor extractor and extracting features
	desExt = xfeatures2d::SIFT::create(nFeaturesToExtract);
	desExt->detect(src, kp1, Mat());
	desExt->compute(src, kp1, desc);
	//savinhg actual number of features and dimension we got
	*nFeatures = desc.rows;
	featDim = desc.cols;
	//allocating memory for the point array (to be returned) and data array
	if (!(retFeatures = (SPPoint**)malloc(desc.rows * sizeof(SPPoint*)))) {
		printf(ALLOC_ERROR);
		return NULL;
	}

	if (!(data_arr = (double*)malloc(featDim * sizeof(double)))) {
		free(retFeatures);
		printf(ALLOC_ERROR);
		return NULL;
	}
	//generating data array for each point creation
	for (i = 0; i < desc.rows; ++i) {
		for (j = 0; j < featDim; ++j)
			data_arr[j] = (double)desc.at<float>(i, j);
		if (!(retFeatures[i] = spPointCreate(data_arr, featDim, imageIndex))) {
			free(data_arr);
			for (j = 0; j < i; ++j) {
				spPointDestroy(retFeatures[j]);
			}
			free(retFeatures);
			return NULL;
		}

	}
	//freeing
	free(data_arr);
	//returning
	return retFeatures;
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
	// printf("aaa\n");
	queue = spBPQueueCreate(kClosest);
	// printf("bbb\n");
	if(queue == NULL) 	// This check allows us to not have to check for "bad argument" return
						//message with Enqueue, Dequeue and Peek.
	{
		// printf("errorerrorerror\n");
		return NULL;
	}
	/* enqueue image indexes according to calculated distance. */
	for(i=0; i<numberOfImages; i++)
	{
		// printf("ccc\n");
		for(j=0; j<nFeaturesPerImage[i]; j++)
		{
			// printf("ddd\n");
			currentPoint = databaseFeatures[i][j];
			// printf("eee\n");
			// printf("%s %s\n", (currentPoint==NULL)?"YES":"NO", (queryFeature==NULL)?"YES":"NO");
			currentDistance = spPointL2SquaredDistance(currentPoint, queryFeature);
			// printf("fff\n");
			currentIndex = spPointGetIndex(currentPoint);
			// printf("ggg\n");
			msg = spBPQueueEnqueue(queue, currentIndex, currentDistance);
			// printf("hhh\n");
		}
		// printf("iii\n");
	}
	// printf("jjj\n");
	free(currentPoint);
	// printf("kkk\n");
	/* extract the best images from queue into returning array and get rid of it. */
	finalSize = spBPQueueSize(queue);
	// printf("lll\n");
	bestIndexes = (int*)malloc(finalSize*sizeof(int));
	// printf("mmm\n");
	if(bestIndexes != NULL)
	{
		// printf("nnn\n");
		currElement = (BPQueueElement*) malloc(sizeof(BPQueueElement));
		// printf("ooo\n");
		if(currElement != NULL)
		{
			// printf("ppp\n");
			for(i=0; i<finalSize; i++)
			{
				// printf("qqq\n");
				msg = spBPQueuePeek(queue, currElement);
				// printf("rrr\n");
				if(msg == SP_BPQUEUE_EMPTY)	// Prevents us from having to check if msg is "empty" at dequeue.
											// Should never happen aniway, but just in case...
				{
					// printf("moreerrorwtf\n");
					spBPQueueDestroy(queue);
					return NULL;
				}
				// printf("sss\n");
				bestIndexes[i] = currElement->index;
				// printf("ttt\n");
				msg = spBPQueueDequeue(queue);
				// printf("uuu\n");
			}
			// printf("vvv\n");
			free(currElement);
			// printf("www\n");
		}else{
			// printf("asdfjklhasdfklasdkljf\n");
			printf(ALLOC_ERROR);
		}
	}else{
		// printf("asdfehjuuikiljhklkjlhjkvvvvv\n");
		printf(ALLOC_ERROR);
	}
	// printf("xxx\n");
	spBPQueueDestroy(queue);
	// printf("yyy\n");
	/* return. */
	return bestIndexes;
}
