#ifndef SPCONFIG_H_
#define SPCONFIG_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
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
	if(filename == NULL)
	{
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return NULL;
	}
	spConfig config = (spConfig)malloc(sizeof(sp_config_t));
	if(config == NULL)
	{
		*msg = SP_CONFIG_ALLOC_FAIL;
		return NULL;
	}
	FILE* f;
	SP_CONFIG_MSG retMsg;
	int wereSet[VARS_COUNT] = {0};
	int lineNum = -1, read;
	int unset;
	char* cfgFileCtgr;
	f = fopen(filename, "r");
	if(f == NULL)
	{
		free(config);
		*msg = SP_CONFIG_CANNOT_OPEN_FILE;
		if(filename.strcmp(DEFAULT_CFG_FILE))
		{
			cfgFileCtgr = ERR_MSG_PART_NON_DEFAULT;
		}else{
			cfgFileCtgr = ERR_MSG_PART_DEFAULT;
		}
		printf(ERR_MSG_OPEN_CFG_FILE, cfgFileCtgr, filename);
		return NULL;
	}
	spConfigInitConfig(config, wereSet)
	do
	{
		read = spConfigGetLine(config, f, &retMsg, wereSet);
		lineNum++;
	}while(retMsg != SP_CONFIG_SUCCESS && read != RET_ERR);
	if(read == RET_ERR)
	{
		// TODO handle error
	}
	*msg = retMsg;
	if(retMsg != SP_CONFIG_SUCCESS_SUCCESS)
	{
		printf(ERR_MSG_INVALID_LINE, filename, lineNum)
		spConfigDestroyPartial(config, wereSet);
		return NULL;
	}
	unset = spConfigGetUnset(wereSet);
	if(!unset)
	{
		printf(ERR_MSG_UNSET_PARAM, filename, lineNum, VAR_AT(unset));
		spConfigDestroyPartial(config, wereSet);
		*msg = spConfigGetMissingmessage(unset);
		return NULL;
	}
	return config;
}

void spConfigInitConfig(spConfig config, int set[])
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
int spConfigGetLine(spConfigCreate config, File* f, SP_CONFIG_MSG* msg, int set[])
{
	int size = MAX_LINE_SIZE, read, obtained;
	char[] lineBuff[size];
	memset(lineBuff, NULL_CHAR, size)
	read = getline(&linebuff, &size, f);
	if(read > 0)
	{
		obtained = spConfigObtainAndSet(config, lineBuff, msg);
		if(obtained >= 0 && obtained < VARS_COUNT)
		{
			set[obtained] = 1;
		}
	}else{
		obtained = RET_ERR;
	}
	return obtained;
}

int spConfigObtainAndSet(spConfigCreate config, char* line, SP_CONFIG_MSG* msg)
{
	char *var, *val;
	int varNum;
	SP_CONFIG_TYPE varType;
	SKIP_SPACES(line);
	if(line[0] == COMMENT_CHAR)
	{
		return RET_COMMENT_LINE;
	}
	var = line;
	SKIP_UNTIL_SPACES(line);
	if(line[0] == SET_CHAR)
	{
		line[0] = NULL_CHAR;
		line++;
	}else{
		line[1] = NULL_CHAR;
		line += 2;
		SKIP_SPACES(line);
		if(line[0] != SET_CHAR)
		{
			*msg = SP_CONFIG_INVALID_STRING; // TODO check forum to see if ok
			return RET_ERR;
		}
		line++;
	}
	varNum = spConfigVarNum(var);
	varType = spConfigVarType(varNum);
	if(varType == NULL)
	{
		*msg = SP_CONFIG_INVALID_STRING;
		return RET_ERR;
	}
	SKIP_SPACES(line);
	val = line;
	SKIP_UNTIL_SPACES(line);
	line[1] = NULL_CHAR;
	line += 2;
	SKIP_SPACES(line);
	if(line[0] != NULL_CHAR)
	{
		if(varType == SP_CONFIG_TYPE_INT)
		{
			*msg = SP_CONFIG_INVALID_INTEGER;
		}else{
			*msg = SP_CONFIG_INVALID_STRING;
		}
		return RET_ERR;
	}
	return spConfigCheckAndSetValue(config, varNum, val, msg);
}

int spConfigVarNum(char* name)
{
	return NULL // TODO lots of strcmprs
}

SP_CONFIG_TYPE spConfigVarType(int num)
{
	if(varNum == VARNUM_imgNum || varNum == VARNUM_pcaDim || 
		varNum == VARNUM_featureNum || varNum == VARNUM_knnNumImg || 
		varNum = VARNUM_knnNumFeatures || varNum == VARNUM_logLvl)
	{
		return SP_CONFIG_TYPE_INT;
	}
	if(varNum == VARNUM_imgDir || varNum == imgPre || varNum == imgSuf || 
		varNum == VARNUM_pcaFile || varNum == VARNUM_logFile)
	{
		return SP_CONFIG_TYPE_STR;
	}
	if(varNum == VARNUM_useMinGUI || varNum == VARNUM_extractMode)
	{
		return SP_CONFIG_TYPE_BOOL;
	}
	if(varNum == VARNUM_splitMethod)
	{
		return SP_CONFIG_TYPE_ENUM;
	}
	return NULL;
}

int spConfigCheckAndSetValue(const SPConfig config, int varNum, char* valStr, SP_CONFIG_MSG* msg)
{
	// TODO make shorter
	char* pBuff;
	int pVal, vType;
	bool metconstraints = false;
	if(spConfigVarType(varNum) == SP_CONFIG_TYPE_INT)
	{
		pVal = atoi(varNum);
	}else{
		pBuff = (char*)malloc(strlen(valStr)*sizeof(char));
		strcpy(pBuff, valStr);
		break;
	}
	switch(varNum)
	{
		case VARNUM_imgDir:
			config->imgDir = pBuff;
			metconstraints = true;
			break;
		case VARNUM_imgPre:
			config->imgPre = pBuff;
			metconstraints = true;
			break;
		case VARNUM_imgSuf:
			config->imgSuf = pBuff;
			metconstraints = true;
			break;
		case VARNUM_imgNum:
			if(pval > 0)
			{
				metconstraints = true;
				config->imgNum = pval;
			}
			break;
		case VARNUM_pcaDim:
			if(pval >= 10 && pval <= 28) 
			{
				metconstraints = true;
				config->imgNum = pval;
			}
			break;
		case VARNUM_pcaFile:
			config->imgSuf = pBuff;
			metconstraints = true;
			break;
		case VARNUM_featureNum:
			if(pval > 0)
			{
				config->featureNum = pval;
			}
			break;
		case VARNUM_extractMode:
			if(!strcmp(pBuff, TRUE_STR))
			{
				config->extractMode = true;
				metconstraints = true;
			}
			if(!strcmp(pBuff, FALSE_STR))
			{
				config->extractMode = false;
				metconstraints = true;
			}
			free(pBuff);
			break;
		case VARNUM_knnNumImg:
			if(pval > 0)
			{
				config->knnNumImg = pval;
				metconstraints = true;
			}
			break;
		case VARNUM_splitMethod:
			if(!strcmp(pBuff, SP_KD_SPLIT_SPLIT_RAND))
			{
				config->splitMethod = RANDOM;
				metconstraints = true;
			}
			if(!strcmp(pBuff, SP_KD_SPLIT_SPLIT_MAX))
			{
				config->splitMethod = MAX_SPREAD;
				metconstraints = true;
			}
			if(!strcmp(pBuff, SP_KD_SPLIT_SPLIT_INC))
			{
				config->splitMethod = INCREMENTAL;
				metconstraints = true;
			}
			free(pBuff);
			break;
		case VARNUM_knnNumFeatures:
			if(pval > 0)
			{
				config->knnNumFeatures = pval;
				metconstraints = true;
			}
			break;
		case VARNUM_useMinGUI:
			if(!strcmp(pBuff, TRUE_STR))
			{
				config->useMinGUI = true;
				metconstraints = true;
			}
			if(!strcmp(pBuff, FALSE_STR))
			{
				config->useMinGUI = false;
				metconstraints = true;
			}
			free(pBuff);
			break;
		case VARNUM_logLvl:
			if(pval > 0 && pval < 3)
			{
				config->logLvl = pval;
				metconstraints = true;
			}
			break;
		case VARNUM_logFile:
			config->imgSuf = pBuff;
			metconstraints = true;
			break;
		default:
			break;
	}
	if(metconstraints)
	{
		*msg = SP_CONFIG_SUCCESS;
		return varNum;
	}else{
		*msg = SP_CONFIG_INVALID_INTEGER;
		return RET_ERR_CONSTRAINTS;
	}
}

char* spConfigGetImgDir(const spConfig config, SP_CONFIG_MSG* msg)
{
	CFG_GET(imgDir);
}

char* spConfigGetImgPrefix(const spConfigCreate config, SP_CONFIG_MSG* msg)
{
	CFG_GET(imgPre);
}

char* spConfigGetImgSuffix(const spConfigCreate config, SP_CONFIG_MSG* msg)
{
	CFG_GET(imgSuf);
}
int spConfigGetNumOfImages(const SPConfig config, SP_CONFIG_MSG* msg)
{
	CFG_GET(imgNum);
}
int spConfigGetPCADim(const SPConfig config, SP_CONFIG_MSG* msg)
{
	CFG_GET(pcaDim);
}
char* spConfigGetPCAFile(const spConfig config, SP_CONFIG_MSG* msg)
{
	CFG_GET(pcaFile);
}
int spConfigGetNumOfFeatures(const SPConfig config, SP_CONFIG_MSG* msg)
{
	CFG_GET(featureNum);
}
bool spConfigIsExtractionMode(const SPConfig config, SP_CONFIG_MSG* msg)
{
	CFG_GET(extractMode);
}
int spConfigGetNumOfSimilarImages(const SPConfig config, SP_CONFIG_MSG* msg)
{
	CFG_GET(knnNumImg);
}
SP_KDT_SPLIT spConfgGetSplitMethod(const spConfig config, SP_CONFIG_MSG* msg)
{
	CFG_GET(splitMethod);
}
bool spConfigGetNumOfSimilarFeatures(const SPConfig config, SP_CONFIG_MSG* msg)
{
	CFG_GET(knnNumFeatures);
}
bool spConfigMinimalGui(const SPConfig config, SP_CONFIG_MSG* msg)
{
	CFG_GET(useMinGUI);
}
int spConfigGetLogLevel(const SPConfig config, SP_CONFIG_MSG* msg)
{
	CFG_GET(logLvl);
}
char* spConfigGetLogLevel(const SPConfig config, SP_CONFIG_MSG* msg)
{
	CFG_GET(logFile);
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

SP_CONFIG_MSG spConfigGetMissingmessage(int offset)
{
	switch(offset)
	{
		case VARNUM_imgDir:
			return SP_CONFIG_MISSING_DIR;
		case VARNUM_imgPre:
			return SP_CONFIG_MISSING_PREFIX;
		case VARNUM_imgSuf:
			return SP_CONFIG_MISSING_SUFFIX;
		case SP_CONFIG_MISSING_NUM_IMAGES:
			return SP_CONFIG_MISSING_NUM_IMAGE;
		default:
			return NULL; // unreachable
	}
}

void spConfigDestroyPartial(spConfig config, int set[])
{
	if(config != NULL)
	{
		FREE_IF_SET(imgDir);
		FREE_IF_SET(imgPre);
		FREE_IF_SET(imgSuf);
		FREE_IF_SET(pcaFile);
		FREE_IF_SET(logFile);
		free(config);
	}
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

#endif /* SPCONFIG_H_ */
