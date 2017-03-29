#include <stdbool.h>
#include "SPKDTree.h"

#define NULL_CHAR '\0'
#define COMMENT_CHAR '#'
#define SET_CHAR '='
#define SET_CHAR_STR "="
#define WHITESPACE_CHARS " \f\n\r\t\v"
/* Error messages: */
#define ERR_MSG_INV_LINE "File: %s\nLine: %d\nMessage: Invalid configuration line"
#define ERR_MSG_CONSTRNT "File: %s\nLine: %d\nMessage: Invalid value - constraint not met"
#define ERR_MSG_NO_VALUE "File: %s\nLine: %d\nMessage: Parameter %s is not set"
/* Var nums: */
#define VARNUM_imgDir 0
#define VARNUM_imgPre 1
#define VARNUM_imgSuf 2
#define VARNUM_imgNum 3
#define VARNUM_pcaDim 4
#define VARNUM_pcaFile 5
#define VARNUM_featureNum 6
#define VARNUM_extractMode 7
#define VARNUM_knnNumImg 8
#define VARNUM_splitMethod 9
#define VARNUM_knnNumFeatures 10
#define VARNUM_useMinGUI 11
#define VARNUM_logLvl 12
#define VARNUM_logFile 13
/* Var names: */
#define VARNAME_imgDir "spImagesDirectory"
#define VARNAME_imgPre "spImagesPrefix"
#define VARNAME_imgSuf "spImagesSuffix"
#define VARNAME_imgNum "spNumOfImages"
#define VARNAME_pcaDim "spPCADimension"
#define VARNAME_pcaFile "spPCAFilename"
#define VARNAME_featureNum "spNumOfFeatures"
#define VARNAME_extractMode "spExtractionMode"
#define VARNAME_knnNumImg "spNumOfSimilarImages"
#define VARNAME_splitMethod "spKDTreeSplitMethod"
#define VARNAME_knnNumFeatures "spKNN"
#define VARNAME_useMinGUI "spMinimalGUI"
#define VARNAME_logLvl "spLoggerLevel"
#define VARNAME_logFile "spLoggerFilename"
/* Boolean Names: */
#define VAL_BOOL_TRUE "true"
#define VAL_BOOL_FALSE "false"
/* Splie Method Names: */
#define VAL_SPLIT_RAND "RANDOM"
#define VAL_SPLIT_MAX "MAX_SPREAD"
#define VAL_SPLIT_INC "INCREMENTAL"
/* Allowed suffixes: */
#define VAL_SUFFIX_JPG ".jpg"
#define VAL_SUFFIX_PNG ".png"
#define VAL_SUFFIX_BMP ".bmp"
#define VAL_SUFFIX_GIF ".gif"
/* Macros: */
#define COMPARE_AND_RETURN_INT(var, num, name) if((var)==(num)){return(name);}
#define COMPARE_AND_RETURN_STR(var, name, num) if(!strcmp((var),(name))){return(num);}
typedef enum sp_config_util_msg_t {
	SP_CONFIG_UTIL_SUCCESS,
	SP_CONFIG_UTIL_EMPTY_LINE,
	SP_CONFIG_UTIL_BAD_LINE
} SP_CONFIG_UTIL_MSG;

int spConfigUtilCountDigits(int num);

SP_CONFIG_UTIL_MSG spConfigUtilParseLine(char* line, char** var, char** val);

bool spConfigUtilIsInt(int varNum);

bool spConfigUtilIsBoolean(int varNum);

bool spConfigUtilIsString(int varNum);

int spConfigUtilGetVarNum(char* varName);

char* spConfigUtilGetVarName(int varNum);

bool spConfigUtilSetAtLoc(void* loc, int varNum, char* valStr);

bool spConfigUtilSetIntAtLoc(int* loc, char* valStr);

bool spConfigUtilSetBooleanAtLoc(int* loc, char* valStr);

bool spConfigUtilSetStringAtLoc(char** loc, char* valStr);

bool spConfigUtilsSetStringSuffixAtLoc(char** loc, char* valStr);

bool spConfigUtilSetSplitAtLoc(SP_KDT_SPLIT* loc, char* valStr);

void spConfigUtilPrintUnset(int unset);