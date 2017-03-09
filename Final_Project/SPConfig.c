#ifndef SPCONFIG_H_
#define SPCONFIG_H_

#include <stdbool.h>
#include <stdio.h>
#include "SPLogger.h"
#include "SPConfig.h"

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

typedef struct sp_config_t{
	char* imgDir;
	char* imgPre;
	char* imgSuf;
	int imgNum;
	int pcaDim;
	char* pcaFile;
	int featureNum;
	bool extractMode;
	int knnNumImg;
	KDTREETHING;
	int knnNumFeatures;
	bool useMinGUI;
	int logLvl;
	char* logFile
};


SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg)
{
	return NULL; // TODO finish
}


bool spConfigIsExtractionMode(const SPConfig config, SP_CONFIG_MSG* msg)
{
	*msg = SP_CONFIG_SUCCESS;
	return config->extractMode;
}


bool spConfigMinimalGui(const SPConfig config, SP_CONFIG_MSG* msg)
{
	*msg = SP_CONFIG_SUCCESS;
	return config->useMinGUI;
}


int spConfigGetNumOfImages(const SPConfig config, SP_CONFIG_MSG* msg)
{
	*msg = SP_CONFIG_SUCCESS;
	return config->imgNum;
}


int spConfigGetNumOfFeatures(const SPConfig config, SP_CONFIG_MSG* msg)
{
	*msg = SP_CONFIG_SUCCESS;
	return config->featureNum;
}


int spConfigGetPCADim(const SPConfig config, SP_CONFIG_MSG* msg)
{
	*msg = SP_CONFIG_SUCCESS;
	return config->pcaDim;
}

SP_CONFIG_MSG spConfigGetImagePath(char* imagePath, const SPConfig config,
		int index)
{
	return NULL; // TODO finish
}

SP_CONFIG_MSG spConfigGetPCAPath(char* pcaPath, const SPConfig config)
{
	return NULL; // TODO finish
}


void spConfigDestroy(SPConfig config)
{
	free(config);
}

#endif /* SPCONFIG_H_ */
