#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "main_aux.h"

int main()
{
	int retVal, imgNum, binNum, featureNum, actualFeatureNum, bestRGBCount, bestSIFTCount,
		*featuresPerImage, *bestRGBImages, *bestSIFTImages;
	char dirPath[INPUT_LEN+1], imgPrefix[INPUT_LEN+1], imgSuffix[INPUT_LEN+1],
		 queryPath[INPUT_LEN+1];
	SPPoint ***imgsHists, **queryHist,
			***imgsFeatures, **queryFeatures;
	retVal = getInputFromUser(dirPath, imgPrefix, &imgNum, imgSuffix,
							  &binNum, &featureNum);
	retVal = getImgDataMult(dirPath, imgPrefix, imgNum, imgSuffix,
							binNum, featureNum, &featuresPerImage,
							&imgsHists, &imgsFeatures);
	do{
		retVal = getQueryPath(queryPath);
		if(SHOULD_RUN(queryPath))
		{
			retVal = getImgData(queryPath, QUERY_INDEX, binNum, featureNum, &actualFeatureNum,
								&queryHist, &queryFeatures);
			retVal = getBestResults(imgsHists, imgsFeatures, queryHist, queryFeatures,
								   imgNum, &featuresPerImage,
								   &bestRGBImages, &bestSIFTImages, &bestRGBCount, &bestSIFTCount);
			retVal = destroyPointsArray(queryHist, COLOR_NUM);
			retVal = destroyPointsArray(queryFeatures, actualFeatureNum);
			free(bestRGBImages);
			free(bestSIFTImages);
		}
	}while(SHOULD_RUN(queryPath));
	retVal = destroyPointsArrayArray(imgsHists, imgNum, 0, NULL);
	retVal = destroyPointsArrayArray(imgsFeatures, imgNum, 1, featuresPerImage);
	free(featuresPerImage);
	if(retVal)
	{
		printf("hi\n");
	}
	return(RET_SUCCESS);
}
