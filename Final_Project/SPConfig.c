#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> /* for isdigit */
#include "SPLogger.h"
#include "SPConfig.h"

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

/********************/
/* Creating methods */
/********************/
SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg)
{
	FILE* cfgFile;
	SPConfig cfg = NULL;
	int success, lineNum = 0;
	char *line, *missing = NULL;
	bool readAll = false;
	if(!filename)
	{
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return NULL;
	}
	if(!(cfgFile = fopen(filename, READ_MODE)))
	{
		/* TODO: print error? */
		*msg = SP_CONFIG_CANNOT_OPEN_FILE;
		return NULL;
	}
	if(!(cfg = (SPConfig)malloc(sizeof(struct sp_config_t))))
	{
		/* TODO: print error? */
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
			/* TODO: print error? */
			*msg = SP_CONFIG_ALLOC_FAIL;
			fclose(cfgFile);
			spConfigDestroy(cfg);
			free(line);
			return NULL;
		}
		if(!fgets(line, MAX_LINE_SIZE+1, cfgFile))
		{
			free(line);
			readAll = true;
			break;
		}
		spConfigParseAndSet(cfg, line, filename, lineNum, msg);
		free(line);
		if(*msg != SP_CONFIG_SUCCESS){ break; }
		lineNum++;
	}
	fclose(cfgFile);
	if(readAll)
	{
		*msg = spConfigCheckAllSet(cfg);
		if(*msg != SP_CONFIG_SUCCESS)
		{
			switch(*msg)
			{
				case SP_CONFIG_MISSING_DIR:
					missing = VARN_IMG_DIR;
					break;
				case SP_CONFIG_MISSING_PREFIX:
					missing = VARN_IMG_PRE;
					break;
				case SP_CONFIG_MISSING_SUFFIX:
					missing = VARN_IMG_SUF;
					break;
				case SP_CONFIG_MISSING_NUM_IMAGES:
					missing = VARN_IMG_NUM;
					break;
			}
			printf(ERR_MSG_UNSET_PARAM, filename, lineNum, missing);
			spConfigDestroy(cfg);
			return NULL;
		}
	}
	return cfg;
}
void spConfigSetDefaults(SPConfig config, SP_CONFIG_MSG* msg)
{
	char *defPcaFile, *defLogFile;
	if(!(defPcaFile = (char*)malloc(strlen(DEFAULT_PCA_FILE)+1)))
	{
		/* TODO: print error? */
		*msg = SP_CONFIG_ALLOC_FAIL;
		return;
	}
	if(!(defLogFile = (char*)malloc(strlen(DEFAULT_LOG_FILE)+1)))
	{
		/* TODO: print error? */
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

void spConfigParseAndSet(SPConfig cfg, char* line, const char* fileName, int lineNum, SP_CONFIG_MSG* msg)
{
	char *var = NULL, *val = NULL;
	spConfigParseLine(line, &var, &val, msg);
	if(*msg == SP_CONFIG_SUCCESS)
	{
		/* Line syntax is OK */
		if(var)
		{
			/* not empty or comment */
			spConfigAnalizeAndSet(cfg, var, val, msg);
			if(*msg != SP_CONFIG_INVALID_ARGUMENT)
			{
				/* var name is recognized */
				if(*msg != SP_CONFIG_SUCCESS)
				{
					/* not successful - constraints must have not been met */
					printf(ERR_MSG_INVALID_VAL, fileName, lineNum);
				}
				/* constraitns have been met, or not and error was printed */
				return;
			}
		}else{
			/* empty or comment line */
			return;
		}
	}
	/* Line syntax is not OK, or is OK but var name is not recognized */
	printf(ERR_MSG_INVALID_LINE, fileName, lineNum);
	
}

void spConfigParseLine(char* line, char** varName, char** valStr, SP_CONFIG_MSG* msg)
{
	char *varNameH, *valStrH, *shouldBeNull;
	char *varNameT, *valStrT;
	if(line[0] == SET_CHAR){
		/* line starts with set char */
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
			*msg = SP_CONFIG_INVALID_STRING;
			return;
		}else{
			/* Empty line */
			*msg = SP_CONFIG_SUCCESS;
			return;
		}
	}
	if(varNameT[0] == COMMENT_CHAR){
		/* Comment line */
		*msg = SP_CONFIG_SUCCESS;
		return;
	}
	if(strtok(NULL, WHITESPACE_STR)){
		/* 2 or more words before set char (or no set char at all) */
		*msg = SP_CONFIG_INVALID_STRING;
		return;
	}
	valStrT = (valStrH)?strtok(valStrH, WHITESPACE_STR):NULL;
	if(valStrT == NULL){
		/* nothing after set char (maybe whitespaces) */
		*msg = SP_CONFIG_INVALID_STRING;
		return;
	}
	if(strtok(NULL, WHITESPACE_STR)){
		/* 2 or more words after set char */
		*msg = SP_CONFIG_INVALID_STRING;
		return;		
	}
	/* line's syntax is good */
	*msg = SP_CONFIG_SUCCESS;
	*varName = varNameT;
	*valStr = valStrT;
}

void spConfigAnalizeAndSet(SPConfig cfg, char* var, char* val, SP_CONFIG_MSG* msg)
{
	bool isDec = isDecimalNumber(val), isBool = isBooleanValue(val), 
		s=false, s2=false;
	SP_KDT_SPLIT splt;
	if(!strcmp(var,VARN_IMG_DIR)){  s=true;
		spConfigSetImgDir(cfg, strdup(val), msg);
	}
	if(!strcmp(var,VARN_IMG_PRE)){ s=true;
		spConfigSetImgPrefix(cfg, strdup(val), msg);
	}
	if(!strcmp(var,VARN_IMG_SUF)){ s=true;
		spConfigSetImgSuffix(cfg, strdup(val), msg);
	}
	if(!strcmp(var,VARN_IMG_NUM)){ s=true;
		if(!isDec)	{*msg = SP_CONFIG_INVALID_INTEGER;}
		else 		{spConfigSetNumOfImages(cfg, atoi(val), msg);}
	}
	if(!strcmp(var,VARN_PCA_DIM)){ s=true;
		if(!isDec)	{*msg = SP_CONFIG_INVALID_INTEGER;}
		else 		{spConfigSetPCADim(cfg, atoi(val), msg);}
	}
	if(!strcmp(var,VARN_PCA_FIL)){ s=true;
		spConfigSetPCAFile(cfg, strdup(val), msg);
	}
	if(!strcmp(var,VARN_FTR_NUM)){ s=true;
		if(!isDec)	{*msg = SP_CONFIG_INVALID_INTEGER;}
		else 		{spConfigSetNumOfFeatures(cfg, atoi(val), msg);}
	}
	if(!strcmp(var,VARN_EXT_MOD)){ s=true;
		if(!isBool)	{*msg = SP_CONFIG_INVALID_STRING;}
		else 		{spConfigSetExtractionMode(cfg, strcmp(val,FALSE_STR), msg);}
	}
	if(!strcmp(var,VARN_SIM_NUM)){ s=true;
		if(!isDec)	{*msg = SP_CONFIG_INVALID_INTEGER;}
		else 		{spConfigSetNumOfSimilarImages(cfg, atoi(val), msg);}
	}
	if(!strcmp(var,VARN_SPL_MTD)){ s=true;
		if(!strcmp(val,SPLIT_STR_RND)){splt = SP_KD_SPLIT_RANDOM;		s2=true;}
		if(!strcmp(val,SPLIT_STR_MAX)){splt = SP_KD_SPLIT_MAX_SPREAD;	s2=true;}
		if(!strcmp(val,SPLIT_STR_INC)){splt = SP_KD_SPLIT_INCREMENTAL;	s2=true;}
		if(s2)		{spConfigSetSplitMethod(cfg, splt, msg);}
		else		{*msg = SP_CONFIG_INVALID_STRING; }
	}
	if(!strcmp(var,VARN_KNN_NUM)){ s=true;
		if(!isDec)	{*msg = SP_CONFIG_INVALID_INTEGER;}
		else 		{spConfigSetNumOfSimilarFeatures(cfg, atoi(val), msg);}
	}
	if(!strcmp(var,VARN_MIN_GUI)){ s=true;
		if(!isBool)	{*msg = SP_CONFIG_INVALID_STRING;}
		else 		{spConfigSetMinimalGui(cfg, strcmp(val,FALSE_STR), msg);}
	}
	if(!strcmp(var,VARN_LOG_LVL)){ s=true;
		if(!isDec)	{*msg = SP_CONFIG_INVALID_INTEGER;}
		else 		{spConfigSetLogLevel(cfg, atoi(val), msg);}
	}
	if(!strcmp(var,VARN_LOG_FIL)){ s=true;
		spConfigSetLogFile(cfg, strdup(val), msg);
	}
	if(!s) { *msg = SP_CONFIG_INVALID_ARGUMENT; };
}

SP_CONFIG_MSG spConfigCheckAllSet(SPConfig config)
{
	if(!(config->imgDirSet)) {return SP_CONFIG_MISSING_DIR; }
	if(!(config->imgPreSet)) {return SP_CONFIG_MISSING_PREFIX; }
	if(!(config->imgSufSet)) {return SP_CONFIG_MISSING_SUFFIX; }
	if(!(config->imgNumSet)) {return SP_CONFIG_MISSING_NUM_IMAGES; }
	return SP_CONFIG_SUCCESS;
}

/************************/
/* Getters and Setters: */
/************************/

/* image directory */
char* spConfigGetImgDir(const SPConfig config, SP_CONFIG_MSG* msg)
{
	GETTER_BODY(config->imgDir, NULL);
}
void spConfigSetImgDir(const SPConfig config, char* val, SP_CONFIG_MSG* msg)
{
	config->imgDir = val;
	config->imgDirSet = true;
	*msg = SP_CONFIG_SUCCESS;
}
/* image prefix */
char* spConfigGetImgPrefix(const SPConfig config, SP_CONFIG_MSG* msg)
{
	GETTER_BODY(config->imgPre, NULL);
}
void spConfigSetImgPrefix(const SPConfig config, char* val, SP_CONFIG_MSG* msg)
{
	config->imgPre = val;
	config->imgPreSet = true;
	*msg = SP_CONFIG_SUCCESS;
}
/* image suffix*/
char* spConfigGetImgSuffix(const SPConfig config, SP_CONFIG_MSG* msg)
{
	GETTER_BODY(config->imgSuf, NULL);
}
void spConfigSetImgSuffix(const SPConfig config, char* val, SP_CONFIG_MSG* msg)
{
	config->imgSuf = val;
	config->imgSufSet = true;
	*msg = SP_CONFIG_SUCCESS;
}
/* num of images */
int spConfigGetNumOfImages(const SPConfig config, SP_CONFIG_MSG* msg)
{
	GETTER_BODY(config->imgNum, -1);
}
void spConfigSetNumOfImages(const SPConfig config, int val, SP_CONFIG_MSG* msg)
{
	if(val <= 0)
	{
		*msg = SP_CONFIG_INVALID_INTEGER;
	}else{
		*msg = SP_CONFIG_SUCCESS;
		config->imgNum = val;
		config ->imgNumSet = true;
	}
}
/* PCA dimension */
int spConfigGetPCADim(const SPConfig config, SP_CONFIG_MSG* msg)
{
	GETTER_BODY(config->pcaDim, -1);
}
void spConfigSetPCADim(const SPConfig config, int val, SP_CONFIG_MSG* msg)
{
	if(val < 10 || val > 28)
	{
		*msg = SP_CONFIG_INVALID_INTEGER;
	}else{
		*msg = SP_CONFIG_SUCCESS;
		config->pcaDim = val;
	}
}
/* PCA file */
char* spConfigGetPCAFile(const SPConfig config, SP_CONFIG_MSG* msg)
{
	GETTER_BODY(config->pcaFile, NULL);
}
void spConfigSetPCAFile(const SPConfig config, char* val, SP_CONFIG_MSG* msg)
{
	config->pcaFile = val;
	*msg = SP_CONFIG_SUCCESS;
}
/* number of features */
int spConfigGetNumOfFeatures(const SPConfig config, SP_CONFIG_MSG* msg)
{
	GETTER_BODY(config->featureNum, -1);
}
void spConfigSetNumOfFeatures(const SPConfig config, int val, SP_CONFIG_MSG* msg)
{
	if(val <= 0)
	{
		*msg = SP_CONFIG_INVALID_INTEGER;
	}else{
		*msg = SP_CONFIG_SUCCESS;
		config->featureNum = val;
	}
}
/* extraction mode */
bool spConfigIsExtractionMode(const SPConfig config, SP_CONFIG_MSG* msg)
{
	GETTER_BODY(config->extractMode == 1, false);
}
void spConfigSetExtractionMode(const SPConfig config, bool val, SP_CONFIG_MSG* msg)
{
	config->extractMode = val;
}
/* number of similar images */
int spConfigGetNumOfSimilarImages(const SPConfig config, SP_CONFIG_MSG* msg)
{
	GETTER_BODY(config->simImgNum, -1);
}
void spConfigSetNumOfSimilarImages(const SPConfig config, int val, SP_CONFIG_MSG* msg)
{
	if(val <= 0)
	{
		*msg = SP_CONFIG_INVALID_INTEGER;
	}else{
		*msg = SP_CONFIG_SUCCESS;
		config->simImgNum = val;
	}
}
/* split method */
SP_KDT_SPLIT spConfigGetSplitMethod(const SPConfig config, SP_CONFIG_MSG* msg)
{
	GETTER_BODY(config->splitMethod, SP_KD_SPLIT_UNKNOWN);
}
void spConfigSetSplitMethod(const SPConfig config, SP_KDT_SPLIT val, SP_CONFIG_MSG* msg)
{
	config->splitMethod = val;
}
/* KNN number */
int spConfigGetNumOfSimilarFeatures(const SPConfig config, SP_CONFIG_MSG* msg)
{
	GETTER_BODY(config->knnNum, false);
}
void spConfigSetNumOfSimilarFeatures(const SPConfig config, int val, SP_CONFIG_MSG* msg)
{
	if(val<=0)
	{
		*msg = SP_CONFIG_INVALID_INTEGER;
	}else{
		*msg = SP_CONFIG_SUCCESS;
		config->knnNum = val;
	}
}
/* minimal GUI */
bool spConfigMinimalGui(const SPConfig config, SP_CONFIG_MSG* msg)
{
	GETTER_BODY(config->useMinGUI == 1, false);
}
void spConfigSetMinimalGui(const SPConfig config, bool val, SP_CONFIG_MSG* msg)
{
	config->useMinGUI = val;
}
/* log level */
int spConfigGetLogLevel(const SPConfig config, SP_CONFIG_MSG* msg)
{
	GETTER_BODY(config->logLvl, -1);
}
void spConfigSetLogLevel(const SPConfig config, int val, SP_CONFIG_MSG* msg)
{
	if(val<=0 || val > 4)
	{
		*msg = SP_CONFIG_INVALID_INTEGER;
	}else{
		*msg = SP_CONFIG_SUCCESS;
		config->logLvl = val;
	}
}
/* log file */
char* spConfigGetLogFile(const SPConfig config, SP_CONFIG_MSG* msg)
{
	GETTER_BODY(config->logFile, NULL);
}
void spConfigSetLogFile(const SPConfig config, char* val, SP_CONFIG_MSG* msg)
{
	config->logFile = val;
	*msg = SP_CONFIG_SUCCESS;
}
/*******************/
/* Helping methods */
/*******************/
SP_CONFIG_MSG spConfigGetImagePath(char* imagePath, const SPConfig config,
		int index)
{
	int dirLen, preLen, iLen, sufLen, totalLen, temp;
	if(imagePath == NULL || config == NULL)
	{
		return SP_CONFIG_INVALID_ARGUMENT;
	}
	if(index > config->imgNum || index < 0)
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
/****************************************************/
/* Methods not related directly to the config file: */
/****************************************************/
bool isDecimalNumber(char* str)
{
	str += (*str==NEG_CHAR)?1:0;
	while(*str)
	{
		if(!isdigit(*(str++))){ return false; }
	}
	return true;
}
bool isBooleanValue(char* str)
{
	return (!strcmp(str, TRUE_STR))||(!strcmp(str, FALSE_STR));
}
void spConfigDestroy(SPConfig config)
{
	if(config != NULL)
	{
		if(config->imgDirSet){
			free(config->imgDir); 
		}
		if(config->imgPreSet){
			free(config->imgPre);
		}
		if(config->imgSufSet){
		 free(config->imgSuf);
		}
		free(config->pcaFile);
		free(config->logFile);
		free(config);
	}	
}