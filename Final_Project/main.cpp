#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "main_aux.h"
#include "SPImageProc.h"
/*
Files with unresolved TODO's:
 - main.cpp
 - main_aux.cpp
 - main_aux.h
 - SPConfig.c
*/
int main(int argc, char* argv[])
{
	SPConfig config;
	SP_LOGGER_MSG logMsg;
	SPPoint **queryFeatures;
	int queryFeatCount, *nearestIndexes;
	char *cfgFileName, query[MAX_QUERY_LENGTH+1];
	/* Initialize Config: */
	if((argc != 1) && (argc != 3 || strcmp(argv[1],CMD_OPTION_C))){
		printf(ERR_CMD_ARGS); return RET_FAIL;
	}
	if(!(config = initConfig((cfgFileName = (argc>1)?argv[2]:DEF_CFG_FILE)))){
		return RET_FAIL;
	}
	/* Initialize logger: */
	logMsg = initLog(config);
	if(logMsg != SP_LOGGER_DEFINED && logMsg != SP_LOGGER_SUCCESS)
	{
		/* TODO: print error? */
		spConfigDestroy(config);
	}
	/* Extract features from database (images/files) and store if needed: */
	/* TODO: extract features and store them in a database data structure */
	/* queries loop: */
	do{
		do{
			getQueryPath(query);
			if(SHOULD_RUN(query))
			{
				queryFeatures = getImageFeatures(query, QUERY_INDEX, &queryFeatCount);
			}
		}while(SHOULD_RUN(query) && !queryFeatures);
		if(SHOULD_RUN(query))
		{
			/* TODO: get closest images indexes into nearestIndexes */
			printNearestIndexes(config, nearestIndexes);
			destroyPointsArray(queryFeatures);
			free(nearestIndexes);
		}
	}while(SHOULD_RUN(query));
	/* Clean up and exit: */
	/* TODO: destroy the database data structure */
	spLoggerDestroy();
	spConfigDestroy(config);
	return RET_SUCCESS;
}