#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "main_aux.h"

int main()
{
	int retVal, imgNum, binNum, featureNum, bestRGBCount, bestSIFTCount,
		*featuresPerImage, *bestRGBImages, *bestSIFTImages;
	char dirPath[INPUT_LEN+1], imgPrefix[INPUT_LEN+1], imgSuffix[INPUT_LEN+1],
		 queryPath[INPUT_LEN+1];
	SPPoint ***imgsHists, **queryHist,
			***imgsFeatures, **queryFeatures;
	retVal = getInputFromUser(dirPath, imgPrefix, &imgNum, imgSuffix,
							  &binNum, &featureNum);
	retVal = getImgDataMult(dirPath, imgPrefix, &imgNum, imgSuffix,
							binNum, featureNum, featuresPerImage,
							imgsHists, imgsFeatures);
	do{
		retVal = getQueryPath(queryPath);
		if(SHOULD_RUN(queryPath))
		{
			retVal = getImgData(queryPath, binNum, featureNum,
								queryHist, queryFeatures);
			retVal = getBestImages(imgsHists, imgsFeatures, queryHist, queryFeatures,
								   imgNum, featuresPerImage,
								   bestRGBImages, bestSIFTImages, &bestRGBCount, &bestSIFTCount);
			retVal = printResults(bestRGBImages, bestSIFTImages, bestRGBCount, bestSIFTCount);
			retVal = cleanUp(queryHist, queryFeatures);
			free(bestRGBImages);
			free(bestSIFTImages);
		}
	}while(SHOULD_RUN(queryPath));
	retVal = cleanUpMult(imgsHists, imgsFeatures);
	free(featuresPerImage);
	return(RET_SUCCESS);
}
