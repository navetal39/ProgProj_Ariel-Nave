#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SPConfigUtil.h"
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
	char *var, *val, *line = (char*)malloc((MAX_LINE_SIZE+2)*sizeof(char));
	SP_CONFIG_UTIL_MSG utilMsg;
	configFile = fopen(filename, READ_MODE);
	if(configFile == NULL)
	{
		/* TODO print error */
		*msg = SP_CONFIG_CANNOT_OPEN_FILE;
		return NULL;
	}
	config = (SPConfig)malloc(sizeof(struct sp_config_t));
	if(config == NULL)
	{
		/* TODO print error */
		*msg = SP_CONFIG_ALLOC_FAIL;
		fclose(configFile);
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
			*msg = SP_CONFIG_BAD_LINE;
			/* TODO print error */
			fclose(configFile);
			free(line);
			return NULL;
		}
		if(utilMsg != SP_CONFIG_UTIL_EMPTY_LINE &&
			!spConfigSetValue(config, var, val))
		{
			*msg =(spConfigUtilIsInt(spConfigUtilGetVarNum(var)))
				?SP_CONFIG_INVALID_INTEGER
				:SP_CONFIG_INVALID_STRING;
		}
		lineNum++;
	}
	unset = spConfigGetUnset(config);
	if(unset >= 0)
	{
		spConfigUtilPrintUnset(unset);
		free(config);
		config = NULL;
	}
	fclose(configFile);
	free(line);
	return config;
}

void spConfigInitDefaults(SPConfig config)
{
	config->pcaDim			=	DEFAULT_PCA_DIM;
	config->pcaFile			=	DEFAULT_PCA_FILE;
	config->featureNum		=	DEFAULT_FEATURES_NUM;
	config->extractMode		=	DEFAULT_EXT_MODE;
	config->useMinGUI		=	DEFAULT_MIN_GUI;
	config->knnNumImg		=	DEFAULT_SIM_IMG_NUM;
	config->knnNumFeatures	=	DEFAULT_KNN_FEATS;
	config->splitMethod		=	DEFAULT_SPLIT_METHOD;
	config->logLvl			=	DEFAULT_LOG_LEVEL;
	config->logFile			=	DEFAULT_LOG_FILE;
}

bool spConfigSetValue(SPConfig config, char* var, char* val)
{
	int varNum = spConfigUtilGetVarNum(var);
	void* loc;
	if(varNum<VARNUM_imgDir||varNum>VARNUM_logFile){ return false; }
	switch(varNum)
	{
		case VARNUM_imgDir:
			loc = (void*)(&(config->imgDir));
			break;
		case VARNUM_imgPre:
			loc = (void*)(&(config->imgPre));
			break;
		case VARNUM_imgSuf:
			loc = (void*)(&(config->imgSuf));
			break;
		case VARNUM_imgNum:
			loc = (void*)(&(config->imgNum));
			break;
		case VARNUM_pcaDim:
			loc = (void*)(&(config->pcaDim));
			break;
		case VARNUM_pcaFile:
			loc = (void*)(&(config->pcaFile));
			break;
		case VARNUM_featureNum:
			loc = (void*)(&(config->featureNum));
			break;
		case VARNUM_extractMode:
			loc = (void*)(&(config->extractMode));
			break;
		case VARNUM_knnNumImg:
			loc = (void*)(&(config->knnNumImg));
			break;
		case VARNUM_splitMethod:
			loc = (void*)(&(config->splitMethod));
			break;
		case VARNUM_knnNumFeatures:
			loc = (void*)(&(config->knnNumFeatures));
			break;
		case VARNUM_useMinGUI:
			loc = (void*)(&(config->useMinGUI));
			break;
		case VARNUM_logLvl:
			loc = (void*)(&(config->logLvl));
			break;
		case VARNUM_logFile:
			loc = (void*)(&(config->logFile));
			break;
		default:
			loc = NULL;
			break;
	}
	return spConfigUtilSetAtLoc(loc, varNum, val);
}

int spConfigGetUnset(SPConfig config)
{
	CHECK_UNSET(config->imgDir,			NULL,					VARNUM_imgDir);
	CHECK_UNSET(config->imgPre,			NULL,					VARNUM_imgPre);
	CHECK_UNSET(config->imgSuf,			NULL,					VARNUM_imgSuf);
	CHECK_UNSET(config->imgNum,			0,						VARNUM_imgNum);
	CHECK_UNSET(config->pcaDim,			0,						VARNUM_pcaDim);
	CHECK_UNSET(config->pcaFile,		NULL,					VARNUM_pcaFile);
	CHECK_UNSET(config->featureNum,		0,						VARNUM_featureNum);
	CHECK_UNSET(config->extractMode,	0,						VARNUM_extractMode);
	CHECK_UNSET(config->knnNumImg,		0,						VARNUM_knnNumImg);
	CHECK_UNSET(config->splitMethod,	SP_KD_SPLIT_UNKNOWN,	VARNUM_splitMethod);
	CHECK_UNSET(config->knnNumFeatures,	0,						VARNUM_knnNumFeatures);
	CHECK_UNSET(config->useMinGUI,		0,						VARNUM_useMinGUI);
	CHECK_UNSET(config->logLvl,			0,						VARNUM_logLvl);
	CHECK_UNSET(config->logFile,		NULL,					VARNUM_logFile);
	return -1;
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

SP_KDT_SPLIT spConfgGetSplitMethod(const SPConfig config, SP_CONFIG_MSG* msg)
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
	if(imagePath == NULL || config == NULL)
	{
		return SP_CONFIG_INVALID_ARGUMENT;
	}
	if(index < config->imgNum)
	{
		return SP_CONFIG_INDEX_OUT_OF_RANGE;
	}
	int dirLen = strlen(config->imgDir);
	int preLen = strlen(config->imgPre);
	int iLen = spConfigUtilCountDigits(index);
	int sufLen = strlen(config->imgSuf);
	int totalLen = dirLen + preLen + iLen + sufLen + 1;
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
	if(pcaPath == NULL || config == NULL)
	{
		return SP_CONFIG_INVALID_ARGUMENT;
	}
	int dirLen = strlen(config->imgDir);
	int pcaLen = strlen(config->pcaFile);
	int totalLen = dirLen + pcaLen + 1;
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
		free(config->imgDir);
		free(config->imgPre);
		free(config->imgSuf);
		free(config->pcaFile);
		free(config->logFile);
		free(config);
	}	
}

int main()
{
	return 0;
}