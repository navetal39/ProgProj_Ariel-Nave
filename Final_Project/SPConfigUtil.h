#define NULL_CHAR '\0'
#define COMMENT_CHAR '#'
#define SET_CHAR '='
#define SET_CHAR_STR "="
#define WHITESPACE_CHARS " \f\n\r\t\v"
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

typedef enum sp_config_util_msg_t {
	SP_CONFIG_UTIL_SUCCESS,
	SP_CONFIG_UTIL_EMPTY_LINE,
	SP_CONFIG_UTIL_BAD_VAR,
	SP_CONFIG_UTIL_BAD_VAL,
	SP_CONFIG_UTIL_BAD_LINE
} SP_CONFIG_UTIL_MSG;

int spConfigUtilCountDigits(int num);

SP_CONFIG_UTIL_MSG spConfigUtilParseLine(char* line, char** var, char** val);

SP_CONFIG_UTIL_MSG spConfigUtilIsBadInt(char* var);

void spConfigUtilPrintUnset(int unset);