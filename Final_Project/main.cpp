#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "main_aux.h"
/*
Files with unresolved TODO's:
 - main.cpp
 - main_aux.cpp
 - main_aux.h
 - SPConfig.c
*/
using namespace sp;

int main(int argc, char* argv[])
{
	SPConfig config;
	SP_LOGGER_MSG logMsg;
	SPPoint **queryFeatures;
	int queryFeatCount, *nearestIndexes = NULL;
	char query[MAX_PATH_LENGTH+1];
	bool isFirstQuery;
	ImageProc *imPr;
	/* Initialize Config: */
	if((argc != 1) && (argc != 3 || strcmp(argv[1],CMD_OPTION_C))){
		printf(ERR_CMD_ARGS);
		return RET_FAIL;
	}
	if(!(config = initConfig((argc>1)?argv[2]:DEF_CFG_FILE))){
		return RET_FAIL;
	}
	/* Initialize Logger: */
	logMsg = initLog(config);
	if(logMsg != SP_LOGGER_DEFINED && logMsg != SP_LOGGER_SUCCESS)
	{
		/* TODO: print error? */
		spConfigDestroy(config);
	}
	spLoggerPrintInfo(MSG_LOG_INIT);
	/* Initialize Image Processor */
	imPr = new ImageProc(config);
	spLoggerPrintInfo(MSG_LOG_INIT_IMPR);
	/* Extract features from database (images/files) and store if needed: */
	/* TODO: extract features using existing method and store them in Ariel's
	KD-Tree, when he finally finishes it... */
	/* queries loop: */
	do{
		spLoggerPrintInfo(MSG_LOG_NEW_QUERY);
		queryFeatures=NULL;
		isFirstQuery=true;
		do{
			getQueryPath(query, isFirstQuery);
			if(SHOULD_RUN(query))
			{
				queryFeatures = imPr->getImageFeatures(query, QUERY_INDEX, &queryFeatCount);
			}
			isFirstQuery=false;
		}while(SHOULD_RUN(query) && !queryFeatures);
		if(SHOULD_RUN(query))
		{
			spLoggerPrintInfo(MSG_LOG_CALC_START);
			/* TODO: get closest images indexes into nearestIndexes */
			spLoggerPrintInfo(MSG_LOG_CALC_END);
			printNearestIndexes(config, imPr, query, nearestIndexes);
			destroyPointsArray(queryFeatures, queryFeatCount);
			free(nearestIndexes);
		}
	}while(SHOULD_RUN(query));
	spLoggerPrintInfo(MSG_LOG_QUERIES_END);
	printf(MSG_EXIT);
	/* Clean up and exit: */
	/* TODO: destroy the database data structure */
	spLoggerDestroy();
	spConfigDestroy(config);
	return RET_SUCCESS;
}