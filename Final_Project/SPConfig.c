#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SPLogger.h"
#include "SPConfig.h"


struct sp_config_t{
	char* imgDir;
	char* imgPre;
	char* imgSuf;
	int imgNum;
	int pcaDim;
	char* pcaFile;
	int featureNum;
	int extractMode;
	int knnNumImg;
	SP_KDT_SPLIT splitMethod;
	int knnNumFeatures;
	int useMinGUI;
	int logLvl;
	char* logFile;
};

SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg)
{
	FILE* configFile;
	SPConfig config;
	int lineNum = 0, unset;
	char *var, *val, *line;
	SP_CONFIG_UTIL_MSG utilMsg;
	line = (char*)malloc((MAX_LINE_SIZE+2)*sizeof(char));
	if(line == NULL)
	{
		/* TODO print error */
		*msg = SP_CONFIG_ALLOC_FAIL;
		return NULL;
	}
	configFile = fopen(filename, READ_MODE);
	if(configFile == NULL)
	{
		/* TODO print error */
		*msg = SP_CONFIG_CANNOT_OPEN_FILE;
		free(line);
		return NULL;
	}
	config = (SPConfig)malloc(sizeof(struct sp_config_t));
	if(config == NULL)
	{
		/* TODO print error */
		*msg = SP_CONFIG_ALLOC_FAIL;
		fclose(configFile);
		free(line);
		return NULL;
	}
	memset(config, 0, sizeof(struct sp_config_t));
	config->splitMethod = SP_KD_SPLIT_UNKNOWN;
	spConfigInitDefaults(config);
	while(fgets(line, MAX_LINE_SIZE+2, configFile))
	{
		if((utilMsg = spConfigUtilParseLine(line, &var, &val)) ==
			SP_CONFIG_UTIL_BAD_LINE)
		{
			printf(ERR_MSG_INV_LINE, filename, lineNum);
			*msg = SP_CONFIG_BAD_LINE;
			fclose(configFile);
			spConfigDestroy(config);
			free(line);
			return NULL;
		}
		if(utilMsg != SP_CONFIG_UTIL_EMPTY_LINE &&
			!spConfigSetValue(config, var, val))
		{
			printf(ERR_MSG_CONSTRNT, filename, lineNum);
			*msg =(spConfigUtilIsInt(spConfigUtilGetVarNum(var)))
				?SP_CONFIG_INVALID_INTEGER
				:SP_CONFIG_INVALID_STRING;
		}
		lineNum++;
	}
	unset = spConfigGetUnset(config);
	if(unset >= 0)
	{
		var = spConfigUtilGetVarName(unset);
		printf(ERR_MSG_NO_VALUE, filename, lineNum, var);
		spConfigDestroy(config);
		config = NULL;
	}
	fclose(configFile);
	free(line);
	return config;
}

char* spConfigGetImgDir(const SPConfig config, SP_CONFIG_MSG* msg)
{
	GETTER_BODY(config->imgDir, NULL);
}

char* spConfigGetImgPrefix(const SPConfig config, SP_CONFIG_MSG* msg)
{
	GETTER_BODY(config->imgPre, NULL);
}

char* spConfigGetImgSuffix(const SPConfig config, SP_CONFIG_MSG* msg)
{
	GETTER_BODY(config->imgSuf, NULL);
}

int spConfigGetNumOfImages(const SPConfig config, SP_CONFIG_MSG* msg)
{
	GETTER_BODY(config->imgNum, -1);
}

int spConfigGetPCADim(const SPConfig config, SP_CONFIG_MSG* msg)
{
	GETTER_BODY(config->pcaDim, -1);
}

char* spConfigGetPCAFile(const SPConfig config, SP_CONFIG_MSG* msg)
{
	GETTER_BODY(config->pcaFile, NULL);
}

int spConfigGetNumOfFeatures(const SPConfig config, SP_CONFIG_MSG* msg)
{
	GETTER_BODY(config->featureNum, -1);
}

bool spConfigIsExtractionMode(const SPConfig config, SP_CONFIG_MSG* msg)
{
	GETTER_BODY(config->extractMode == 1, false);
}

int spConfigGetNumOfSimilarImages(const SPConfig config, SP_CONFIG_MSG* msg)
{
	GETTER_BODY(config->knnNumImg, -1);
}

SP_KDT_SPLIT spConfigGetSplitMethod(const SPConfig config, SP_CONFIG_MSG* msg)
{
	GETTER_BODY(config->splitMethod, SP_KD_SPLIT_UNKNOWN);
}

int spConfigGetNumOfSimilarFeatures(const SPConfig config, SP_CONFIG_MSG* msg)
{
	GETTER_BODY(config->knnNumFeatures, false);
}

bool spConfigMinimalGui(const SPConfig config, SP_CONFIG_MSG* msg)
{
	GETTER_BODY(config->useMinGUI == 1, false);
}

int spConfigGetLogLevel(const SPConfig config, SP_CONFIG_MSG* msg)
{
	GETTER_BODY(config->logLvl, -1);
}

char* spConfigGetLogFile(const SPConfig config, SP_CONFIG_MSG* msg)
{
	GETTER_BODY(config->logFile, NULL);
}

SP_CONFIG_MSG spConfigGetImagePath(char* imagePath, const SPConfig config,
		int index)
{
	int dirLen;
	int preLen;
	int iLen;
	int sufLen;
	int totalLen;
	if(imagePath == NULL || config == NULL)
	{
		return SP_CONFIG_INVALID_ARGUMENT;
	}
	if(index < config->imgNum)
	{
		return SP_CONFIG_INDEX_OUT_OF_RANGE;
	}
	dirLen = strlen(config->imgDir);
	preLen = strlen(config->imgPre);
	iLen = spConfigUtilCountDigits(index);
	sufLen = strlen(config->imgSuf);
	totalLen = dirLen + preLen + iLen + sufLen + 1;
	/* imagePath = (char*)malloc(totalLen); */
	memset(imagePath, NULL_CHAR, totalLen);

	strcpy(imagePath, config->imgDir);
	strcpy(imagePath + (dirLen), config->imgPre);
	sprintf(imagePath + (dirLen + preLen), "%d", index);
	strcpy(imagePath + (dirLen + preLen + iLen), config->imgSuf);

	return SP_CONFIG_SUCCESS;
}

SP_CONFIG_MSG spConfigGetPCAPath(char* pcaPath, const SPConfig config)
{
	int dirLen;
	int pcaLen;
	int totalLen;
	if(pcaPath == NULL || config == NULL)
	{
		return SP_CONFIG_INVALID_ARGUMENT;
	}
	dirLen = strlen(config->imgDir);
	pcaLen = strlen(config->pcaFile);
	totalLen = dirLen + pcaLen + 1;
	/* imagePath = (char*)malloc(totalLen*sizeof(char)); */
	memset(pcaPath, NULL_CHAR, totalLen);

	strcpy(pcaPath, config->imgDir);
	strcpy(pcaPath + (dirLen), config->pcaFile);

	return SP_CONFIG_SUCCESS;
}

void spConfigDestroy(SPConfig config)
{
	if(config != NULL)
	{
		CHECK_AND_FREE(config->imgDir);
		CHECK_AND_FREE(config->imgPre);
		CHECK_AND_FREE(config->imgSuf);
		CHECK_AND_FREE(config->pcaFile);
		CHECK_AND_FREE(config->logFile);
		free(config);
	}	
}

int main(int argc, char* argv[])
{
	/* TODO remove */
	SP_CONFIG_MSG msg;
	SPConfig c = spConfigCreate("trump.txt", &msg);
	return 0;
}