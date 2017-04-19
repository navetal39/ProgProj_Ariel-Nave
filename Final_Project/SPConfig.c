#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SPLogger.h"
#include "SPConfig.h"
/* TODO:
 - remove printfs
 - check var names and error if unrecognized
 - check values according to var and error if bad
 - set values at correct places
 -- change extra bools when needed!
 - add good error printing
 */

struct sp_config_t{
	char* imgDir;	bool imgDirSet;
	char* imgPre;	bool imgPreSet;
	char* imgSuf;	bool imgSufSet;
	int imgNum;		bool imgNumSet;
	int pcaDim; 
	char* pcaFile;
	int featureNum;
	bool extractMode;
	int simImgNum;
	SP_KDT_SPLIT splitMethod;
	int knnNum;
	bool useMinGUI;
	int logLvl;
	char* logFile;
};

SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg)
{
	FILE* cfgFile;
	SPConfig cfg = NULL;
	int success, lineNum = 0;
	char* line;
	if(!filename)
	{
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return NULL;
	}
	if(!(cfgFile = fopen(filename, READ_MODE)))
	{
		printf("can't open file\n");
		*msg = SP_CONFIG_CANNOT_OPEN_FILE;
		return NULL;
	}
	if(!(cfg = (SPConfig)malloc(sizeof(struct sp_config_t))))
	{
		printf("can't alloc config struct\n");
		*msg = SP_CONFIG_ALLOC_FAIL;
		fclose(cfgFile);
		return NULL;
	}
	memset(cfgFile, 0, sizeof(struct sp_config_t));
	spConfigSetDefaults(cfg, msg);
	while(true)
	{
		if(!(line = (char*)malloc((MAX_LINE_SIZE+2)*sizeof(char))))
		{
			printf("can't alloc line\n");
			*msg = SP_CONFIG_ALLOC_FAIL;
			fclose(cfgFile);
			spConfigDestroy(cfg);
			free(line);
			return NULL;
		}
		if(!fgets(line, MAX_LINE_SIZE+1, cfgFile))
		{
			free(line);
			break;
		}
		spConfigParseAndSet(cfg, line, lineNum, msg);
		free(line);
		lineNum++;
	}
	fclose(cfgFile);
	*msg = SP_CONFIG_SUCCESS;
	return cfg;
}
void spConfigSetDefaults(SPConfig config, SP_CONFIG_MSG* msg)
{
	char *defPcaFile, *defLogFile;
	if(!(defPcaFile = (char*)malloc(strlen(DEFAULT_PCA_FILE)+1)))
	{
		printf("can't alloc pca def\n");
		*msg = SP_CONFIG_ALLOC_FAIL;
		return;
	}
	if(!(defLogFile = (char*)malloc(strlen(DEFAULT_LOG_FILE)+1)))
	{
		printf("can't alloc log def\n");
		*msg = SP_CONFIG_ALLOC_FAIL;
		free(defPcaFile);
		return;
	}
	strcpy(defPcaFile, DEFAULT_PCA_FILE);
	strcpy(defLogFile, DEFAULT_LOG_FILE);
	config->pcaDim = DEFAULT_PCA_DIM;
	config->pcaFile = defPcaFile;
	config->featureNum = DEFAULT_FEATURES_NUM;
	config->extractMode = DEFAULT_EXT_MODE;
	config->useMinGUI = DEFAULT_MIN_GUI;
	config->simImgNum = DEFAULT_SIM_IMG_NUM;
	config->knnNum = DEFAULT_KNN;
	config->splitMethod = DEFAULT_SPLIT_METHOD;
	config->logLvl = DEFAULT_LOG_LEVEL;
	config->logFile = defLogFile;
}

void spConfigParseAndSet(SPConfig cfg, char* line, int lineNum, SP_CONFIG_MSG* msg)
{
	char *var = NULL, *val = NULL;
	printf("line: %s\n", line);
	spConfigParseLine(line, &var, &val, msg);
	if(*msg == SP_CONFIG_SUCCESS && var)
	{
		printf("var: %s, val: %s\n", var, val);
	}
}

void spConfigParseLine(char* line, char** varName, char** valStr, SP_CONFIG_MSG* msg)
{
	char *varNameH, *valStrH, *shouldBeNull;
	char *varNameT, *valStrT;
	if(line[0] == SET_CHAR){
		/* line starts with set char */
		printf("starts with =!\n");
		*msg = SP_CONFIG_INVALID_STRING;
		return;
	}
	/* Split line by set char */
	varNameH = strtok(line, SET_CHAR_STR);
	valStrH = strtok(NULL, SET_CHAR_STR);
	shouldBeNull = strtok(NULL, SET_CHAR_STR);
	/*check 1st half*/
	varNameT = (varNameH)?strtok(varNameH, WHITESPACE_STR):NULL;
	if(varNameT == NULL){
		if(valStrH){
			/* Nothing before 1st instance of set char */
			printf("0 before =!\n");
			*msg = SP_CONFIG_INVALID_STRING;
			return;
		}else{
			/* Empty line */
			printf("empty!\n");
			*msg = SP_CONFIG_SUCCESS;
			return;
		}
	}
	if(varNameT[0] == COMMENT_CHAR){
		/* Comment line */
		printf("comment!\n");
		*msg = SP_CONFIG_SUCCESS;
		return;
	}
	if(strtok(NULL, WHITESPACE_STR)){
		/* 2 or more words before set char (or no set char at all) */
		printf("2 before =!n");
		*msg = SP_CONFIG_INVALID_STRING;
		return;
	}
	valStrT = (valStrH)?strtok(valStrH, WHITESPACE_STR):NULL;
	if(valStrT == NULL){
		/* nothing after set char (maybe whitespaces) */
		printf("0 after =!\n");
		*msg = SP_CONFIG_INVALID_STRING;
		return;
	}
	if(strtok(NULL, WHITESPACE_STR)){
		/* 2 or more words after set char */
		printf("2 after =!n");
		*msg = SP_CONFIG_INVALID_STRING;
		return;		
	}
	*msg = SP_CONFIG_SUCCESS;
	*varName = varNameT;
	*valStr = valStrT;
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
	GETTER_BODY(config->simImgNum, -1);
}

SP_KDT_SPLIT spConfigGetSplitMethod(const SPConfig config, SP_CONFIG_MSG* msg)
{
	GETTER_BODY(config->splitMethod, SP_KD_SPLIT_UNKNOWN);
}

int spConfigGetNumOfSimilarFeatures(const SPConfig config, SP_CONFIG_MSG* msg)
{
	GETTER_BODY(config->knnNum, false);
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
	int dirLen, preLen, iLen, sufLen, totalLen, temp;
	if(imagePath == NULL || config == NULL)
	{
		return SP_CONFIG_INVALID_ARGUMENT;
	}
	if(index < config->imgNum)
	{
		return SP_CONFIG_INDEX_OUT_OF_RANGE;
	}
	temp = index; iLen = 1; 
	while(temp > 9)
	{
		temp /= 10; iLen++;
	}
	dirLen = strlen(config->imgDir);
	preLen = strlen(config->imgPre);
	sufLen = strlen(config->imgSuf);
	totalLen = dirLen + preLen + iLen + sufLen + 1;
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
	memset(pcaPath, NULL_CHAR, totalLen);

	strcpy(pcaPath, config->imgDir);
	strcpy(pcaPath + (dirLen), config->pcaFile);

	return SP_CONFIG_SUCCESS;
}

void spConfigDestroy(SPConfig config)
{
	if(config != NULL)
	{
		if(config->imgDirSet){ free(config->imgDir); }
		if(config->imgPreSet){ free(config->imgPre); }
		if(config->imgSufSet){ free(config->imgSuf); }
		free(config->pcaFile);
		free(config->logFile);
		free(config);
	}	
}

int main(int argc, char* argv[])
{
	/* TODO remove */
	SP_CONFIG_MSG msg;
	SPConfig cfg = spConfigCreate("./trump", &msg);
	spConfigDestroy(cfg);
	return 0;
}