#ifndef SPCONFIG_H_
#define SPCONFIG_H_

#include <stdbool.h>
#include <stdio.h>
#include "aux.h"
#include "SPLogger.h"
#include "SPConfig.h"
#include "SPKDTree.h"

typedef struct sp_config_t{
	char* imgDir;
	char* imgPre;
	char* imgSuf;
	int imgNum;
	int pcaDim;
	char* pcaFile;
	int featureNum;
	bool extractMode;
	int knnNumImg;
	SP_KDT_SPLIT splitMethod;
	int knnNumFeatures;
	bool useMinGUI;
	int logLvl;
	char* logFile
};


SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg)
{
	spConfig config = (spConfig)malloc(sizeof(sp_config_t));
	if(config == NULL)
	{
		*msg = SP_CONFIG_ALLOC_FAIL;
		return NULL;
	}
	FILE* f;
	SP_CONFIG_MSG retMsg;
	bool wersSet[VARS_COUNT] = {false};
	f = fopen(filename, "r");
	if(f == NULL)
	{
		free(config);
		*msg = SP_CONFIG_CANNOT_OPEN_FILE;
		return NULL;
	}
	spConfigInitConfig(config, wersSet)
	do{
		retMsg = spConfigGetLine(config, f);
	}while(retMsg != SP_CONFIG_SUCCESS);
	return NULL; // TODO finish
}

void spConfigInitConfig(spConfig config, bool wersSet[])
{
	cfgSet(pcaDim, DEFAULT_PCA_DIM);
	cfgSet(pcaFile, DEFAULT_PCA_FILE);
	cfgSet(featureNum, DEFAULT_FEATURES_NUM);
	cfgSet(extractMode, DEFAULT_EXT_MODE);
	cfgSet(useMinGUI, DEFAULT_MIN_GUI);
	cfgSet(knnNumImg, DEFAULT_SIM_IMG_NUM);
	cfgSet(knnNumFeatures, DEFAULT_KNN_FEATS);
	cfgSet(splitMethod, DEFAULT_SPLIT_METHOD);
	cfgSet(logLvl, DEFAULT_LOG_LEVEL);
	cfgSet(logFile, DEFAULT_LOG_FILE);
}
SP_CONFIG_MSG spConfigGetLine(spConfigCreate config, File* f)
{
	return NULL;
}

char* spConfigGetImgDir(const spConfigCreate config, SP_CONFIG_MSG* msg)
{
	cfgGetHead(imgDir);
}

char* spConfigGetImgPrefix(const spConfigCreate config, SP_CONFIG_MSG* msg)
{
	cfgGetHead(imgPre);
}

char* spConfigGetImgSuffix(const spConfigCreate config, SP_CONFIG_MSG* msg)
{
	cfgGetHead(imgSuf);
}
int spConfigGetNumOfImages(const SPConfig config, SP_CONFIG_MSG* msg)
{
	cfgGetHead(imgNum);
}
int spConfigGetPCADim(const SPConfig config, SP_CONFIG_MSG* msg)
{
	cfgGetHead(pcaDim);
}
char* spConfigGetPCAFile(const spConfig config, SP_CONFIG_MSG* msg)
{
	cfgGetHead(pcaFile);
}
int spConfigGetNumOfFeatures(const SPConfig config, SP_CONFIG_MSG* msg)
{
	cfgGetHead(featureNum);
}
bool spConfigIsExtractionMode(const SPConfig config, SP_CONFIG_MSG* msg)
{
	cfgGetHead(extractMode);
}
bool spConfigGetNumOfSimilarImages(const SPConfig config, SP_CONFIG_MSG* msg)
{
	cfgGetHead(knnNumImg);
}
SP_KDT_SPLIT spConfgGetSplitMethod(const spConfig config, SP_CONFIG_MSG* msg)
{
	cfgGetHead(splitMethod);
}
bool spConfigGetNumOfSimilarFeatures(const SPConfig config, SP_CONFIG_MSG* msg)
{
	cfgGetHead(knnNumFeatures);
}
bool spConfigMinimalGui(const SPConfig config, SP_CONFIG_MSG* msg)
{
	cfgGetHead(useMinGUI);
}
int spConfigGetLogLevel(const SPConfig config, SP_CONFIG_MSG* msg)
{
	cfgGetHead(logLvl);
}
char* spConfigGetLogLevel(const SPConfig config, SP_CONFIG_MSG* msg)
{
	cfgGetHead(logFile);
}

SP_CONFIG_MSG spConfigGetImagePath(char* imagePath, const SPConfig config,
		int index)
{
	if(imagePath == NULL || config == NULL)
	{
		return SP_CONFIG_INVALID_ARGUMENT;
	}
	if(index < config.imgNum)
	{
		return SP_CONFIG_INDEX_OUT_OF_RANGE;
	}
	int dirLen = strlen(config->imgDir);
	int preLen = strlen(config->imgPre);
	int iLen = getDigitscount(index);
	int sufLen = strlen(config->imgSuf);
	totalLen = dirLen + preLen + iLen + sufLen + 1;
	// imagePath = (char*)malloc(totalLen);
	memset(imagePath, '\0', totalLen);

	strcpy(imagePath, config->imgDir);
	strcpy(imagePath + (dirLen), config->imgPre);
	sprintf(imagePath + (dirLen + preLen), "%d", index);
	strcpy(imagePath + (dirLen + preLen + iLen), config->imgSuf);

	return SP_CONFIG_SUCCESS;
}

SP_CONFIG_MSG spConfigGetPCAPath(char* pcaPath, const SPConfig config)
{
	if(imagePath == NULL || config == NULL)
	{
		return SP_CONFIG_INVALID_ARGUMENT;
	}
	int dirLen = strlen(config->imgDir);
	int pcaLen = strlen(config->pcaFile);
	totalLen = dirLen + pcaLen + 1;
	// imagePath = (char*)malloc(totalLen*sizeof(char));
	memset(imagePath, '\0', totalLen);

	strcpy(imagePath, config->imgDir);
	strcpy(imagePath + (dirLen), config->pcaFile);

	return SP_CONFIG_SUCCESS;
}


void spConfigDestroy(SPConfig config)
{
	free(config);
}

#endif /* SPCONFIG_H_ */
