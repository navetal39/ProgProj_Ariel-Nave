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

SPPoint** spGetRGBHist(const char* str,int imageIndex, int nBins)
{
	Mat src;
	Mat b_hist, g_hist, r_hist;
	float range[] = {0, 256};
	const float* histRange = {range};
	SPPoint* retArr[3];
	float* data = (float*)malloc(nBins*sizeof(float));

	src = imread(str, CV_LOAD_IMAGE_COLOR);
	if(src.empty())
	{
		return -1; // TODO make sure it's good
	}
	std::vector<Mat> bgr_planes; // TODO help please
	split(src, bgr_planes);
	calcHist(&bgr_planes[0], 1, 0, Mat(), b_hist, 1, &nBins, &histRange);
	calcHist(&bgr_planes[1], 1, 0, Mat(), g_hist, 1, &nBins, &histRange);
	calcHist(&bgr_planes[2], 1, 0, Mat(), r_hist, 1, &nBins, &histRange);

	free(data);
	return retArr;
}
double spRGBHistL2Distance(SPPoint** rgbHistA, SPPoint** rgbHistB)
{
	return 0.0;
}
SPPoint** spGetSiftDescriptors(const char* str, int imageIndex, int nFeaturesToExtract, int *nFeatures)
{
	return 0;
}
int* spBestSIFTL2SquaredDistance(int kClosest, SPPoint* queryFeature, SPPoint*** databaseFeatures,
		int numberOfImages, int* nFeaturesPerImage)
{
	return 0;
}
