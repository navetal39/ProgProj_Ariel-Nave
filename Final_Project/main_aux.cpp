#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "main_aux.h"
extern "C" {
	#include "spConfig.h"
	#include "SPBPriorityQueue.h"
}

int scoreComp(const void* x, const void* y)
{
	return (((siftScore*)y)->score-((siftScore*)x)->score);
}

SPConfig initConfig(char* cfgPath)
{
	SP_CONFIG_MSG configMsg;
	SPConfig cfg;
	cfg = spConfigCreate(cfgPath, &configMsg);
	if(configMsg == SP_CONFIG_CANNOT_OPEN_FILE)
	{
		printf(ERR_CFG_OPEN, (argc>1)?ERR_CFG_OPEN_D:ERR_CFG_OPEN_C, cfgFileName);
	}
	return cfg;
}
SP_LOGGER_MSG initLog(SPConfig config)
{
	int lvlInt;
	char* filePath;
	SP_CONFIG_MSG configMsg;
	SP_LOGGER_LEVEL lvl = NULL;
	lvlInt = spConfigGetLogLevel(config, &configMsg);
	filePath = spConfigGetLogFile(config, &configMsg);
	switch lvlInt
	{
		case 1:
			lvl = SP_LOGGER_ERROR_LEVEL;
			break;
		case 2:
			lvl = SP_LOGGER_WARNING_ERROR_LEVEL;
			break;
		case 3:
			lvl = SP_LOGGER_INFO_WARNING_ERROR_LEVEL;
			break;
		case 4:
			lvl = SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL;
			break;
		default:
			/* Unreachable, only here to make the code prettier */
			break;
	}
	return spLoggerCreate(filePath, lvl);

}

SPPoint** getFeatures(SPConfig config, int* toatalFeatsNum)
{	
	int iNum, *lengths, i, j, c;
	SP_CONFIG_MSG configMsg;
	SPPoint*** dbFeats;
	SPPoint** dbFeatsMerged;
	bool eMode;
	inum = spConfigGetNumOfImages(config, &configMsg);
	/* TODO check message? */
	emode = spConfigIsExtractionMode(config, &configMsg);
	/* TODO check message? */
	if(eMode)
	{
		if(!(dbFeats = extractDatabaseFeaturesI(config, &lengths)))
		{
			/* TODO handle error */
		}
		if(!(storeDatabaseFeaturesF(config, dbFeats, lengths)))
		{
			/* TODO handle error */
		}
	}else{
		if(!(dbFeats = extractDatabaseFeaturesF(config)))
		{
			/* TODO handle error */
		}
	}
	*totalLen=0;
	for(i=0;i<iNum; i++)
	{
		*totalLen+=lengths[i];
	}
	for(i=0; i<iNum; i++)
	{
		for(j=0; i<lengths[i]; j++)
		{
			dbFeatsMerged[c] = dbFeats[i][j];
		}
		free(dbFeats[i]);
		c++;
	}
	free(lengths);
	return dbFeatsMerged
}
SPPoint*** extractDatabaseFeaturesI(SPConfig config, int** dbFeatsLens)
{
	int i, iNum, j;
	char imgPath[MAX_PATH_LENGTH+1];
	SP_CONFIG_MSG configMsg;
	SPPoint*** dbFeats;
	inum = spConfigGetNumOfImages(config, &configMsg);
	/* TODO check message? */
	if!((dbFeats=(SPPoint***)malloc(iNum*sizeof(SPPoint**))))
	{
		return NULL;
	}
	if!((*dbFeatsLens=(int*)malloc(iNum*sizeof(int))))
	{
		free(dbFeats);
		return NULL;
	}
	for(i=0; i<iNum; i++)
	{
		memset(imgPath, NULL_BYTE, MAX_PATH_LENGTH+1);
		configMsg = spConfigGetImagePath(imgPath, config, i);
		/* TODO check message? */
		if(!(dbFeats[i] = getImageFeatures(imgPath, i, *dbFeatsLens+i)))
		{
			for(j=0;j<i;j++)
			{
				destroyPointsArray(dbFeats[j]);
			}
			free(dbFeats);
			free(*dbFeatsLens);
			*dbFeatsLens = NULL;
			return NULL;
		}
	}
	return dbFeats;
}
SPPoint*** extractDatabaseFeaturesF(SPConfig config)
{
	int i, iNum, j, k, len, dim;
	double *curPointVals;
	char imgPath[MAX_PATH_LENGTH+1], *suffixBackUp, header[MAX_FEATS_HEADER_LEN];
	SP_CONFIG_MSG configMsg;
	FILE* featsFile;
	SPPoint ***dbFeats, **curFilePoints;
	inum = spConfigGetNumOfImages(config, &configMsg);
	/* TODO check message? */
	suffixBackUp = spConfigGetImgSuffix(config, &configMsg);
	/* TODO check message? */
	spConfigSetImgSuffix(config, FEATS_FILE_SUFFIX, &configMsg);
	/* TODO check message? */
	if(!(dbFeats = (SPPoint***)malloc(iNum*sizeof(SPPoint**)))){
		/* TODO handle error */
	}
	for(i=0; i<iNum; i++){
		memset(imgPath, NULL_BYTE, MAX_PATH_LENGTH+1);
		configMsg = spConfigGetImagePath(imgPath, config, i);
		/* TODO check message? */
		if(!(featsFile = fopen(filename, READ_MODE))){
			/* TODO: print error? */
			spConfigSetImgSuffix(config, suffixBackUp, &configMsg);
			/* TODO check message? */
			return;
		}
		memset(header, NULL_BYTE, MAX_FEATS_HEADER_LEN);
		if(fscanf(featsFile, FEATS_FILE_HEAD_T, &len, &dim)<=0){
			/* TODO handle error */
		}
		if(!(curFilePoints = (SPPoint**)malloc(len*sizeof(SPPoint*)))){
			/* TODO handle error */
		}
		for(j=0;j<len;j++){
			if(!(*curPointVals = (int*)malloc(dim*sizeof(double)))){
				/* TODO handle error */
			}
			for(k=0; k<dim;k++){
				if(fscanf(featsFile, FEATS_FILE_COOR_T, curPointVals+k)<=0){
					/* TODO handle error */
				}
			}
			if(!(curFilePoints[j] = spPointCreate(curPointVals, dim, i))){
				/* TODO handle error */
			}
			free(curPointVals);
		}
		dbFeats[i] = curFilePoints;
		fclose(filename);
	}
	spConfigSetImgSuffix(config, suffixBackUp, &configMsg);
	/* TODO check message? */
	return;
}
bool storeDatabaseFeaturesF(SPConfig config, SPPoint*** feats, int* lengths)
{
	int i, iNum, j, k, dim;
	char imgPath[MAX_PATH_LENGTH+1], *suffixBackUp;
	SP_CONFIG_MSG configMsg;
	FILE* featsFile;
	inum = spConfigGetNumOfImages(config, &configMsg);
	/* TODO check message? */
	suffixBackUp = spConfigGetImgSuffix(config, &configMsg);
	/* TODO check message? */
	spConfigSetImgSuffix(config, FEATS_FILE_SUFFIX, &configMsg);
	/* TODO check message? */
	for(i=0; i<iNum; i++){
		memset(imgPath, NULL_BYTE, MAX_PATH_LENGTH+1);
		configMsg = spConfigGetImagePath(imgPath, config, i);
		/* TODO check message? */
		if(!(featsFile = fopen(filename, WRITE_MODE))){
			/* TODO: print error? */
			spConfigSetImgSuffix(config, suffixBackUp, &configMsg);
			/* TODO check message? */
			return;
		}
		dim = spPointGetDimension(feats[i][0]);
		if(fprintf(FEATS_FILE_HEAD_T, lengths[i], dim)<0){
			/* TODO handle error */
		}
		for(j=0;j<lengths[i];j++){
			for(k=0; k<dim;k++){
				if(fprintf(FEATS_FILE_COOR_T, spPointGetAxisCoor(feats[i][j],k))<0){
					/* TODO handle error */
				}
			}
			if(fprintf(SEP_LINE)<0){
				/* TODO handle error */
			}
		}
		fclose(filename);
	}
	spConfigSetImgSuffix(config, suffixBackUp, &configMsg);
	/* TODO check message? */
	return;
}

void getQueryPath(char* queryPath)
{
	printf(INST_QUERY_OR_TERM);
	/* Anything after strlen should already be 0: */
	memset(queryPath, NULL_CHAR, strlen(queryPath));
	READ_STR(queryPath);
}

void printNearestIndexes(SPConfig config, int* indexes)
{
	SP_CONFIG_MSG configMsg;
	char bestPath[MAX_PATH_LENGTH+1];
	bool useMinGui = spConfigMinimalGui(config, &configMsg);
	/* TODO check message? */
	if(!useMinGui)
	{
		printf(OUT_TITLE);
	}
	while(*indexes>=0)
	{
		memset(bestPath, NULL_BYTE, MAX_PATH_LENGTH+1);
		configMsg = spConfigGetImagePath(bestPath, config, *indexes);
		/* TODO check message? */
		if(useMinGui)
		{
			showImage(bestPath);
		}else{
			printf("%s\n", bestPath);
		}
		indexes++;
	}
}

void destroyPointsArray(SPPoint** pointArray, int arrayLength)
{
	int i;
	for(i = 0; i < arrayLength; ++i)
	{
		spPointDestroy(pointArray[i]);
	}
	free(pointArray);
}

void destroyPointsArrayArray(SPPoint*** arraysArray, int arrayLength,
							int isFeaturesArrays, int* varyingLengths)
{
	int i, len;
	for(i = 0; i<arrayLength; i++)
	{
		len = (isFeaturesArrays)?varyingLengths[i]:COLOR_NUM;
		destroyPointsArray(arraysArray[i], len);
	}
	free(arraysArray);
}