#include "sp_image_proc_util.h"
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>//SiftDescriptorExtractor
#include <cstdio>

#define THIRD 0.33
#define RANGE_MAX 256

extern "C"{
	#include "SPBPriorityQueue.h"
	#include "SPPoint.h"
}

using namespace cv;
using namespace std;

SPPoint** spGetRGBHist(const char* str,int imageIndex, int nBins)
{
	int i;
	Mat src;
	vector<Mat> bgr_planes;
	Mat b_hist, g_hist, r_hist;
	float range[2];
	double *temp_data;
	SPPoint** points;

	if (NULL == str || nBins <= 0)
		return NULL;
	src = cv::imread(str, CV_LOAD_IMAGE_COLOR);
	if(src.empty())
		return NULL;

	split(src, bgr_planes);
	range[0] = 0;
	range[1] = RANGE_MAX;

	const float* histRange = {range};

	calcHist(&bgr_planes[0], 1, 0, Mat(), b_hist, 1, &nBins, &histRange);
	calcHist(&bgr_planes[1], 1, 0, Mat(), g_hist, 1, &nBins, &histRange);
	calcHist(&bgr_planes[2], 1, 0, Mat(), r_hist, 1, &nBins, &histRange);
	if(NULL == (temp_data = (double*) malloc(sizeof(double)*nBins)))
		return NULL;

	if(NULL == (points = (SPPoint**) malloc(3*sizeof(SPPoint*))))
	{
		free(temp_data);
		return NULL;
	}
	for (i = 0; i < nBins; ++i)
		temp_data[i] = (double)b_hist.at<float>(i);
	if(NULL == (points[0] = spPointCreate(temp_data, nBins, imageIndex)))
	{
		free(temp_data);
		free(points);
		return NULL;
	}
	for (i = 0; i < nBins; ++i)
		temp_data[i] = (double)g_hist.at<float>(i);
	if(NULL == (points[1] = spPointCreate(temp_data, nBins, imageIndex)))
	{
		free(temp_data);
		spPointDestroy(points[0]);
		free(points);
		return NULL;
	}
	for (i = 0; i < nBins; ++i)
		temp_data[i] = (double)r_hist.at<float>(i);
	if(NULL == (points[2] = spPointCreate(temp_data, nBins, imageIndex)))
	{
		free(temp_data);
		spPointDestroy(points[0]);
		spPointDestroy(points[1]);
		free(points);
		return NULL;
	}
	free(temp_data);
	return points;
}

double spRGBHistL2Distance(SPPoint** rgbHistA, SPPoint** rgbHistB)
{
	double r_square_dist = 0, g_square_dist=0, b_square_dist=0;
	if (NULL == rgbHistA || NULL == rgbHistB)
	{
		return -1;
	}

	b_square_dist = spPointL2SquaredDistance(rgbHistA[0], rgbHistB[0]);
	g_square_dist = spPointL2SquaredDistance(rgbHistA[1], rgbHistB[1]);
	r_square_dist = spPointL2SquaredDistance(rgbHistA[2], rgbHistB[2]);

	return THIRD*(b_square_dist+g_square_dist+r_square_dist);
}

SPPoint** spGetSiftDescriptors(const char* str, int imageIndex, int nFeaturesToExtract, int *nFeatures)
{

	Mat src, ds1;
	vector<KeyPoint> kp1;
	int featureSize, i, j;
	Ptr<xfeatures2d::SiftDescriptorExtractor> detect;
	SPPoint** returnFeatures;
	double* currentData;

	src = imread(str, CV_LOAD_IMAGE_GRAYSCALE);
	if (src.empty())
	{
		return NULL;
	}

	detect = xfeatures2d::SIFT::create(nFeaturesToExtract);
	detect->detect(src, kp1, Mat());
	detect->compute(src, kp1, ds1);

	*nFeatures = ds1.rows;
	featureSize = ds1.cols;

	if(NULL == (returnFeatures = (SPPoint**)malloc(sizeof(SPPoint)*ds1.rows)))
	{
		return NULL;
	}
	if(NULL == (currentData = (double*)malloc(sizeof(double)*featureSize)))
	{
		free(returnFeatures);
		return NULL;
	}

	for (i = 0; i < ds1.rows; ++i)
	{
		for (j = 0; j < featureSize; ++j)
		{
			currentData[j] = (double)ds1.at<float>(i,j);
		}
		returnFeatures[i] = spPointCreate(currentData, featureSize, imageIndex);
		if(NULL == returnFeatures[i])
		{
			free(currentData);
			for (j = 0; j < i; ++j)
			{
				spPointDestroy(returnFeatures[j]);
			}
			free(returnFeatures);
			return NULL;
		}
	}

	free(currentData);
	return returnFeatures;
}

int* spBestSIFTL2SquaredDistance(int kClosest, SPPoint* queryFeature, SPPoint*** databaseFeatures, int numberOfImages, int* nFeaturesPerImage)
{
	SPBPQueue* queue_of_kclosest;
	BPQueueElement current_element;
	int *kclosest_indexes, i ,j;
	double current_dist;

	if(NULL == queryFeature || NULL == databaseFeatures || numberOfImages <= 1 || NULL == nFeaturesPerImage)
	{
		return NULL;
	}

	queue_of_kclosest = spBPQueueCreate(kClosest);
	for (i=0; i < numberOfImages; ++i)
	{
		for (j = 0; j < nFeaturesPerImage[i]; ++j)
		{
			current_dist = spPointL2SquaredDistance(queryFeature, databaseFeatures[i][j]);
			if(SP_BPQUEUE_INVALID_ARGUMENT == spBPQueueEnqueue(queue_of_kclosest, i, current_dist))
			{
				spBPQueueDestroy(queue_of_kclosest);
				return NULL;
			}
		}
	}

	if(NULL == (kclosest_indexes = (int*)malloc(sizeof(int)*kClosest)))
	{
		return NULL;
	}

	for (i = 0; i < kClosest; ++i)
	{
		if (SP_BPQUEUE_EMPTY == spBPQueuePeek(queue_of_kclosest, &current_element))
		{
			break; //means that kClosest is bigger than the total number of features.
		}
		spBPQueueDequeue(queue_of_kclosest);
		kclosest_indexes[i] = current_element.index;
	}

	spBPQueueDestroy(queue_of_kclosest);
	return kclosest_indexes;
}
