#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "main_aux.h"

int main()
{
	int retVal, imgNum, binNum, featureNum, actualFeatureNum,
		*featuresPerImage;
	char dirPath[INPUT_LEN+1], imgPrefix[INPUT_LEN+1], imgSuffix[INPUT_LEN+1],
		 queryPath[INPUT_LEN+1];
	SPPoint ***imgsHists, **queryHist,
			***imgsFeatures, **queryFeatures;
	retVal = getInputFromUser(dirPath, imgPrefix, &imgNum, imgSuffix,
							  &binNum, &featureNum); ASSERT_GOOD(retVal);
	retVal = getImgDataMult(dirPath, imgPrefix, imgNum, imgSuffix,
							binNum, featureNum, &featuresPerImage,
							&imgsHists, &imgsFeatures); ASSERT_GOOD(retVal);
	do{
		getQueryPath(queryPath);
		if(SHOULD_RUN(queryPath))
		{
			retVal = getImgData(queryPath, QUERY_INDEX, binNum, featureNum, &actualFeatureNum,
								&queryHist, &queryFeatures);
			if(retVal == RET_SUCCESS)
			{
				retVal = getBestResults(imgsHists, imgsFeatures, queryHist, queryFeatures,
									   imgNum, featuresPerImage, actualFeatureNum);
				destroyPointsArray(queryHist, COLOR_NUM);
				destroyPointsArray(queryFeatures, actualFeatureNum);
				if(retVal != RET_SUCCESS)
				{
					break;
				}
			}else{
				break;
			}
		}else{
			printf(MSG_EXIT);
		}
	}while(SHOULD_RUN(queryPath));
	destroyPointsArrayArray(imgsHists, imgNum, 0, NULL);
	destroyPointsArrayArray(imgsFeatures, imgNum, 1, featuresPerImage);
	free(featuresPerImage);
	return(RET_SUCCESS);
}
