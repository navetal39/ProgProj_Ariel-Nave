#include "sp_image_proc_util.h"
#include <stdlib.h>
#include <stddef.h>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/features2d.hpp>
#include <vector>

using namespace cv;

#define FEATURE_SIZE 128
#define NUMBER_OF_COLORS 3
#define ANNOUNCE_ALLOC_FAIL "An error occurred - allocation failure\n"

/*
   allocate 2 dimensional array for RGBHist
 */
int** allocRGBHist(int nBins)
{
	int** RGBHist = (int**) malloc(NUMBER_OF_COLORS * sizeof(int*)); // Allocate 2D array
	if (RGBHist == NULL) // If something went wrong return null
		return NULL;

	for (int i = 0; i < NUMBER_OF_COLORS; i++)
	{
		RGBHist[i] = (int*) malloc(nBins * sizeof(int)); // Allocate specific channels
		if (RGBHist[i] == NULL) // If something went wrong, free allocated memory and return null
		{
			for (int j = 0; j < i; j++)
				free(RGBHist[j]);

			free(RGBHist);
			return NULL;
		}
	}
	return RGBHist; // If everything went well return result
}

/*
   Allocate 2 dimensional array for featuresArray
 */
double** allocFeaturesArray(int nFeatures)
{
	double** featuresArray = (double**) malloc(nFeatures * sizeof(double*)); // Allocate 2D array
	if (featuresArray == NULL) // If something went wrong return null
		return NULL;

	for (int i = 0; i < nFeatures; i++)
	{
		featuresArray[i] = (double*) malloc(FEATURE_SIZE * sizeof(double)); // Allocate specific features
		if (featuresArray[i] == NULL) // If something went wrong, free allocated memory and return null
		{
			for (int j = 0; j < i; j++)
				free(featuresArray[j]);

			free(featuresArray);
			return NULL;
		}
	}

	return featuresArray; // If everything went well return result
}

int** spGetRGBHist(char* str, int nBins)
{
	// Declare variables
	Mat src;
	Mat rgbChannels[NUMBER_OF_COLORS];
	float range[] = { 0, 256 };
	const float* histRange = { range };
	int nImages = 1;
	Mat rHist, gHist, bHist;

	// If input is illegal, terminate
	if (nBins <= 0 || str == NULL)
		return NULL;

	int** RGBHist = allocRGBHist(nBins);
	if (RGBHist == NULL) // If something went wrong with memory allocation, terminate
		return NULL;

	src = imread(str, CV_LOAD_IMAGE_COLOR);
	if (src.empty()) // If something went wrong with loading the picture, terminate
		return NULL;

	// Use openCV to proccess image
	split(src, rgbChannels);
	calcHist(&rgbChannels[0], nImages, 0, Mat(), rHist, 1, &nBins, &histRange);
	calcHist(&rgbChannels[1], nImages, 0, Mat(), gHist, 1, &nBins, &histRange);
	calcHist(&rgbChannels[2], nImages, 0, Mat(), bHist, 1, &nBins, &histRange);

	// Copy data into 2D array
	for(int i = 0; i < rHist.rows; i++)
	{
		RGBHist[0][i] = cvRound(rHist.at<float>(i,0));
		RGBHist[1][i] = cvRound(gHist.at<float>(i,0));
		RGBHist[2][i] = cvRound(bHist.at<float>(i,0));
    }

	return RGBHist;
}

double spRGBHistL2Distance(int** histA, int** histB, int nBins)
{
    // Declare and initialize values
	double redL2Distance = 0, greenL2Distance = 0, blueL2Distance = 0;
	double histL2Distance = 0;

    if (nBins <= 0 || histA == NULL || histB == NULL) // if input is illegal, terminate
		return -1;

    // Calculate distance
    for (int i = 0; i < nBins; i++)
	{
		redL2Distance += ((double)histA[0][i] - (double)histB[0][i]) * ((double)histA[0][i] - (double)histB[0][i]);
		greenL2Distance += ((double)histA[1][i] - (double)histB[1][i]) * ((double)histA[1][i] - (double)histB[1][i]);
		blueL2Distance += ((double)histA[2][i] - (double)histB[2][i]) * ((double)histA[2][i] - (double)histB[2][i]);
	}

	histL2Distance = 0.33 * redL2Distance + 0.33 * greenL2Distance + 0.33 * blueL2Distance;

	return histL2Distance;
}

double** spGetSiftDescriptors(char* str, int maxNFeautres, int *nFeatures)
{
    // Declare variables
	Mat src;
	double** featuresArray;
	std::vector<KeyPoint> kp1;
	Mat ds1;
	Ptr<xfeatures2d::SiftDescriptorExtractor> detect =
			xfeatures2d::SIFT::create(maxNFeautres);

    if (str == NULL || maxNFeautres <= 0 || nFeatures == NULL) // If input is illegal, terminate
		return NULL;

	src = imread(str, CV_LOAD_IMAGE_GRAYSCALE);
    if (src.empty()) // If image loading failed, terminate
		return NULL;

    // Calculate features
	detect->detect(src, kp1, Mat());
	detect->compute(src, kp1, ds1);

    *nFeatures =ds1.rows; // Update value as expected
    featuresArray = allocFeaturesArray(*nFeatures);
    if (featuresArray == NULL) // If something went wrong, free allocated memory and return null
		return NULL;

    // Move data into return array
	for (int i = 0; i < *nFeatures; i++)
		for (int j = 0; j < ds1.cols; j++)
			featuresArray[i][j] = ds1.at<float>(i,j);

	return featuresArray;
}

double spL2SquaredDistance(double* featureA, double* featureB)
{
    // Declare and initialize variables
	double L2SquaredDistance = 0;

    if (featureA == NULL || featureB == NULL) // If input is illegal, terminate
		return -1;
    // Calculate distance
    for (int i = 0; i < FEATURE_SIZE; i++)
		L2SquaredDistance += (featureA[i] - featureB[i]) * (featureA[i] - featureB[i]);

	return L2SquaredDistance;
}

/*
   Is used for sorting FEATURE_SIZE
*/
struct Feature
{
		int image;
		double dist;
};

/*
 * Compares two Feature objects, first by distnace, then by index
*/
int featureComp(const void* a, const void* b) {
	Feature *fa = (Feature*) a;
	Feature *fb = (Feature*) b;

	if (fa->dist == fb->dist)
		return fa->image - fb->image;

	if(fa->dist < -0.1)
		return 1;
	if(fb->dist < -0.1)
		return -1;

	if (fa->dist > fb->dist)
		return 1;
	else
		return -1;
}

int* spBestSIFTL2SquaredDistance(int bestNFeatures, double* featureA,
		double*** databaseFeatures, int numberOfImages,
		int* nFeaturesPerImage)
{
    // Declare variables
	int* returnArray;
	Feature* featureArray;
	Feature worst;
	Feature temp;

	if (featureA == NULL || databaseFeatures == NULL
            || numberOfImages <= 0 || nFeaturesPerImage == NULL) { // If input was illegal, terminate
		return NULL;
	}

    // Initialize arrays to be used
	returnArray = (int*) malloc(bestNFeatures * sizeof(int));
	featureArray = (Feature*) malloc(bestNFeatures * sizeof(Feature));
    if (returnArray == NULL || featureArray == NULL) // If allocation went wrong, terminate
	{
		printf(ANNOUNCE_ALLOC_FAIL);
		free(returnArray);
		free(featureArray);
		return NULL;
	}

    // Initialize array values
	for (int i = 0; i < bestNFeatures; i++)
	{
		featureArray[i].image = 0;
		featureArray[i].dist = -1;
	}

    // Keep track of worst item in array, to know if new feature is better or not
	worst.image = -1;
	worst.dist = -1;

	for (int i = 0; i < numberOfImages; i++)
	{
		for (int j = 0; j < nFeaturesPerImage[i]; j++)
		{
            // Load current feature
			temp.image = i;
			temp.dist = spL2SquaredDistance(featureA, databaseFeatures[i][j]);
            if (featureComp(&temp, &worst) < 0) // If current feature is better than current worst
			{
                featureArray[bestNFeatures - 1] = temp; // Replace worst by current feature
                qsort(featureArray, bestNFeatures, sizeof(Feature), featureComp); // Sort the array
                worst = featureArray[bestNFeatures - 1]; // Reassign worst
			}
		}
	}

    // Put appropriate values in return array
	for (int i = 0; i < bestNFeatures; i++)
		returnArray[i] = featureArray[i].image;

    // Free unnecessary memory
	free(featureArray);

	return returnArray;
}
