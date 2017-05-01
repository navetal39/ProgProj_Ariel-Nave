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
	KDTree tree;
	SPPoint **queryFeatures, **dbFeatures;
	int dbFeatCount, queryFeatCount, *nearestIndexes = NULL;
	char query[MAX_PATH_LENGTH+1];
	bool isFirstQuery, printSuccessful;
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
	if(logMsg != SP_LOGGER_DEFINED && logMsg != SP_LOGGER_SUCCESS){
		printf(ERR_LOG_OPEN);
		spConfigDestroy(config);
	}
	spLoggerPrintInfo(MSG_LOG_INIT);
	/* Initialize Image Processor */
	ImageProc imPr = ImageProc(config);
	spLoggerPrintInfo(MSG_LOG_INIT_IMPR);
	/* Initialize KDTree: */
	dbFeatures = getFeatures(config, &imPr, &dbFeatCount);
	tree = makeKDTree(config, dbFeatures, dbFeatCount);
	destroyPointsArray(dbFeatures, dbFeatCount);
	/* queries loop: */
	do{
		spLoggerPrintInfo(MSG_LOG_NEW_QUERY);
		queryFeatures=NULL;
		isFirstQuery=true;
		do{
			getQueryPath(query, isFirstQuery);
			if(SHOULD_RUN(query)){
				queryFeatures = imPr.getImageFeatures(query, QUERY_INDEX, &queryFeatCount);
			}
			isFirstQuery=false;
		}while(SHOULD_RUN(query) && !queryFeatures);
		if(SHOULD_RUN(query)){
			spLoggerPrintInfo(MSG_LOG_CALC_START);
			nearestIndexes = getNearestIndexes(config, queryFeatures, queryFeatCount, tree);
			spLoggerPrintInfo(MSG_LOG_CALC_END);
			printSuccessful = printNearestIndexes(config, &imPr, query, nearestIndexes);
			destroyPointsArray(queryFeatures, queryFeatCount);
			free(nearestIndexes);
			if(!printSuccessful){ break; }
		}
	}while(SHOULD_RUN(query));
	spLoggerPrintInfo(MSG_LOG_QUERIES_END);
	printf(MSG_EXIT);
	/* Clean up and exit: */
	spKDTreeDestroy(tree);
	spLoggerDestroy();
	spConfigDestroy(config);
	return RET_SUCCESS;
}