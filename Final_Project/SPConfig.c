#ifndef SPCONFIG_H_
#define SPCONFIG_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h> // for errno
#include "auxil.h"
#include "SPLogger.h"
#include "SPConfig.h"
#include "SPKDTree.h"

struct sp_config_t{
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

struct sp_str_chain{
	char* value,
	stringChain next
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
	int lineNum = -1, unset
	SP_CONFIG_LINE_STATUS lineRet;
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
		lineRet = spConfigProcessLine(config, f, &retMsg, wereSet);
		lineNum++;
	}while(retMsg != SP_CONFIG_SUCCESS && lineRet == SP_CONFIG_ST_SUCCESS);
	if(lineRet == SP_CONFIG_ST_ERR)
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
	if(unset <= VARS_COUNT)
	{
		printf(ERR_MSG_UNSET_PARAM, filename, lineNum, spConfigVarName(unset));
		spConfigDestroyPartial(config, wereSet);
		*msg = spConfigGetMissingmessage(unset);
		return NULL;
	}
	return config;
}

void spConfigInitConfig(spConfig config, int set[])
{
	cfgSet(config->_pcaDim,			VARNUM_pcaDim,			DEFAULT_PCA_DIM);
	cfgSet(config->_pcaFile,		VARNUM_pcaFile,			DEFAULT_PCA_FILE);
	cfgSet(config->_featureNum,		VARNUM_featureNum,		DEFAULT_FEATURES_NUM);
	cfgSet(config->_extractMode,	VARNUM_extractMode,		DEFAULT_EXT_MODE);
	cfgSet(config->_useMinGUI,		VARNUM_useMinGUI,		DEFAULT_MIN_GUI);
	cfgSet(config->_knnNumImg,		VARNUM_knnNumImg,		DEFAULT_SIM_IMG_NUM);
	cfgSet(config->_knnNumFeatures, VARNUM_knnNumFeatures,	DEFAULT_KNN_FEATS);
	cfgSet(config->_splitMethod,	VARNUM_splitMethod,		DEFAULT_SPLIT_METHOD);
	cfgSet(config->_logLvl,			VARNUM_logLvl,			DEFAULT_LOG_LEVEL);
	cfgSet(config->_logFile,		VARNUM_logFile, 		DEFAULT_LOG_FILE);
}

SP_CONFIG_LINE_STATUS spConfigProcessLine(spConfig config,
				File* f, SP_CONFIG_MSG* msg, int set[])
{
	int size = MAX_LINE_SIZE, read, varNum;
	char* lineBuff = (char*)malloc((size+1)*sizeof(char));
	char *src, *dst;
	struct sp_str_chain* lineChain;
	SP_CONFIG_LINE_STATUS ret;
	memset(lineBuff, NULL_CHAR, size)
	read = getline(&linebuff, &size, f);
	if(read<0)
	{
		if(errno == 0)
		{
			ret = SP_CONFIG_ST_END;
		}else{
			ret = SP_CONFIG_ST_ERR;
		}
	}else{
		*msg = spConfigSplitLine(lineBuff, &src, &dst);
		if(*msg == NULL)
		{
			if(VAR_IS_INT(src))
			{
				*msg = SP_CONFIG_INVALID_INTEGER;
			}else{
				*msg = SP_CONFIG_INVALID_STRING;
			}
		}
		if(*msg != SP_CONFIG_SUCCESS)
		{
			ret = SP_CONFIG_ST_ERR;
		}else{
			varNum = spConfigVarNum(src);
			ret = spConfigAssertAndSet(config, varNum, dst, msg);
		}
	}
	free(lineBuff);
	return ret;
}

SP_CONFIG_MSG spConfigSplitLine(char* line, char** src, char** dst)
{
	char *srcEnd, *dstEnd;
	SKIP_WHITESPACES(line);
	if(line[0] == SET_CHAR || line[0] == NULL_CHAR)
	{
		return SP_CONFIG_INVALID_STRING;
	}
	*src = line;
	SKIP_TO_SPECIAL(line);
	srcEnd = line;
	SKIP_WHITESPACES(line);
	if(line[0] != SET_CHAR)
	{
		return SP_CONFIG_INVALID_STRING;
	}
	line++;
	SKIP_WHITESPACES(line);
	if(line[0] == NULL_CHAR || line[0] == SET_CHAR)
	{
		return NULL;
	}
	*dst = line;
	SKIP_TO_SPECIAL(line);
	dstEnd = line;
	SKIP_WHITESPACES(line);
	if(line[0] != NULL_CHAR)
	{
		return NULL;
	}
	*srcEnd = NULL_CHAR;
	*dstEnd = NULL_CHAR;
	return SP_CONFIG_SUCCESS;
}

int spConfigVarNum(char* varName)
{
	CHECK_VAR_NUM(varName,	VARNAME_imgDir,			VARNUM_imgDir);
	CHECK_VAR_NUM(varName,	VARNAME_imgPre,			VARNUM_imgPre);
	CHECK_VAR_NUM(varName,	VARNAME_imgSuf,			VARNUM_imgSuf);
	CHECK_VAR_NUM(varName,	VARNAME_imgNum,			VARNUM_imgNum);
	CHECK_VAR_NUM(varName,	VARNAME_pcaDim,			VARNUM_pcaDim);
	CHECK_VAR_NUM(varName,	VARNAME_pcaFile,		VARNUM_pcaFile);
	CHECK_VAR_NUM(varName,	VARNAME_featureNum,		VARNUM_featureNum);
	CHECK_VAR_NUM(varName,	VARNAME_extractMode,	VARNUM_extractMode);
	CHECK_VAR_NUM(varName,	VARNAME_knnNumImg,		VARNUM_knnNumImg);
	CHECK_VAR_NUM(varName,	VARNAME_splitMethod,	VARNUM_splitMethod);
	CHECK_VAR_NUM(varName,	VARNAME_knnNumFeatures,	VARNUM_knnNumFeatures);
	CHECK_VAR_NUM(varName,	VARNAME_useMinGUI,		VARNUM_useMinGUI);
	CHECK_VAR_NUM(varName,	VARNAME_logLvl,			VARNUM_logLvl);
	CHECK_VAR_NUM(varName,	VARNAME_logFile,		VARNUM_logFile);
}
char* spConfigVarName(int varNum)
{
	switch(varNum)
	{
		case VARNUM_imgDir:			return VARNAME_imgDir;
		case VARNUM_imgPre:			return VARNAME_imgPre;
		case VARNUM_imgSuf:			return VARNAME_imgSuf;
		case VARNUM_imgNum:			return VARNAME_imgNum;
		case VARNUM_pcaDim:			return VARNAME_pcaDim;
		case VARNUM_pcaFile:		return VARNAME_pcaFile;
		case VARNUM_featureNum:		return VARNAME_featureNum;
		case VARNUM_extractMode:	return VARNAME_extractMode;
		case VARNUM_knnNumImg:		return VARNAME_knnNumImg;
		case VARNUM_splitMethod:	return VARNAME_splitMethod;
		case VARNUM_knnNumFeatures:	return VARNAME_knnNumFeatures;
		case VARNUM_useMinGUI:		return VARNAME_useMinGUI;
		case VARNUM_logLvl:			return VARNAME_logLvl;
		case VARNUM_logFile:		return VARNAME_logFile;
	}
	return NULL;

}

SP_CONFIG_LINE_STATUS spConfigAssertAndSet(spConfig config,
				int varNum, char* valStr, SP_CONFIG_MSG* msg)
{
	int len = strlen(valStr), pVal = -1, i;
	char* pBuff = (char*)malloc((len+1)*sizeof(char));
	bool isNum = True;
	SP_CONFIG_LINE_STATUS ret = SP_CONFIG_ST_SUCCESS;
	strcpy(pBuff, valStr);
	if(isNum(pBuff)){ pVal = atoi(pBuff); }
	switch(varNum){
		case VARNUM_imgDir:
			spConfig->imgDir = pBuff;
			break;
		case VARNUM_imgPre:
			spConfig->imgPre = pBuff;
			break;
		case VARNUM_imgSuf:
			if(IS_GOOD_SUFFIX(pBuff))
			{
				spConfig->imgSuf = pBuff;
			}else{ ret = SP_CONFIG_ST_ERR; *msg = SP_CONFIG_INVALID_STRING; }
			break;
		case VARNUM_imgNum:
			if(pVal>0) {
				spConfig->imgNum = pVal;
			}else{ ret = SP_CONFIG_ST_ERR; *msg = SP_CONFIG_INVALID_INTEGER; }
			break;
		case VARNUM_pcaDim:
			if(pVal>=10 && pVal <= 28) {
				spConfig->pcaDim = pVal;
			}else{ ret = SP_CONFIG_ST_ERR; *msg = SP_CONFIG_INVALID_INTEGER; }
			break;
		case VARNUM_pcaFile:
			spConfig->pcaFile = pBuff;
			break;
		case VARNUM_featureNum:
			if(pVal>0) {
				spConfig->featureNum = pVal;
			}else{ ret = SP_CONFIG_ST_ERR; *msg = SP_CONFIG_INVALID_INTEGER; }
			break;
		case VARNUM_extractMode:
			if(IS_BOOL(pBuff)) {
				spConfig->extractMode = (strcmp(pBuff, TRUE_STR))?false:true;
			}else{ ret = SP_CONFIG_ST_ERR; *msg = SP_CONFIG_INVALID_STRING; }
			break;
		case VARNUM_knnNumImg:
			if(pVal>0) {
				spConfig->knnNumImg = pVal;
			}else{ ret = SP_CONFIG_ST_ERR; *msg = SP_CONFIG_INVALID_INTEGER; }
			break;
		case VARNUM_splitMethod:
			if(IS_GOOD_SPLIT(pBuff)) {
				if(strcmp(pBuff, SPLIT_INC)) { spConfig->splitMethod = SP_KD_SPLIT_INCREMENTAL; }
				if(strcmp(pBuff, SPLIT_MAX)) { spConfig->splitMethod = SP_KD_SPLIT_MAX_SPREAD; }
				if(strcmp(pBuff, SPLIT_RAND)){ spConfig->splitMethod = SP_KD_SPLIT_RANDOM; }
			}else{ ret = SP_CONFIG_ST_ERR; *msg = SP_CONFIG_INVALID_STRING; }
			break;
		case VARNUM_knnNumFeatures:
			if(pVal>0) {
				spConfig->knnNumFeatures = pVal;
			}else{ ret = SP_CONFIG_ST_ERR; *msg = SP_CONFIG_INVALID_INTEGER; }
			break;
		case VARNUM_useMinGUI:
			if(IS_BOOL(pBuff)) {
				spConfig->useMinGUI = (strcmp(pBuff, TRUE_STR))?false:true;
			}else{ ret = SP_CONFIG_ST_ERR; *msg = SP_CONFIG_INVALID_STRING; }
			break;
		case VARNUM_logLvl:
			if(pVal>=1 && pVal <= 4) {
				spConfig->logLvl = pVal;
			}else{ ret = SP_CONFIG_ST_ERR; *msg = SP_CONFIG_INVALID_INTEGER; }
			break;
		case VARNUM_logFile:
			spConfig->logFile = pBuff;
			break;
	}
	return ret;
}

char* spConfigGetImgDir(const spConfig config, SP_CONFIG_MSG* msg)
{
	CFG_GET(config->imgDir);
}
char* spConfigGetImgPrefix(const spConfigCreate config, SP_CONFIG_MSG* msg)
{
	CFG_GET(config->imgPre);
}
char* spConfigGetImgSuffix(const spConfigCreate config, SP_CONFIG_MSG* msg)
{
	CFG_GET(config->imgSuf);
}
int spConfigGetNumOfImages(const SPConfig config, SP_CONFIG_MSG* msg)
{
	CFG_GET(config->imgNum);
}
int spConfigGetPCADim(const SPConfig config, SP_CONFIG_MSG* msg)
{
	CFG_GET(config->pcaDim);
}
char* spConfigGetPCAFile(const spConfig config, SP_CONFIG_MSG* msg)
{
	CFG_GET(config->pcaFile);
}
int spConfigGetNumOfFeatures(const SPConfig config, SP_CONFIG_MSG* msg)
{
	CFG_GET(config->featureNum);
}
bool spConfigIsExtractionMode(const SPConfig config, SP_CONFIG_MSG* msg)
{
	CFG_GET(config->extractMode);
}
int spConfigGetNumOfSimilarImages(const SPConfig config, SP_CONFIG_MSG* msg)
{
	CFG_GET(config->knnNumImg);
}
SP_KDT_SPLIT spConfgGetSplitMethod(const spConfig config, SP_CONFIG_MSG* msg)
{
	CFG_GET(config->splitMethod);
}
bool spConfigGetNumOfSimilarFeatures(const SPConfig config, SP_CONFIG_MSG* msg)
{
	CFG_GET(config->knnNumFeatures);
}
bool spConfigMinimalGui(const SPConfig config, SP_CONFIG_MSG* msg)
{
	CFG_GET(config->useMinGUI);
}
int spConfigGetLogLevel(const SPConfig config, SP_CONFIG_MSG* msg)
{
	CFG_GET(config->logLvl);
}
char* spConfigGetLogLevel(const SPConfig config, SP_CONFIG_MSG* msg)
{
	CFG_GET(config->logFile);
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

int spConfigGetUnset(int wereSet)
{
	int i = 0;
	while(wereSet[i] != 0)
	{
		i++;
	}
	return i;
}

void spConfigDestroyPartial(spConfig config, int set[])
{
	if(config != NULL)
	{
		FREE_IF_SET(config->imgDir, VARNUM_imgDir);
		FREE_IF_SET(config->imgPre, VARNUM_imgPre);
		FREE_IF_SET(config->imgSuf, VARNUM_imgSuf);
		FREE_IF_SET(config->pcaFile, VARNUM_pcaFile);
		FREE_IF_SET(config->logFile, VARNUM_logFile);
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
