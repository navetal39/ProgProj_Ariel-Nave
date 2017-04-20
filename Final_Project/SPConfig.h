#ifndef SPCONFIG_H_
#define SPCONFIG_H_

#include <stdbool.h>
#include "SPLogger.h"
#include "SPKDTree.h"
/* Constants: */
#define MAX_LINE_SIZE 1024 /* may be assumed according to the forum */
#define READ_MODE "r"
#define NULL_CHAR '\0'
#define NEG_CHAR '-'
#define WHITESPACE_STR " \f\n\r\t\v"
#define SET_CHAR '='
#define SET_CHAR_STR "="
#define COMMENT_CHAR '#'
#define TRUE_STR "true"
#define FALSE_STR "false"
#define SPLIT_STR_RND "RANDOM"
#define SPLIT_STR_MAX "MAX_SPREAD"
#define SPLIT_STR_INC "INCREMENTAL"
/* Error messagesL */
#define ERR_MSG_INVALID_LINE "File: %s\nLine: %d\nMessage: Invalid configuration line\n"
#define ERR_MSG_INVALID_VAL "File: %s\nLine: %d\nMessage: Invalid value - constraint not met\n"
#define ERR_MSG_UNSET_PARAM "File: %s\nLine: %d\nMessage: Parameter %s is not set\n"
#define ERR_MSG_OPEN_CFG_FILE "The %sconfiguration file %s couldn't be open\n"
#define ERR_MSG_PART_DEFAULT "default "
#define ERR_MSG_PART_NON_DEFAULT ""
/* Default values: */
#define DEFAULT_CFG_FILE "spcbir.config"

#define DEFAULT_PCA_DIM 20
#define DEFAULT_PCA_FILE "pca.yml"
#define DEFAULT_FEATURES_NUM 100
#define DEFAULT_EXT_MODE true
#define DEFAULT_MIN_GUI false
#define DEFAULT_SIM_IMG_NUM 1
#define DEFAULT_KNN 1
#define DEFAULT_SPLIT_METHOD SP_KD_SPLIT_MAX_SPREAD
#define DEFAULT_LOG_LEVEL 3
#define DEFAULT_LOG_FILE "stdout"
/* variable names: */
#define VARN_IMG_DIR "spImagesDirectory"
#define VARN_IMG_PRE "spImagesPrefix"
#define VARN_IMG_SUF "spImagesSuffix"
#define VARN_IMG_NUM "spNumOfImages"
#define VARN_PCA_DIM "spPCADimension"
#define VARN_PCA_FIL "spPCAFilename"
#define VARN_FTR_NUM "spNumOfFeatures"
#define VARN_EXT_MOD "spExtractionMode"
#define VARN_SIM_NUM "spNumOfSimilarImages"
#define VARN_SPL_MTD "spKDTreeSplitMethod"
#define VARN_KNN_NUM "spKNN"
#define VARN_MIN_GUI "spMinimalGUI"
#define VARN_LOG_LVL "spLoggerLevel"
#define VARN_LOG_FIL "spLoggerFilename"
/* Macros: */
#define GETTER_BODY(toGet, bad) do{\
		if(config == NULL)\
		{\
			*msg = SP_CONFIG_INVALID_ARGUMENT;\
			return (bad);\
		}\
		*msg = SP_CONFIG_SUCCESS;\
		return (toGet);\
	}while(0);
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
	SP_CONFIG_BAD_LINE,
	SP_CONFIG_INDEX_OUT_OF_RANGE,
	SP_CONFIG_SUCCESS
} SP_CONFIG_MSG;

typedef struct sp_config_t* SPConfig;


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
SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg);

void spConfigSetDefaults(SPConfig config, SP_CONFIG_MSG* msg);

void spConfigParseAndSet(SPConfig cfg, char* line, const char* fileName, int lineNum, SP_CONFIG_MSG* msg);

void spConfigParseLine(char* line, char** varName, char** valStr, SP_CONFIG_MSG* msg);

void spConfigAnalizeAndSet(SPConfig cfg, char* var, char* val, SP_CONFIG_MSG* msg);

SP_CONFIG_MSG spConfigCheckAllSet(SPConfig config);

/************************/
/* Getters and Setters: */
/************************/

char* spConfigGetImgDir(const SPConfig config, SP_CONFIG_MSG* msg);
void spConfigSetImgDir(const SPConfig config, char* val, SP_CONFIG_MSG* msg);

char* spConfigGetImgPrefix(const SPConfig config, SP_CONFIG_MSG* msg);
void spConfigSetImgPrefix(const SPConfig config, char* val, SP_CONFIG_MSG* msg);

char* spConfigGetImgSuffix(const SPConfig config, SP_CONFIG_MSG* msg);
void spConfigSetImgSuffix(const SPConfig config, char* val, SP_CONFIG_MSG* msg);

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
void spConfigSetNumOfImages(const SPConfig config, int val, SP_CONFIG_MSG* msg);

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
void spConfigSetPCADim(const SPConfig config, int val, SP_CONFIG_MSG* msg);

char* spConfigGetPCAFile(const SPConfig config, SP_CONFIG_MSG* msg);
void spConfigSetPCAFile(const SPConfig config, char* val, SP_CONFIG_MSG* msg);

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
void spConfigSetNumOfFeatures(const SPConfig config, int val, SP_CONFIG_MSG* msg);

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
void spConfigSetExtractionMode(const SPConfig config, bool val, SP_CONFIG_MSG* msg);

int spConfigGetNumOfSimilarImages(const SPConfig config, SP_CONFIG_MSG* msg);
void spConfigSetNumOfSimilarImages(const SPConfig config, int val, SP_CONFIG_MSG* msg);

SP_KDT_SPLIT spConfigGetSplitMethod(const SPConfig config, SP_CONFIG_MSG* msg);
void spConfigSetSplitMethod(const SPConfig config, SP_KDT_SPLIT val, SP_CONFIG_MSG* msg);

int spConfigGetNumOfSimilarFeatures(const SPConfig config, SP_CONFIG_MSG* msg);
void spConfigSetNumOfSimilarFeatures(const SPConfig config, int val, SP_CONFIG_MSG* msg);

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
void spConfigSetMinimalGui(const SPConfig config, bool val, SP_CONFIG_MSG* msg);

int spConfigGetLogLevel(const SPConfig config, SP_CONFIG_MSG* msg);
void spConfigSetLogLevel(const SPConfig config, int val, SP_CONFIG_MSG* msg);

char* spConfigGetLogFile(const SPConfig config, SP_CONFIG_MSG* msg);
void spConfigSetLogFile(const SPConfig config, char* val, SP_CONFIG_MSG* msg);
/*******************/
/* Helping methods */
/*******************/

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

/****************************************************/
/* Methods not related directly to the config file: */
/****************************************************/
bool isDecimalNumber(char* str);
bool isBooleanValue(char* str);

/**
 * Frees all memory resources associate with config. 
 * If config == NULL nothig is done.
 */
void spConfigDestroy(SPConfig config);

#endif /* SPCONFIG_H_ */
