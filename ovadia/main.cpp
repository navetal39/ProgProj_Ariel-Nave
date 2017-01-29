#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "sp_image_proc_util.h"
#include "main_aux.h"

int main() {
	char imageDirectory[INPUT_MAXLEN], imagePrefix[INPUT_MAXLEN], imageSuffix[INPUT_MAXLEN], imageQuery[INPUT_MAXLEN];
	//,featuresNumberAsString[INPUT_MAXLEN], imagesNumberAsString[INPUT_MAXLEN],binsNumberAsString[INPUT_MAXLEN];
	int imagesNumber, binsNumber, featuresNumber, *nFeaturesPerImage
	,nFeatures, *RGBIndices, *hitList;
	SPPoint ***histArr, ***siftArr, **currHist, **currSift;


	/*memset(imageDirectory, NULL_TERMINATOR, INPUT_MAXLEN);
	memset(imagePrefix, NULL_TERMINATOR, INPUT_MAXLEN);
	memset(imageSuffix, NULL_TERMINATOR, INPUT_MAXLEN);
	printf(PATH_IN);
	scanf("%s", imageDirectory);

	printf(PREF_IN);
	scanf("%s", imagePrefix);

	printf(IMG_IN);
	scanf("%s", imagesNumberAsString);
	if ((imagesNumber = atoi(imagesNumberAsString)) < MIN_RANGE) {
		printf(IMG_ERROR);
		return ERR_RETVAL;
	}

	printf(SUFF_IN);
	scanf("%s", imageSuffix);

	printf(BIN_IN);
	scanf("%s", binsNumberAsString);
	binsNumber = atoi(binsNumberAsString);
	if (binsNumber < MIN_RANGE || binsNumber > MAX_RANGE) {
		printf(BIN_ERROR);
		return ERR_RETVAL;
	}
	

	printf(FEAT_IN);
	scanf("%s", featuresNumberAsString);
	featuresNumber = atoi(featuresNumberAsString);
	if (featuresNumber < MIN_RANGE || featuresNumber > MAX_RANGE) {
		printf(FEAT_ERROR);
		return ERR_RETVAL;
	}
	*/
	handleInput(imageDirectory,imagePrefix,&imagesNumber,imageSuffix,&binsNumber,&featuresNumber);
	if (!(histArr = generateHistArray(imageDirectory, imagesNumber, imagePrefix, imageSuffix, binsNumber))) {
		return ERR_RETVAL;
	}
	if(!(nFeaturesPerImage = (int *)malloc(imagesNumber*sizeof(int)))){
		printf(ALLOC_ERROR);
		destroyAllRGB(histArr,imagesNumber);
		return ERR_RETVAL;
	}

	if (!(siftArr = generateSIFTArray(imageDirectory, imagesNumber, imagePrefix, imageSuffix, featuresNumber, nFeaturesPerImage))) {
		destroyAllRGB(histArr, imagesNumber);
		free(nFeaturesPerImage);
		return ERR_RETVAL;
	}

	printf(QUERY_IN);
	scanf("%s", imageQuery);
	while (strcmp(imageQuery, TERMINATE)) {
		if (!(currHist = spGetRGBHist(imageQuery, 0, binsNumber))) {
			destroyAllRGB(histArr, imagesNumber);
			destroyAllSIFT(siftArr, imagesNumber, nFeaturesPerImage);
			free(nFeaturesPerImage);
			return ERR_RETVAL;
		}
		if (!(currSift = spGetSiftDescriptors(imageQuery, 0, featuresNumber, &nFeatures))) {
			destroyRGBHist(currHist);
			destroyAllRGB(histArr, imagesNumber);
			destroyAllSIFT(siftArr, imagesNumber, nFeaturesPerImage);
			free(nFeaturesPerImage);
			return ERR_RETVAL;
		}
		RGBIndices = spBestRGBHistL2SquaredDistance(currHist, histArr, imagesNumber);
		printf(RGB_OUT);
		printArr(RGBIndices, K_CLOSES);
		hitList = generateHitList(currSift, siftArr, nFeatures, imagesNumber, nFeaturesPerImage);
		printf(SIFT_OUT);
		printArr(hitList, K_CLOSES);
		free(RGBIndices);
		destroyRGBHist(currHist);
		destroySIFT(currSift, nFeatures);
		printf(QUERY_IN);
		scanf("%s", imageQuery);
		free(hitList);
	}
	printf(EXITING);
	destroyAllRGB(histArr, imagesNumber);
	destroyAllSIFT(siftArr, imagesNumber, nFeaturesPerImage);
	free(nFeaturesPerImage);
	return (SUCCESS_RETVAL);

}
