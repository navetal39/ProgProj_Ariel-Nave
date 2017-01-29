extern "C" {
#include "SPPoint.h"
#include "SPBPriorityQueue.h"
}

#include "sp_image_proc_util.h"
#include <opencv2/imgproc.hpp>//calcHist
#include <opencv2/core.hpp>//Mat
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/opencv.hpp>
#include <cstdio>
#include <cstdlib>

#define ALLOC_ERROR "An error occurred - allocation failure\n"

using namespace cv;

SPPoint** spGetRGBHist(const char* str, int imageIndex, int nBins) {
	//declaring variables
	Mat b_hist, g_hist, r_hist, src = imread(str, CV_LOAD_IMAGE_COLOR);
	SPPoint **arr;
	if(!(arr = (SPPoint**)malloc(3*sizeof(SPPoint*)))){
		printf(ALLOC_ERROR);
		return NULL;
	}
	double *data_arr;
	int forLoops;
	//allocating place for our data array
	if (!(data_arr = (double*)malloc(nBins * sizeof(double)))) {
		printf(ALLOC_ERROR);
		return NULL;
	}
	float range[] = {0, 256};
	const float* histRange = {range};
	if (src.empty()) {
		return NULL;
	}
	//calculating histograms
	Mat bgr_planes[3];
	split(src, bgr_planes);
	calcHist(&bgr_planes[0], 1, 0, Mat(), b_hist, 1, &nBins, &histRange);
	calcHist(&bgr_planes[1], 1, 0, Mat(), g_hist, 1, &nBins, &histRange);
	calcHist(&bgr_planes[2], 1, 0, Mat(), r_hist, 1, &nBins, &histRange);
	//inserting the histogram values to the data array
	for (forLoops = 0; forLoops < nBins; ++forLoops)
		data_arr[forLoops] = (double)r_hist.at<float>(forLoops);
	//creating point for the array to be returned
	if (!(arr[0] = spPointCreate(data_arr, nBins, imageIndex))) {
		free(data_arr);
		free(arr);
		return NULL;
	}
	//repeating for other colours
	for (forLoops = 0; forLoops < nBins; ++forLoops)
		data_arr[forLoops] = (double)g_hist.at<float>(forLoops);

	if (!(arr[1] = spPointCreate(data_arr, nBins, imageIndex))) {
		free(data_arr);
		spPointDestroy(arr[0]);
		free(arr);
		return NULL;
	}
	for (forLoops = 0; forLoops < nBins; ++forLoops)
		data_arr[forLoops] = (double)b_hist.at<float>(forLoops);

	if (!(arr[2] = spPointCreate(data_arr, nBins, imageIndex))) {
		free(data_arr);
		spPointDestroy(arr[0]);
		spPointDestroy(arr[1]);
		free(arr);
		return NULL;
	}
	//freeing and returning :-)
	free(data_arr);
	return arr;
}

double spRGBHistL2Distance(SPPoint** rgbHistA, SPPoint** rgbHistB) {
	//declaring variables
	double d1, d2, d3;
	d1 = spPointL2SquaredDistance(rgbHistA[0], rgbHistB[0]);
	d2 = spPointL2SquaredDistance(rgbHistA[1], rgbHistB[1]);
	d3 = spPointL2SquaredDistance(rgbHistA[2], rgbHistB[2]);
	//returning mean value
	return (0.33 * d1 + 0.33 * d2 + 0.33 * d3);
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
			printf("POINTFAIL\n");
			free(retFeatures);
			return NULL;
		}

	}
	//freeing
	free(data_arr);
	//returning
	return retFeatures;
}
int* spBestSIFTL2SquaredDistance(int kCloses, SPPoint* queryFeature,
                                 SPPoint*** databaseFeatures, int numberOfImages, int* nFeaturesPerImage) {
	//declaring variables
	int i, j, *toRet;
	BPQueueElement res;
	//creating the queue to store the closest features
	SPBPQueue *ourQueue = spBPQueueCreate(kCloses);
	//making sure arguments are valid
	if (!queryFeature || !databaseFeatures || !nFeaturesPerImage) {
		return NULL;
	}
	//allocating memory for the returned *int
	if (!(toRet = (int *)(malloc(sizeof(int) * kCloses)))) {
		printf(ALLOC_ERROR);
		return NULL;
	}
	//iterates over all the points and enqueues them
	for (i = 0; i < numberOfImages; ++i) {
		for (j = 0; j < nFeaturesPerImage[i]; ++j) {
			spBPQueueEnqueue(ourQueue, i, spPointL2SquaredDistance(databaseFeatures[i][j], queryFeature));
		}
	}
	//extracting the kCloses closest elements of the queue
	//and inserting their index(the image they belong to) to the array toRet
	for (i = 0; i < kCloses; ++i) {
		if (spBPQueuePeek(ourQueue, &res) == SP_BPQUEUE_INVALID_ARGUMENT) {
			spBPQueueDestroy(ourQueue);
			free (toRet);
			return NULL;
		}
		spBPQueueDequeue(ourQueue);
		toRet[i] = res.index;
	}
	//destroying
	spBPQueueDestroy(ourQueue);
	//returning :-)
	return toRet;
}
