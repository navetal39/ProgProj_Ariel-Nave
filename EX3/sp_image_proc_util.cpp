#include <opencv2/imgproc.hpp>//calcHist
#include <opencv2/core.hpp>//Mat
#include <opencv2/highgui.hpp>
#include <vector>
#include <iostream>
#include "sp_image_proc_util.h"
extern "C"{
	#include "SPBPriorityQueue.h"
}
using namespace cv;

#define SIFT_FEATURE_DIM 128
#define COLOR_NUM 3
#define COLOR_RANGE_MAX 256
#define COLOR_RANGE_MIN 0

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
	return 0;
}
int* spBestSIFTL2SquaredDistance(int kClosest, SPPoint* queryFeature, SPPoint*** databaseFeatures,
		int numberOfImages, int* nFeaturesPerImage)
{
	return 0;
}
