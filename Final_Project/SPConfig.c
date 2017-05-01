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
	int lineNum = 1;
	char *line, *missing = NULL;
	bool readAll = false;
	if(msg==NULL) {return NULL;}
	if(!filename){
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return NULL;
	}
	if(!(cfgFile = fopen(filename, READ_MODE))){
		/* TODO: print error? */
		*msg = SP_CONFIG_CANNOT_OPEN_FILE;
		return NULL;
	}
	if(!(cfg = (SPConfig)malloc(sizeof(struct sp_config_t)))){
		/* TODO: print error? */
		*msg = SP_CONFIG_ALLOC_FAIL;
		fclose(cfgFile);
		return NULL;
	}
	memset(cfg, 0, sizeof(struct sp_config_t));
	spConfigSetDefaults(cfg, msg);
	while(true){
		if(!(line = (char*)malloc((MAX_LINE_SIZE+2)*sizeof(char))))	{
			/* TODO: print error? */
			*msg = SP_CONFIG_ALLOC_FAIL;
			fclose(cfgFile);
			spConfigDestroy(cfg);
			free(line);
			return NULL;
		}
		if(!fgets(line, MAX_LINE_SIZE+1, cfgFile)){
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
	if(readAll){
		*msg = spConfigCheckAllSet(cfg);
		if(*msg != SP_CONFIG_SUCCESS){
			GET_MISSING(*msg);
			printf(ERR_MSG_UNSET_PARAM, filename, lineNum-1, missing);
			spConfigDestroy(cfg);
			return NULL;
		}
	}else{
		spConfigDestroy(cfg); 
		return NULL;
	}
	return cfg;
}
void spConfigSetDefaults(SPConfig config, SP_CONFIG_MSG* msg){
	if(msg==NULL){ return; }
	char *defPcaFile, *defLogFile;
	if(!(defPcaFile = (char*)malloc(strlen(DEFAULT_PCA_FILE)+1))){
		/* TODO: print error? */
		*msg = SP_CONFIG_ALLOC_FAIL;
		return;
	}
	if(!(defLogFile = (char*)malloc(strlen(DEFAULT_LOG_FILE)+1))){
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
	config->imgDirSet = false;
	config->imgPreSet = false;
	config->imgSufSet = false;
	config->imgNumSet = false;
	*msg = SP_CONFIG_SUCCESS;
}

void spConfigParseAndSet(SPConfig cfg, char* line, const char* fileName,
	int lineNum, SP_CONFIG_MSG* msg){
	if(msg==NULL){ return; }
	char *var = NULL, *val = NULL;
	spConfigParseLine(line, &var, &val, msg);
	if(*msg == SP_CONFIG_SUCCESS){
		/* Line syntax is OK */
		if(var){
			/* not empty or comment */
			spConfigAnalizeAndSet(cfg, var, val, msg);
			if(*msg == SP_CONFIG_ALLOC_FAIL){
				return;
			}
			if(*msg != SP_CONFIG_INVALID_ARGUMENT){
				/* var name is recognized */
				if(*msg != SP_CONFIG_SUCCESS){
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

void spConfigParseLine(char* line, char** varName, char** valStr,
	SP_CONFIG_MSG* msg)
{
	char *varNameH, *valStrH, *shouldBeNull;
	char *varNameT, *valStrT;
	if(msg==NULL){ return; }
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
	if(shouldBeNull || line[strlen(varNameH)+1]==SET_CHAR){
		/* 2 or more set chars - 1st cond. is for seperated set chars,
			2nd cond. is for "==" or longer*/
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
		s=false, s2=false, isStr=false;
	char* valDup;
	if(msg==NULL){ return; }
	if(!(valDup = (char*)malloc((strlen(val)+1)*sizeof(char)))){
		*msg = SP_CONFIG_ALLOC_FAIL; return;
	}
	strcpy(valDup, val);
	SP_KDT_SPLIT splt;
	COMPARE_AND_SET_STR(VARN_IMG_DIR, spConfigSetImgDir);
	COMPARE_AND_SET_STR(VARN_IMG_PRE, spConfigSetImgPrefix);
	COMPARE_AND_SET_STR(VARN_IMG_SUF, spConfigSetImgSuffix);
	COMPARE_AND_SET_STR(VARN_PCA_FIL, spConfigSetPCAFile);
	COMPARE_AND_SET_STR(VARN_LOG_FIL, spConfigSetLogFile);
	COMPARE_AND_SET(VARN_IMG_NUM, isDec, SP_CONFIG_INVALID_INTEGER,
		spConfigSetNumOfImages, atoi(val));
	COMPARE_AND_SET(VARN_PCA_DIM, isDec, SP_CONFIG_INVALID_INTEGER,
		spConfigSetPCADim, atoi(val));
	COMPARE_AND_SET(VARN_FTR_NUM, isDec, SP_CONFIG_INVALID_INTEGER,
		spConfigSetNumOfFeatures, atoi(val));
	COMPARE_AND_SET(VARN_EXT_MOD, isBool, SP_CONFIG_INVALID_STRING,
		spConfigSetExtractionMode, strcmp(val,FALSE_STR));
	COMPARE_AND_SET(VARN_SIM_NUM, isDec, SP_CONFIG_INVALID_INTEGER,
		spConfigSetNumOfSimilarImages, atoi(val));
	COMPARE_AND_SET(VARN_KNN_NUM, isDec, SP_CONFIG_INVALID_INTEGER,
		spConfigSetNumOfSimilarFeatures, atoi(val));
	COMPARE_AND_SET(VARN_MIN_GUI, isBool, SP_CONFIG_INVALID_STRING,
		spConfigSetMinimalGui, strcmp(val,FALSE_STR));
	COMPARE_AND_SET(VARN_LOG_LVL, isDec, SP_CONFIG_INVALID_INTEGER,
		spConfigSetLogLevel, atoi(val));
	if(!strcmp(var,VARN_SPL_MTD)){ s=true;
		if(!strcmp(val,SPLIT_STR_RND)){splt = SP_KD_SPLIT_RANDOM;		s2=true;}
		if(!strcmp(val,SPLIT_STR_MAX)){splt = SP_KD_SPLIT_MAX_SPREAD;	s2=true;}
		if(!strcmp(val,SPLIT_STR_INC)){splt = SP_KD_SPLIT_INCREMENTAL;	s2=true;}
		if(s2)		{spConfigSetSplitMethod(cfg, splt, msg);}
		else		{*msg = SP_CONFIG_INVALID_STRING; }
	}
	if(!s) { *msg = SP_CONFIG_INVALID_ARGUMENT; };
	if(!isStr) {free(valDup);}
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
	if(msg==NULL){ return; }
	free(config->imgDir);
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
	if(msg==NULL){ return; }
	free(config->imgPre);
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
	if(msg==NULL){ return; }
	if(!strcmp(val, SUF_STR_JPG) || !strcmp(val, SUF_STR_PNG) || 
		!strcmp(val, SUF_STR_BMP) || !strcmp(val, SUF_STR_GIF)){
		free(config->imgSuf);
		config->imgSuf = val;
		config->imgSufSet = true;
		*msg = SP_CONFIG_SUCCESS;
	}else{
		*msg = SP_CONFIG_INVALID_STRING;
	}
}
/* num of images */
int spConfigGetNumOfImages(const SPConfig config, SP_CONFIG_MSG* msg)
{
	GETTER_BODY(config->imgNum, -1);
}
void spConfigSetNumOfImages(const SPConfig config, int val, SP_CONFIG_MSG* msg)
{
	if(msg==NULL){ return; }
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
	if(msg==NULL){ return; }
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
	if(msg==NULL){ return; }
	free(config->pcaFile);
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
	if(msg==NULL){ return; }
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
	if(msg==NULL){ return; }
	*msg = SP_CONFIG_SUCCESS;
	config->extractMode = val;
}
/* number of similar images */
int spConfigGetNumOfSimilarImages(const SPConfig config, SP_CONFIG_MSG* msg)
{
	GETTER_BODY(config->simImgNum, -1);
}
void spConfigSetNumOfSimilarImages(const SPConfig config, int val, SP_CONFIG_MSG* msg)
{
	if(msg==NULL){ return; }
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
	if(msg==NULL){ return; }
	*msg = SP_CONFIG_SUCCESS;
	config->splitMethod = val;
}
/* KNN number */
int spConfigGetNumOfSimilarFeatures(const SPConfig config, SP_CONFIG_MSG* msg)
{
	GETTER_BODY(config->knnNum, false);
}
void spConfigSetNumOfSimilarFeatures(const SPConfig config, int val, SP_CONFIG_MSG* msg)
{
	if(msg==NULL){ return; }
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
	if(msg==NULL){ return; }
	*msg = SP_CONFIG_SUCCESS;
	config->useMinGUI = val;
}
/* log level */
int spConfigGetLogLevel(const SPConfig config, SP_CONFIG_MSG* msg)
{
	GETTER_BODY(config->logLvl, -1);
}
void spConfigSetLogLevel(const SPConfig config, int val, SP_CONFIG_MSG* msg)
{
	if(msg==NULL){ return; }
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
	if(msg==NULL){ return; }
	free(config->logFile);
	config->logFile = val;
	*msg = SP_CONFIG_SUCCESS;
}
/*******************/
/* Helping methods */
/*******************/
SP_CONFIG_MSG spConfigGetImagePath(char* imagePath, const SPConfig config,
		int index)
{
	if(imagePath == NULL || config == NULL)
	{
		return SP_CONFIG_INVALID_ARGUMENT;
	}
	if(index > config->imgNum || index < 0)
	{
		return SP_CONFIG_INDEX_OUT_OF_RANGE;
	}
	sprintf(imagePath, "%s%s%d%s", config->imgDir,
	 		config->imgPre, index, config->imgSuf);
	return SP_CONFIG_SUCCESS;
}

SP_CONFIG_MSG spConfigGetFeatsFilePath(char* featsPath, const SPConfig config,
		int index)
{
	if(featsPath == NULL || config == NULL)
	{
		return SP_CONFIG_INVALID_ARGUMENT;
	}
	if(index > config->imgNum || index < 0)
	{
		return SP_CONFIG_INDEX_OUT_OF_RANGE;
	}
	sprintf(featsPath, "%s%s%d%s", config->imgDir,
	 		config->imgPre, index, SUF_STR_FTR);
	return SP_CONFIG_SUCCESS;
}

SP_CONFIG_MSG spConfigGetPCAPath(char* pcaPath, const SPConfig config)
{
	if(pcaPath == NULL || config == NULL)
	{
		return SP_CONFIG_INVALID_ARGUMENT;
	}
	sprintf(pcaPath, "%s%s",config->imgDir, config->pcaFile);
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