#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> /* for isdigit */
#include "SPConfigUtil.h"

int spConfigUtilCountDigits(int num)
{
	int count = 0;
	do{
		num /= 10;
		count++;
	}while(num!=0);
	return count;
}

SP_CONFIG_UTIL_MSG spConfigUtilParseLine(char* line, char** var, char** val)
{
	char* temp[4];
	int tempLen;
	temp[0] = strtok(line, WHITESPACE_CHARS);
	temp[1] = strtok(NULL, WHITESPACE_CHARS);
	temp[2] = strtok(NULL, WHITESPACE_CHARS);
	temp[3] = strtok(NULL, WHITESPACE_CHARS);

	if(temp[0]==NULL || temp[0][0] == COMMENT_CHAR) {
		return SP_CONFIG_UTIL_EMPTY_LINE;
	}
	if(temp[0] != NULL && temp[1] == NULL) {
		*var = strtok(temp[0],  SET_CHAR_STR);
		*val = strtok(NULL, SET_CHAR_STR);
		if(*var == NULL || *val == NULL){
			return SP_CONFIG_UTIL_BAD_LINE;
		}
	}
	if(temp[1] != NULL && temp[2] == NULL) {
		tempLen = strlen(temp[0]);
		if(temp[0][tempLen-1] == SET_CHAR && temp[1][0] != SET_CHAR) {
			temp[0][tempLen-1] = NULL_CHAR;
		}
		if(temp[0][tempLen-1] != SET_CHAR && temp[1][0] == SET_CHAR) {
			temp[1]++;
		}
		if((temp[0][tempLen-1] == SET_CHAR && temp[1][0] == SET_CHAR)||
			(temp[0][tempLen-1] != SET_CHAR && temp[1][0] != SET_CHAR)) {
			return SP_CONFIG_UTIL_BAD_LINE;
		}
		*var = temp[0];
		*val = temp[1];
	}
	if(temp[2] != NULL && temp[3] == NULL) {
		if(strcmp(temp[1], SET_CHAR_STR)) {
			return SP_CONFIG_UTIL_BAD_LINE;
		}else{
			*var = temp[0];
			*val = temp[2];
		}
	}
	if(temp[3] != NULL) {
		return SP_CONFIG_UTIL_BAD_LINE;
	}
	return SP_CONFIG_UTIL_SUCCESS;
}

bool spConfigUtilIsInt(int varNum)
{
	return (varNum == VARNUM_imgNum			|| varNum == VARNUM_pcaDim		||
			varNum == VARNUM_featureNum		|| varNum == VARNUM_knnNumImg	|| 
			varNum == VARNUM_knnNumFeatures	|| varNum == VARNUM_logLvl);
}
bool spConfigUtilIsBoolean(int varNum)
{
	return (varNum == VARNUM_extractMode || varNum == VARNUM_useMinGUI);
}
bool spConfigUtilIsString(int varNum)
{
	return (varNum == VARNUM_imgDir	|| varNum == VARNUM_imgPre	||
			varNum == VARNUM_imgSuf	|| varNum == VARNUM_pcaFile	|| 
			varNum == VARNUM_logFile); 
}

int spConfigUtilGetVarNum(char* varName)
{
	COMPARE_AND_RETURN_STR(varName, VARNAME_imgDir,			VARNUM_imgDir);
	COMPARE_AND_RETURN_STR(varName, VARNAME_imgPre,			VARNUM_imgPre);
	COMPARE_AND_RETURN_STR(varName, VARNAME_imgSuf,			VARNUM_imgSuf);
	COMPARE_AND_RETURN_STR(varName, VARNAME_imgNum,			VARNUM_imgNum);
	COMPARE_AND_RETURN_STR(varName, VARNAME_pcaDim,			VARNUM_pcaDim);
	COMPARE_AND_RETURN_STR(varName, VARNAME_pcaFile,		VARNUM_pcaFile);
	COMPARE_AND_RETURN_STR(varName, VARNAME_featureNum,		VARNUM_featureNum);
	COMPARE_AND_RETURN_STR(varName, VARNAME_extractMode,	VARNUM_extractMode);
	COMPARE_AND_RETURN_STR(varName, VARNAME_knnNumImg,		VARNUM_knnNumImg);
	COMPARE_AND_RETURN_STR(varName, VARNAME_splitMethod,	VARNUM_splitMethod);
	COMPARE_AND_RETURN_STR(varName, VARNAME_knnNumFeatures,	VARNUM_knnNumFeatures);
	COMPARE_AND_RETURN_STR(varName, VARNAME_useMinGUI,		VARNUM_useMinGUI);
	COMPARE_AND_RETURN_STR(varName, VARNAME_logLvl,			VARNUM_logLvl);
	COMPARE_AND_RETURN_STR(varName, VARNAME_logFile,		VARNUM_logFile);
	return -1;
}
char* spConfigUtilGetVarName(int varNum)
{
	COMPARE_AND_RETURN_INT(varNum, VARNUM_imgDir,			VARNAME_imgDir);
	COMPARE_AND_RETURN_INT(varNum, VARNUM_imgPre,			VARNAME_imgPre);
	COMPARE_AND_RETURN_INT(varNum, VARNUM_imgSuf,			VARNAME_imgSuf);
	COMPARE_AND_RETURN_INT(varNum, VARNUM_imgNum,			VARNAME_imgNum);
	COMPARE_AND_RETURN_INT(varNum, VARNUM_pcaDim,			VARNAME_pcaDim);
	COMPARE_AND_RETURN_INT(varNum, VARNUM_pcaFile,			VARNAME_pcaFile);
	COMPARE_AND_RETURN_INT(varNum, VARNUM_featureNum,		VARNAME_featureNum);
	COMPARE_AND_RETURN_INT(varNum, VARNUM_extractMode,		VARNAME_extractMode);
	COMPARE_AND_RETURN_INT(varNum, VARNUM_knnNumImg,		VARNAME_knnNumImg);
	COMPARE_AND_RETURN_INT(varNum, VARNUM_splitMethod,		VARNAME_splitMethod);
	COMPARE_AND_RETURN_INT(varNum, VARNUM_knnNumFeatures,	VARNAME_knnNumFeatures);
	COMPARE_AND_RETURN_INT(varNum, VARNUM_useMinGUI,		VARNAME_useMinGUI);
	COMPARE_AND_RETURN_INT(varNum, VARNUM_logLvl,			VARNAME_logLvl);
	COMPARE_AND_RETURN_INT(varNum, VARNUM_logFile,			VARNAME_logFile);
	return NULL;
}

bool spConfigUtilSetAtLoc(void* loc, int varNum, char* valStr)
{
	if(spConfigUtilIsInt(varNum))
	{
		return spConfigUtilSetIntAtLoc((int*) loc, valStr);
	}
	if(spConfigUtilIsBoolean(varNum))
	{
		return spConfigUtilSetBooleanAtLoc((int*) loc, valStr);
	}
	if(spConfigUtilIsString(varNum))
	{
		if(varNum == VARNUM_imgSuf)
		{
			spConfigUtilsSetStringSuffixAtLoc((char**) loc, valStr);
		}
		return spConfigUtilSetStringAtLoc((char**) loc, valStr);
	}
	return spConfigUtilSetSplitAtLoc((SP_KDT_SPLIT*) loc, valStr);
}

bool spConfigUtilSetIntAtLoc(int* loc, char* valStr)
{
	char* temp = valStr;
	while(temp[0]) {if(!isdigit(temp[0])){return false;}}
	*loc = atoi(valStr);
	return true;

}
bool spConfigUtilSetBooleanAtLoc(int* loc, char* valStr)
{
	if(!strcmp(valStr, VAL_BOOL_TRUE))
	{
		*loc = 1;
		return true;
	}
	if(!strcmp(valStr, VAL_BOOL_FALSE))
	{
		*loc = -1;
		return true;
	}
	return false;
}
bool spConfigUtilSetStringAtLoc(char** loc, char* valStr)
{
	int len = strlen(valStr);
	char* cpy = (char*)malloc((len+1)*sizeof(char));
	strcpy(cpy, valStr);
	cpy[len] = NULL_CHAR;
	*loc = cpy;
	return true;
}
bool spConfigUtilsSetStringSuffixAtLoc(char** loc, char* valStr)
{
	if(!strcmp(valStr, VAL_SUFFIX_JPG)	|| !strcmp(valStr, VAL_SUFFIX_PNG) ||
		!strcmp(valStr, VAL_SUFFIX_BMP)	|| !strcmp(valStr, VAL_SUFFIX_GIF))
	{
		return spConfigUtilSetStringAtLoc(loc, valStr);
	}
	return false;
}
bool spConfigUtilSetSplitAtLoc(SP_KDT_SPLIT* loc, char* valStr)
{
	if(!strcmp(valStr, VAL_SPLIT_RAND))
	{
		*loc = SP_KD_SPLIT_RANDOM;
		return true;
	}
	if(!strcmp(valStr, VAL_SPLIT_MAX))
	{
		*loc = SP_KD_SPLIT_MAX_SPREAD;
		return true;
	}
	if(!strcmp(valStr, VAL_SPLIT_INC))
	{
		*loc = SP_KD_SPLIT_INCREMENTAL;
		return true;
	}
	return false;
}