#ifndef SPCONFIG_H_
#define SPCONFIG_H_

#include <stdbool.h>
#include <stdio.h>
#include "SPLogger.h"
#include "SPKDTree.h"

/**
 * A data-structure which is used for configuring the system.
 */

typedef enum sp_config_msg_t {
	SP_CONFIG_MISSING_DIR,
	SP_CONFIG_MISSING_PREFIX,
	SP_CONFIG_MISSING_SUFFIX,
	SP_CONFIG_MISSING_NUM_IMAGES,
	SP_CONFIG_CANNOT_OPEN_FILE,
	SP_CONFIG_ALLOC_FAIL,
	SP_CONFIG_INVALID_INTEGER,
	SP_CONFIG_INVALID_STRING,
	SP_CONFIG_INVALID_ARGUMENT,
	SP_CONFIG_INDEX_OUT_OF_RANGE,
	SP_CONFIG_SUCCESS
} SP_CONFIG_MSG;

typedef enum sp_config_line_status_t {
	SP_CONFIG_ST_SUCCESS,
	SP_CONFIG_ST_ERR,
	SP_CONFIG_ST_END
} SP_CONFIG_LINE_STATUS;

typedef struct sp_config_t* SPConfig;

/* Constants: */
#define VARS_COUNT 14
#define MAX_LINE_SIZE 1024 // may be assumed according to the forum
#define TRUE_STR "true"
#define FALSE_STR "false"
/* Split method names: */
#define SPLIT_RAND "RANDOM"
#define SPLIT_MAX "MAX_SPREAD"
#define SPLIT_INC "INCREMENTAL"
/* Allowed suffixes: */
#define SUFFIX_JPG ".jpg"
#define SUFFIX_PNG ".png"
#define SUFFIX_BMP ".bmp"
#define SUFFIX_GIF ".gif"
/* Error messagesL */
#define ERR_MSG_INVALID_LINE "File: %s\nLine: %d\nMessage: Invalid configuration line"
#define ERR_MSG_INVALID_VAL "File: %s\nLine: %d\nMessage: Invalid value - constraint not met"
#define ERR_MSG_UNSET_PARAM "File: %s\nLine: %d\nMessage: Parameter %s is not set"
#define ERR_MSG_OPEN_CFG_FILE "The %s configuration file %s couldn't be open"
#define ERR_MSG_PART_DEFAULT "default "
#define ERR_MSG_PART_NON_DEFAULT ""
/* Special characters: */
#define SPACE_CHAR ' '
#define NEWLINE_CHAR '\n'
#define SET_CHAR '='
#define COMMENT_CHAR '#'
#define NULL_CHAR '\0'
/* vars nums and names: */
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
/* Default values: */
#define DEFAULT_PCA_DIM 20
#define DEFAULT_PCA_FILE "pca.yml"
#define DEFAULT_CFG_FILE "spcbir.config"
#define DEFAULT_FEATURES_NUM 100
#define DEFAULT_EXT_MODE true
#define DEFAULT_MIN_GUI false
#define DEFAULT_SIM_IMG_NUM 1
#define DEFAULT_KNN_FEATS 1
#define DEFAULT_SPLIT_METHOD SP_KD_SPLIT_MAX_SPREAD
#define DEFAULT_LOG_LEVEL 3
#define DEFAULT_LOG_FILE "stdout"
/* Macros: */
#define FREE_IF_SET(var, loc) if(set[(loc)] == 1){free(var);}
#define CFG_GET(var) do{\
		if(config==NULL)\
			{\
				*msg=SP_CONFIG_INVALID_ARGUMENT;\
				return NULL;\
			}\
		*msg = SP_CONFIG_SUCCESS;\
		return (var);\
	}while(0);
#define cfgSet(var, varLoc, val) do{\
		(var) = (val);
		set[(varLoc)] = 2;
	}while(0);
#define SKIP_WHITESPACES(ptr) do{\
		while((ptr)[0] == SPACE_CHAR || (ptr)[0] == NEWLINE_CHAR) {\
			(ptr)++;\
		}\
	}while(0);
#define SKIP_TO_SPECIAL(ptr) do{\
		while((ptr)[0] != SPACE_CHAR && (ptr)[0] != NEWLINE_CHAR &&\
			(ptr)[0] != NULL_CHAR && (ptr)[0] != SET_CHAR) {\
			(ptr)++;\
		}\
	}while(0);
#define VAR_IS_INT(name) (!(strcmp((name), VARNAME_imgNum)*\
							strcmp((name), VARNAME_pcaDim)*\
							strcmp((name), VARNAME_featureNum)*\
							strcmp((name), VARNAME_knnNumImg)*\
							strcmp((name), VARNAME_knnNumFeatures)*\
							strcmp((name), VARNAME_logLvl)))
#define IS_GOOD_SUFFIX(buf) (!(strcmp((buf), SUFFIX_BMP)*\
							strcmp((buf), SUFFIX_JPG)*\
							strcmp((buf), SUFFIX_PNG)*\
							strcmp((buf), SUFFIX_GIF)))
#define IS_BOOL(buf) (!(strcmp((buf), TRUE_STR)*\
							strcmp((buf), FALSE_STR)))
#define IS_GOOD_SPLIT(buf)(!(strcmp((buf), SPLIT_INC)*\
							strcmp((buf), SPLIT_MAX)*\
							strcmp((buf), SPLIT_RAND)))
#define ELSE_SET_INV_INT 
#define CHECK_VAR_NUM(name, comp, num) if(!strcmp((name), (comp))){return (num);}
/**
 * Creates a new system configuration struct. The configuration struct
 * is initialized based on the configuration file given by 'filename'.
 * 
 * @param filename - the name of the configuration file
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return NULL in case an error occurs. Otherwise, a pointer to a struct which
 * 		   contains all system configuration.
 * 
 * The resulting value stored in msg is as follow:
 * - SP_CONFIG_INVALID_ARGUMENT - if filename == NULL
 * - SP_CONFIG_CANNOT_OPEN_FILE - if the configuration file given by filename cannot be open
 * - SP_CONFIG_ALLOC_FAIL - if an allocation failure occurred
 * - SP_CONFIG_INVALID_INTEGER - if a line in the config file contains invalid integer
 * - SP_CONFIG_INVALID_STRING - if a line in the config file contains invalid string
 * - SP_CONFIG_MISSING_DIR - if spImagesDirectory is missing
 * - SP_CONFIG_MISSING_PREFIX - if spImagesPrefix is missing
 * - SP_CONFIG_MISSING_SUFFIX - if spImagesSuffix is missing 
 * - SP_CONFIG_MISSING_NUM_IMAGES - if spNumOfImages is missing
 * - SP_CONFIG_SUCCESS - in case of success
 *
 *
 */
SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg, int set[]);

void spConfigInitConfig(spConfig config, int set[]);

SP_CONFIG_LINE_STATUS spConfigProcessLine(spConfigCreate config,
				File* f, SP_CONFIG_MSG* msg);

SP_CONFIG_MSG spConfigSplitLine(char* line, char* src, char* dst);

int spConfigVarNum(char* varName);

char* spConfigVarName(int varNum);

SP_CONFIG_LINE_STATUS spConfigAssertAndSet(spConfig config,
				int varNum, char* valStr, SP_CONFIG_MSG* msg);

char* spConfigGetImgDir(const spConfigCreate config, SP_CONFIG_MSG* msg);

char* spConfigGetImgPrefix(const spConfigCreate config, SP_CONFIG_MSG* msg);

char* spConfigGetImgSuffix(const spConfigCreate config, SP_CONFIG_MSG* msg);

/*
 * Returns the number of images set in the configuration file, i.e the value
 * of spNumOfImages.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
int spConfigGetNumOfImages(const SPConfig config, SP_CONFIG_MSG* msg);

/**
 * Returns the dimension of the PCA. i.e the value of spPCADimension.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
int spConfigGetPCADim(const SPConfig config, SP_CONFIG_MSG* msg);

char* spConfigGetPCAFile(const spConfig config, SP_CONFIG_MSG* msg);

/*
 * Returns the number of features to be extracted. i.e the value
 * of spNumOfFeatures.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
int spConfigGetNumOfFeatures(const SPConfig config, SP_CONFIG_MSG* msg);

/*
 * Returns true if spExtractionMode = true, false otherwise.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return true if spExtractionMode = true, false otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
bool spConfigIsExtractionMode(const SPConfig config, SP_CONFIG_MSG* msg);

int spConfigGetNumOfSimilarImages(const SPConfig config, SP_CONFIG_MSG* msg);

SP_KDT_SPLIT spConfgGetSplitMethod(const spConfig config, SP_CONFIG_MSG* msg);

bool spConfigGetNumOfSimilarFeatures(const SPConfig config, SP_CONFIG_MSG* msg);

/*
 * Returns true if spMinimalGUI = true, false otherwise.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return true if spExtractionMode = true, false otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
bool spConfigMinimalGui(const SPConfig config, SP_CONFIG_MSG* msg);

int spConfigGetLogLevel(const SPConfig config, SP_CONFIG_MSG* msg);

char* spConfigGetLogLevel(const SPConfig config, SP_CONFIG_MSG* msg);

/**
 * Given an index 'index' the function stores in imagePath the full path of the
 * ith image.
 *
 * For example:
 * Given that the value of:
 *  spImagesDirectory = "./images/"
 *  spImagesPrefix = "img"
 *  spImagesSuffix = ".png"
 *  spNumOfImages = 17
 *  index = 10
 *
 * The functions stores "./images/img10.png" to the address given by imagePath.
 * Thus the address given by imagePath must contain enough space to
 * store the resulting string.
 *
 * @param imagePath - an address to store the result in, it must contain enough space.
 * @param config - the configuration structure
 * @param index - the index of the image.
 *
 * @return
 * - SP_CONFIG_INVALID_ARGUMENT - if imagePath == NULL or config == NULL
 * - SP_CONFIG_INDEX_OUT_OF_RANGE - if index >= spNumOfImages
 * - SP_CONFIG_SUCCESS - in case of success
 */
SP_CONFIG_MSG spConfigGetImagePath(char* imagePath, const SPConfig config,
		int index);

/**
 * The function stores in pcaPath the full path of the pca file.
 * For example given the values of:
 *  spImagesDirectory = "./images/"
 *  spPcaFilename = "pca.yml"
 *
 * The functions stores "./images/pca.yml" to the address given by pcaPath.
 * Thus the address given by pcaPath must contain enough space to
 * store the resulting string.
 *
 * @param imagePath - an address to store the result in, it must contain enough space.
 * @param config - the configuration structure
 * @return
 *  - SP_CONFIG_INVALID_ARGUMENT - if imagePath == NULL or config == NULL
 *  - SP_CONFIG_SUCCESS - in case of success
 */
SP_CONFIG_MSG spConfigGetPCAPath(char* pcaPath, const SPConfig config);

int spConfigGetUnset(int wereSet);

void spConfigDestroyPartial(spConfig config, int set[]);

/**
 * Frees all memory resources associate with config. 
 * If config == NULL nothig is done.
 */
void spConfigDestroy(SPConfig config);

#endif /* SPCONFIG_H_ */
