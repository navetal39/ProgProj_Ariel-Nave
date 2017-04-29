#ifndef SPCONFIG_H_
#define SPCONFIG_H_

#include <stdbool.h>
#include "SPLogger.h"
#include "SPKDTree.h"
/* Constants: */
#define MAX_LINE_SIZE 1024 /* may be assumed according to the forum */
#define READ_MODE "r"
#define WRITE_MODE "w"
#define NULL_CHAR '\0'
#define NEG_CHAR '-'
#define WHITESPACE_STR " \f\n\r\t\v"
#define SET_CHAR '='
#define SET_CHAR_STR "="
#define COMMENT_CHAR '#'
/* Allowed values: */
#define TRUE_STR "true"
#define FALSE_STR "false"

#define SPLIT_STR_RND "RANDOM"
#define SPLIT_STR_MAX "MAX_SPREAD"
#define SPLIT_STR_INC "INCREMENTAL"

#define SUF_STR_JPG ".jpg"
#define SUF_STR_PNG ".png"
#define SUF_STR_BMP ".bmp"
#define SUF_STR_GIF ".gif"
#define SUF_STR_FTR ".feats"
/* Error messages: */
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
		if(msg==NULL){ return(bad); }\
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

/**
 * SPConfig summary
 *
 * the SPConfig contains inforamtion extracted from a configuration file
 * The configuration file is a text file (or at least should be readable as such), with each line representing
 * one configuration variable (unless it's either empty or a comment line.)
 * 
 * The valid configuration line's format is as follows:
 * [ \f\n\r\t\v]*(\S)*[ \f\n\r\t\v]*=[ \f\n\r\t\v]*(\S)*[ \f\n\r\t\v]*\n?
 * with the '\n' not appearing at the end only if it's the last line in the file.
 * Comment lines are lines that have 'W' as their first non-whitespace character.
 *
 * The following functions are supported:
 *
 * spConfigCreate - creates a config struct
 * spConfigSetDefaults - sets the values of all configuration variables in a config struct to their
 * 						default values (if they have one)
 * spConfigParseAndSet - Parses a configuration line and updates a config struct accordingly
 * spConfigParseLine - parses a configuration line
 * spConfigAnalizeAndSet - checks that the given variable and value meet all constraints and updates a config struct
 * 							accordingly
 * spConfigCheckAllSet - checks that all fields in a config struct are set
 * Getters and Setters for each one of the 14 configuration variables
 * spConfigDestroy - frees all memory used by a config struct
 * spConfigGetImagePath - retrives the full path of an image of a specific index
 * spConfigGetPCAPath - retrives the full path of the PCA file
 * 
 */

/** the struct used for kd_tree node**/

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

/**
 * Sets the fields that have default values in the config struct to those vaules.
 * 
 * @param config - a config struct pointer to edit
 * @assert config != NULL
 * @param msg - a pointer in which the msg returned by the function is stored
 * @assert msg != NULL
 *
 * - SP_CONFIG_ALLOC_FAIL - if an allocation error occured.
 * - SP_CONFIG_SUCCESS - in case of success
 */
void spConfigSetDefaults(SPConfig config, SP_CONFIG_MSG* msg);

/**
 * parses the string in "line" and, if it's a valid configuration line, edits "cfg"
 * according to the info inside said line.
 * 
 * @param cfg - a valid config struct pointer to edit
 * @param line - the line to parse
 * @param fileName - the name of the config file (for printing informative errors.)
 * @param lineNum - the line's number in the config file (for printing informative errors.)
 * @param msg - a pointer in which the msg returned by the function is stored
 * @assert msg != NULL
 *
 * - For information about the diffrent values of msg, see documentation of "spConfigParseLine"
 *   and spConfigAnalizeAndSet
 */
void spConfigParseAndSet(SPConfig cfg, char* line, const char* fileName, int lineNum, SP_CONFIG_MSG* msg);

/**
 * parses the string in "line". If it's in the format of a valid configuration line it'll store the variable's
 * name in "varName" and the value (in string format) in "valStr". If it's a comment line or empty line both of these
 * will be set to NULL.
 * 
 * @param line - the line to parse
 * @param varName - a non-NULL pointer to where the variable's name shall be stored (if found)
 * @param varName - a non-NULL pointer to where the value (in string format) shall be stored (if found)
 * @param msg - a pointer in which the msg returned by the function is stored
 * @assert msg != NULL
 * 
 * - SP_CONFIG_INVALID_STRING - if the line's format isn't a legal configuration line format
 * - SP_CONFIG_SUCCESS - in the case of success
 */
void spConfigParseLine(char* line, char** varName, char** valStr, SP_CONFIG_MSG* msg);

/**
 * Checks that the value of "var" is a valid configuration variable name, and if so it will set it's value in
 * the configuration struct pointed at by "cfg" to the value of "val", in the correct format(int/string/bool/etc.)
 * 
 * @param cfg - a config struct pointer to edit
 * @assert config != NULL
 * @param line - the line to parse
 * @param fileName - the name of the config file (for printing informative errors.)
 * @param lineNum - the line's number in the config file (for printing informative errors.)
 * @param msg - a pointer in which the msg returned by the function is stored
 * @assert msg != NULL
 * 
 * - SP_CONFIG_ALLOC_FAIL - if an allocation failure occurred
 * - SP_CONFIG_INVALID_INTEGER - if the value is supposed to be converted to int to be stored but is not a proper base 10 integer.
 * - SP_CONFIG_INVALID_STRING - if the value's not supposed to be converted to int to be stored (bool/split method) but is not one
 *								of the allowed values
 * - SP_CONFIG_INVALID_ARGUMENT - if varName does not match any legal configuration variable
 * - SP_CONFIG_SUCCESS - in the case of success
 * - Additionally, SP_CONFIG_INVALID_INTEGER and SP_CONFIG_INVALID_STRING may be returned in msg by the correct setter function if
 *   the value can be converted to the correct form(int/not int) but does not meet the constraints of the variable.
 */
void spConfigAnalizeAndSet(SPConfig cfg, char* var, char* val, SP_CONFIG_MSG* msg);

/**
 * Checks that all the fields without default values in "config" have been set and returns a message that matches the first filed
 * that was found to be not set, or if all were set.
 * 
 * - SP_CONFIG_MISSING_DIR - if the image directory field has not been set
 * - SP_CONFIG_MISSING_PREFIX - if the image prefix field has not been set
 * - SP_CONFIG_MISSING_SUFFIX - if the image suffix field has not been set
 * - SP_CONFIG_MISSING_NUM_IMAGES - if the number of images field has not been set
 * - SP_CONFIG_SUCCESS - if all fileds with no default values have been set
 */
SP_CONFIG_MSG spConfigCheckAllSet(SPConfig config);

/************************/
/* Getters and Setters: */
/************************/

/*
 * Returns the images directory set in the configuration file, i.e the value
 * of spImagesDirectory.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
char* spConfigGetImgDir(const SPConfig config, SP_CONFIG_MSG* msg);

/*
 * Checks that the given value "val" meets the constraints of spImagesDirectory, and if
 * they do it sets the fields in "config" such that when asked for the value of spImagesDirectory,
 * the value of "val" will be returned.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param val - the value to replace the current one in the config struct
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
void spConfigSetImgDir(const SPConfig config, char* val, SP_CONFIG_MSG* msg);

/*
 * Returns the images prefix set in the configuration file, i.e the value
 * of spImagesPrefix.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
char* spConfigGetImgPrefix(const SPConfig config, SP_CONFIG_MSG* msg);

/*
 * Checks that the given value "val" meets the constraints of spImagesPrefix, and if
 * they do it sets the fields in "config" such that when asked for the value of spImagesPrefix,
 * the value of "val" will be returned.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param val - the value to replace the current one in the config struct
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
void spConfigSetImgPrefix(const SPConfig config, char* val, SP_CONFIG_MSG* msg);

/*
 * Returns the images suffix set in the configuration file, i.e the value
 * of spImagesSuffix.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
char* spConfigGetImgSuffix(const SPConfig config, SP_CONFIG_MSG* msg);

/*
 * Checks that the given value "val" meets the constraints of spImagesSuffix, and if
 * they do it sets the fields in "config" such that when asked for the value of spImagesSuffix,
 * the value of "val" will be returned.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param val - the value to replace the current one in the config struct
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_INVALID_STRING - if the constraints on val are not met
 * - SP_CONFIG_SUCCESS - in case of success
 * 
 */
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

/*
 * Checks that the given value "val" meets the constraints of spNumOfImages, and if
 * they do it sets the fields in "config" such that when asked for the value of spNumOfImages,
 * the value of "val" will be returned.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param val - the value to replace the current one in the config struct
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_INVALID_INTEGER - if the constraints on val are not met
 * - SP_CONFIG_SUCCESS - in case of success
 */
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

/*
 * Checks that the given value "val" meets the constraints of spPCADimension, and if
 * they do it sets the fields in "config" such that when asked for the value of spPCADimension,
 * the value of "val" will be returned.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param val - the value to replace the current one in the config struct
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_INVALID_INTEGER - if the constraints on val are not met
 * - SP_CONFIG_SUCCESS - in case of success
 */
void spConfigSetPCADim(const SPConfig config, int val, SP_CONFIG_MSG* msg);

/*
 * Returns the pca file set in the configuration file, i.e the value
 * of spPcaFilename.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
char* spConfigGetPCAFile(const SPConfig config, SP_CONFIG_MSG* msg);

/*
 * Checks that the given value "val" meets the constraints of spPCAFilename, and if
 * they do it sets the fields in "config" such that when asked for the value of spPCAFilename,
 * the value of "val" will be returned.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param val - the value to replace the current one in the config struct
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
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

/*
 * Checks that the given value "val" meets the constraints of spNumOfFeatures, and if
 * they do it sets the fields in "config" such that when asked for the value of spNumOfFeatures,
 * the value of "val" will be returned.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param val - the value to replace the current one in the config struct
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_INVALID_INTEGER - if the constraints on val are not met
 * - SP_CONFIG_SUCCESS - in case of success
 */
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

/*
 * Checks that the given value "val" meets the constraints of spExtractionMode, and if
 * they do it sets the fields in "config" such that when asked for the value of spExtractionMode,
 * the value of "val" will be returned.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param val - the value to replace the current one in the config struct
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
void spConfigSetExtractionMode(const SPConfig config, bool val, SP_CONFIG_MSG* msg);

/*
 * Returns the number of similar images set in the configuration file, i.e the value
 * of spNumOfSimilarImg.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
int spConfigGetNumOfSimilarImages(const SPConfig config, SP_CONFIG_MSG* msg);

/*
 * Checks that the given value "val" meets the constraints of spNumOfSimilarImg, and if
 * they do it sets the fields in "config" such that when asked for the value of spNumOfSimilarImg,
 * the value of "val" will be returned.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param val - the value to replace the current one in the config struct
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_INVALID_INTEGER - if the constraints on val are not met
 * - SP_CONFIG_SUCCESS - in case of success
 */
void spConfigSetNumOfSimilarImages(const SPConfig config, int val, SP_CONFIG_MSG* msg);

/*
 * Returns the KDTree's split method set in the configuration file, i.e the value
 * of spKDTreeSplitMethod.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
SP_KDT_SPLIT spConfigGetSplitMethod(const SPConfig config, SP_CONFIG_MSG* msg);

/*
 * Checks that the given value "val" meets the constraints of spKDTreeSplitMethod, and if
 * they do it sets the fields in "config" such that when asked for the value of spKDTreeSplitMethod,
 * the value of "val" will be returned.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param val - the value to replace the current one in the config struct
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
void spConfigSetSplitMethod(const SPConfig config, SP_KDT_SPLIT val, SP_CONFIG_MSG* msg);

/*
 * Returns the number of similar features set in the configuration file, i.e the value
 * of spKNN.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
int spConfigGetNumOfSimilarFeatures(const SPConfig config, SP_CONFIG_MSG* msg);

/*
 * Checks that the given value "val" meets the constraints of spKNN, and if
 * they do it sets the fields in "config" such that when asked for the value of spKNN,
 * the value of "val" will be returned.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param val - the value to replace the current one in the config struct
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
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
 * - SP_CONFIG_INVALID_INTEGER - if the constraints on val are not met
 * - SP_CONFIG_SUCCESS - in case of success
 */
bool spConfigMinimalGui(const SPConfig config, SP_CONFIG_MSG* msg);

/*
 * Checks that the given value "val" meets the constraints of spMinimalGui, and if
 * they do it sets the fields in "config" such that when asked for the value of spMinimalGui,
 * the value of "val" will be returned.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param val - the value to replace the current one in the config struct
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
void spConfigSetMinimalGui(const SPConfig config, bool val, SP_CONFIG_MSG* msg);

/*
 * Returns an integer representing the logger's level that is set in the configuration file,
 * i.e the value of spLoggerLevel, such that "1" means "Error level", 2 means "Error/warning level", etc.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
int spConfigGetLogLevel(const SPConfig config, SP_CONFIG_MSG* msg);

/*
 * Checks that the given value "val" meets the constraints of spLoggerLevel, and if
 * they do it sets the fields in "config" such that when asked for the value of spLoggerLevel,
 * the value of "val" will be returned.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param val - the value to replace the current one in the config struct
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_INVALID_INTEGER - if the constraints on val are not met
 * - SP_CONFIG_SUCCESS - in case of success
 */
void spConfigSetLogLevel(const SPConfig config, int val, SP_CONFIG_MSG* msg);
/*
 * Returns the logger's output file name set in the configuration file, i.e the value
 * of spLoggerFilename.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
char* spConfigGetLogFile(const SPConfig config, SP_CONFIG_MSG* msg);

/*
 * Checks that the given value "val" meets the constraints of spLoggerFilename, and if
 * they do it sets the fields in "config" such that when asked for the value of spLoggerFilename,
 * the value of "val" will be returned.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param val - the value to replace the current one in the config struct
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
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

/**
 * The function returns wether the given string represents a valid integer in base 10.
 * 
 * @return
 * true if str represents a valid integer in base 10, false otherwise.
 */
bool isDecimalNumber(char* str);

/**
 * The function returns wether the given string represents a valid boolean value
 * 
 * @return
 * true if str is "true" or "false", false otherwise.
 */
bool isBooleanValue(char* str);

/**
 * Frees all memory resources associate with config. 
 * If config == NULL nothig is done.
 */
void spConfigDestroy(SPConfig config);

#endif /* SPCONFIG_H_ */
