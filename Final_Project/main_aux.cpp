#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "main_aux.h"
extern "C" {
	#include "SPBPriorityQueue.h"
}

using namespace sp;
int scoreComp(const void* x, const void* y)
{
	return (((siftScore*)y)->score-((siftScore*)x)->score);
}

SPConfig initConfig(const char* cfgPath)
{
	SP_CONFIG_MSG configMsg;
	SPConfig cfg;
	cfg = spConfigCreate(cfgPath, &configMsg);
	if(configMsg == SP_CONFIG_CANNOT_OPEN_FILE)
	{
		printf(ERR_CFG_OPEN, (!strcmp(cfgPath, DEF_CFG_FILE))
			?ERR_CFG_OPEN_D
			:ERR_CFG_OPEN_C, cfgPath);
	}
	return cfg;
}
SP_LOGGER_MSG initLog(SPConfig config)
{
	int lvlInt;
	char* filePath;
	SP_CONFIG_MSG configMsg;
	SP_LOGGER_LEVEL lvl;
	lvlInt = spConfigGetLogLevel(config, &configMsg);
	filePath = spConfigGetLogFile(config, &configMsg);
	if(!(strcmp(filePath, DEFAULT_LOG_FILE))){
		filePath = NULL;
	}
	switch(lvlInt)
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

SPPoint** getFeatures(SPConfig config, ImageProc* imPr, int* totalLen)
{	
	int iNum, *lengths, i, j, c;
	SP_CONFIG_MSG configMsg;
	SPPoint*** dbFeats;
	SPPoint** dbFeatsMerged = NULL;
	bool eMode;
	iNum = spConfigGetNumOfImages(config, &configMsg);
	/* TODO check message? */
	eMode = spConfigIsExtractionMode(config, &configMsg);
	/* TODO check message? */
	if(!(lengths=(int*)malloc(iNum*sizeof(int)))){
		spLoggerPrintError(MSG_LOG_AUX_ALLOC_ERR, __FILE__, __func__, __LINE__);
		return NULL;
	}
	if(eMode){
		if(!(dbFeats = extractDatabaseFeaturesI(config, imPr, lengths))){
			free(lengths);
			return NULL;
		}
		if(!(storeDatabaseFeaturesF(config, dbFeats, lengths))){
			free(lengths);
			destroyPointsArrayArray(dbFeats, iNum, lengths);
			return NULL;
		}
	}else{
		if(!(dbFeats = extractDatabaseFeaturesF(config, lengths))){
			free(lengths);
			return NULL;
		}
	}
	*totalLen=0;
	for(i=0;i<iNum; i++){
		*totalLen+=lengths[i];
	}
	if(!(dbFeatsMerged=(SPPoint**)malloc(*totalLen*sizeof(SPPoint*)))){
		spLoggerPrintError(MSG_LOG_AUX_ALLOC_ERR, __FILE__, __func__, __LINE__);
		free(lengths);
		destroyPointsArrayArray(dbFeats, iNum, lengths);
		return NULL;
	}
	c=0;
	for(i=0; i<iNum; i++){
		for(j=0; i<lengths[i]; j++){
			dbFeatsMerged[c] = dbFeats[i][j];
		}
		c++;
	}
	free(lengths);
	destroyPointsArrayArray(dbFeats, iNum, lengths);
	return dbFeatsMerged;
}
SPPoint*** extractDatabaseFeaturesI(SPConfig config, ImageProc* imPr, int dbFeatsLens[])
{
	int i, iNum;
	char imgPath[MAX_PATH_LENGTH+1];
	SP_CONFIG_MSG configMsg;
	SPPoint*** dbFeats;
	iNum = spConfigGetNumOfImages(config, &configMsg);
	/* TODO check message? */
	if(!(dbFeats=(SPPoint***)malloc(iNum*sizeof(SPPoint**))))
	{
		spLoggerPrintError(MSG_LOG_AUX_ALLOC_ERR, __FILE__, __func__, __LINE__);
		return NULL;
	}
	for(i=0; i<iNum; i++)
	{
		memset(imgPath, NULL_CHAR, MAX_PATH_LENGTH+1);
		configMsg = spConfigGetImagePath(imgPath, config, i);
		/* TODO check message? */
		if(!(dbFeats[i] = imPr->getImageFeatures(imgPath, i, dbFeatsLens+i)))
		{
			destroyPointsArrayArray(dbFeats, i, dbFeatsLens);
			return NULL;
		}
	}
	return dbFeats;
}
SPPoint*** extractDatabaseFeaturesF(SPConfig config, int lengths[])
{
	SPPoint*** ret;
	int iNum, i;
	SP_CONFIG_MSG configMsg;
	iNum = spConfigGetNumOfImages(config, &configMsg);
	/* TODO check message? */
	if(!(ret=(SPPoint***)malloc(iNum*sizeof(SPPoint**)))){
		spLoggerPrintError(MSG_LOG_AUX_ALLOC_ERR, __FILE__, __func__, __LINE__);
		return NULL;
	}
	for(i=0;i<iNum;i++)
	{
		if(!(ret[i]=getImageFeaturesF(config, i, lengths+i))){
			destroyPointsArrayArray(ret, i, lengths);
			return NULL;
		}
	}
	return ret;
}
SPPoint** getImageFeaturesF(SPConfig config, int index, int* length)
{
	SPPoint** ret;
	int i, toRead[2], dim;
	char filePath[MAX_PATH_LENGTH+1], *sufNew, *sufBackup,
		cplxErrMsg[MAX_ERR_MSG_LENGTH+MAX_PATH_LENGTH];
	SP_CONFIG_MSG configMsg;
	FILE* file;

	sufNew = (char*)malloc(strlen(FEATS_FILE_SUFFIX)*sizeof(char)+1);
	strcpy(sufNew, FEATS_FILE_SUFFIX);
	sufBackup = spConfigGetImgSuffix(config, &configMsg);
	/* TODO check message? */
	spConfigSetImgSuffix(config, sufNew, &configMsg);
	/* TODO check message? */
	spConfigGetImagePath(filePath, config, index);
	spConfigSetImgSuffix(config, sufBackup, &configMsg);
	/* TODO check message? */
	free(sufNew);

	if(!(file=(fopen(filePath, READ_MODE)))){
		sprintf(cplxErrMsg, MSG_LOG_AUX_OPEN_ERR, filePath);
		spLoggerPrintError(cplxErrMsg, __FILE__, __func__, __LINE__);
		return NULL;
	}
	if(fread((void*)toRead, sizeof(int), 2, file)<2){
		sprintf(cplxErrMsg, MSG_LOG_AUX_READ_FEAT_ERR, filePath);
		spLoggerPrintError(cplxErrMsg, __FILE__, __func__, __LINE__);
		fclose(file);
		return NULL;
	}
	*length = toRead[0]; dim = toRead[1];
	if(!(ret = (SPPoint**)malloc(*length*sizeof(SPPoint*)))){
		spLoggerPrintError(MSG_LOG_AUX_ALLOC_ERR, __FILE__, __func__, __LINE__);
		fclose(file);
		return NULL;
	}
	for(i=0;i<*length;i++)
	{
		if(!(ret[i]=(getImageFeatureF(file, filePath, dim, index)))){
			destroyPointsArray(ret, i);
			fclose(file);
			return NULL;
		}
	}
	fclose(file);
	return ret;
}
SPPoint* getImageFeatureF(FILE* file, char* filePath, int dim, int index)
{
	SPPoint* ret;
	double *tempArr;
	char cplxErrMsg[MAX_ERR_MSG_LENGTH+MAX_PATH_LENGTH];
	if(!(tempArr = (double*)malloc(dim*sizeof(double))))
	{
		spLoggerPrintError(MSG_LOG_AUX_ALLOC_ERR, __FILE__, __func__, __LINE__);
		return NULL;
	}
	if(fread((void*)tempArr, sizeof(double),dim,file)<(size_t)dim){
		sprintf(cplxErrMsg, MSG_LOG_AUX_READ_FEAT_ERR, filePath);
		spLoggerPrintError(cplxErrMsg, __FILE__, __func__, __LINE__);
		free(tempArr);
		return NULL;
	}
	ret = spPointCreate(tempArr, dim, index);
	free(tempArr);
	return ret;
}
bool storeDatabaseFeaturesF(SPConfig config, SPPoint** feats[], int lengths[])
{
	int iNum, i;
	SP_CONFIG_MSG configMsg;
	iNum = spConfigGetNumOfImages(config, &configMsg);
	/* TODO check message? */
	for(i=0;i<iNum;i++)
	{
		if(!storeImageFeaturesF(config, feats[i], lengths[i])){
			return false;
		}
	}
	return true;
}
bool storeImageFeaturesF(SPConfig config, SPPoint* feats[], int length)
{
	int i, index, toWrite[2], dim;
	char filePath[MAX_PATH_LENGTH+1], *sufNew, *sufBackup, cplxErrMsg[MAX_ERR_MSG_LENGTH+MAX_PATH_LENGTH];;
	SP_CONFIG_MSG configMsg;
	FILE* file;

	dim = spPointGetDimension(feats[0]);
	toWrite[0]=length; toWrite[1]=dim;
	index = spPointGetIndex(feats[0]);

	sufNew = (char*)malloc(strlen(FEATS_FILE_SUFFIX)*sizeof(char)+1);
	strcpy(sufNew, FEATS_FILE_SUFFIX);
	sufBackup = spConfigGetImgSuffix(config, &configMsg);
	/* TODO check message? */
	spConfigSetImgSuffix(config, sufNew, &configMsg);
	/* TODO check message? */
	spConfigGetImagePath(filePath, config, index);
	spConfigSetImgSuffix(config, sufBackup, &configMsg);
	/* TODO check message? */
	free(sufNew);

	if(!(file=(fopen(filePath, WRITE_MODE)))){
		sprintf(cplxErrMsg, MSG_LOG_AUX_OPEN_ERR, filePath);
		spLoggerPrintError(cplxErrMsg, __FILE__, __func__, __LINE__);
		return false;
	}
	if(fwrite((void*)toWrite, sizeof(int), 2, file)<2){
		sprintf(cplxErrMsg, MSG_LOG_AUX_WRITE_FEAT_ERR, filePath);
		spLoggerPrintError(cplxErrMsg, __FILE__, __func__, __LINE__);
		fclose(file);
		return false;
	}
	for(i=0;i<length;i++)
	{
		if(!(storeImageFeatureF(file, filePath, feats[i]))){
			fclose(file);
			return false;
		}
	}
	fclose(file);
	return true;
}
bool storeImageFeatureF(FILE* file, char* filePath, SPPoint* feature)
{
	int i, dim;
	double *tempArr;
	char cplxErrMsg[MAX_ERR_MSG_LENGTH+MAX_PATH_LENGTH];
	dim = spPointGetDimension(feature);
	if(!(tempArr = (double*)malloc(dim*sizeof(double)))){
		spLoggerPrintError(MSG_LOG_AUX_ALLOC_ERR, __FILE__, __func__, __LINE__);
		return false;
	}
	for(i=0;i<dim;i++)
	{
		tempArr[i] = spPointGetAxisCoor(feature, i);
	}
	if(fwrite((void*)tempArr, sizeof(double), dim, file)<(size_t)dim){
		sprintf(cplxErrMsg, MSG_LOG_AUX_WRITE_FEAT_ERR, filePath);
		spLoggerPrintError(cplxErrMsg, __FILE__, __func__, __LINE__);
		free(tempArr);
		return false;
	}
	free(tempArr);
	return true;
}

void getQueryPath(char* queryPath, bool isFirstQuery)
{
	printf((isFirstQuery)?INST_QUERY:INST_QUERY_REDO);
	/* Anything after strlen should already be 0: */
	memset(queryPath, NULL_CHAR, strlen(queryPath));
	READ_STR(queryPath);
}

void printNearestIndexes(SPConfig config, ImageProc* imPr, char* qPath, int* indexes)
{
	SP_CONFIG_MSG configMsg;
	char bestPath[MAX_PATH_LENGTH+1];
	bool useMinGui = spConfigMinimalGui(config, &configMsg);
	/* TODO check message? */
	if(!useMinGui)
	{
		printf(OUT_TITLE, qPath);
	}
	while(*indexes>=0)
	{
		memset(bestPath, NULL_CHAR, MAX_PATH_LENGTH+1);
		configMsg = spConfigGetImagePath(bestPath, config, *indexes);
		/* TODO check message? */
		if(useMinGui)
		{
		imPr->showImage(bestPath);
		}else{
			printf("%s\n", bestPath);
		}
		indexes++;
	}
}