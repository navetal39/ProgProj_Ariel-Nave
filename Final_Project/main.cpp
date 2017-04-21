#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "main_aux.h"

int main(int argc, char* argv[])
{
	SPConfig config;
	char *cfgFileName, query[MAX_QUERY_LENGTH+1];
	/* Initialize Config: */
	if((argc != 1) && (argc != 3 || strcmp(argv[1],CMD_OPTION_C))){
		printf(ERR_CMD_ARGS); return RET_FAIL;
	}
	if(!(config = initConfig((cfgFileName = (argc>1)?argv[2]:DEF_CFG_FILE)))){
		return RET_FAIL;
	}
	/* Initialize logger: */
	/* Extract features from images/files: */
	/* queries loop: */
	do{
		getQueryPath(query);
		if(SHOULD_RUN(query))
		{
			/* The meat of the program (Tofu for Ariel) */
		}
	}while(SHOULD_RUN(query));
	/* Clean up and exit: */
	spConfigDestroy(config);
	return RET_SUCCESS;
}