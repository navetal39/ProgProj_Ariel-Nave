#include "main_aux.h"

using namespace sp;

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
	if(!config){
		spLoggerPrintError(MSG_LOG_AUX_CFG_NULL, __FILE__, __func__, __LINE__);
		return NULL;
	}
	iNum = spConfigGetNumOfImages(config, &configMsg);
	eMode = spConfigIsExtractionMode(config, &configMsg);
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
		for(j=0; j<lengths[i]; j++){
			dbFeatsMerged[c] = spPointCopy(dbFeats[i][j]);
			c++;
		}
	}
	destroyPointsArrayArray(dbFeats, iNum, lengths);
	free(lengths);
	return dbFeatsMerged;
}
SPPoint*** extractDatabaseFeaturesI(SPConfig config, ImageProc* imPr, int dbFeatsLens[])
{
	int i, iNum;
	char imgPath[MAX_PATH_LENGTH+1];
	SP_CONFIG_MSG configMsg;
	SPPoint*** dbFeats;
	if(!config){
		spLoggerPrintError(MSG_LOG_AUX_CFG_NULL, __FILE__, __func__, __LINE__);
		return NULL;
	}
	iNum = spConfigGetNumOfImages(config, &configMsg);
	if(!(dbFeats=(SPPoint***)malloc(iNum*sizeof(SPPoint**))))
	{
		spLoggerPrintError(MSG_LOG_AUX_ALLOC_ERR, __FILE__, __func__, __LINE__);
		return NULL;
	}
	for(i=0; i<iNum; i++)
	{
		memset(imgPath, NULL_CHAR, MAX_PATH_LENGTH+1);
		configMsg = spConfigGetImagePath(imgPath, config, i);
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
	if(!config){
		spLoggerPrintError(MSG_LOG_AUX_CFG_NULL, __FILE__, __func__, __LINE__);
		return NULL;
	}
	iNum = spConfigGetNumOfImages(config, &configMsg);
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
	char filePath[MAX_PATH_LENGTH+1],
		cplxErrMsg[MAX_ERR_MSG_LENGTH+MAX_PATH_LENGTH];
	FILE* file;
	if(!config){
		spLoggerPrintError(MSG_LOG_AUX_CFG_NULL, __FILE__, __func__, __LINE__);
		return NULL;
	}
	spConfigGetFeatsFilePath(filePath, config, index);

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
	if(!config){
		spLoggerPrintError(MSG_LOG_AUX_CFG_NULL, __FILE__, __func__, __LINE__);
		return false;
	}
	iNum = spConfigGetNumOfImages(config, &configMsg);
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
	char filePath[MAX_PATH_LENGTH+1],
		cplxErrMsg[MAX_ERR_MSG_LENGTH+MAX_PATH_LENGTH];
	FILE* file;
	if(!config){
		spLoggerPrintError(MSG_LOG_AUX_CFG_NULL, __FILE__, __func__, __LINE__);
		return false;
	}

	dim = spPointGetDimension(feats[0]);
	toWrite[0]=length; toWrite[1]=dim;
	index = spPointGetIndex(feats[0]);

	spConfigGetFeatsFilePath(filePath, config, index);

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

KDTree makeKDTree(SPConfig config, SPPoint** features, int len)
{
	SP_CONFIG_MSG configMsg;
	SP_KDT_SPLIT meth;
	if(!config){
		spLoggerPrintError(MSG_LOG_AUX_CFG_NULL, __FILE__, __func__, __LINE__);
		return NULL;
	}
	meth = spConfigGetSplitMethod(config, &configMsg);
	return spKDTreeCreate(features, len, meth);
}

void getQueryPath(char* queryPath, bool isFirstQuery)
{
	printf((isFirstQuery)?INST_QUERY:INST_QUERY_REDO);
	/* Anything after strlen should already be 0: */
	memset(queryPath, NULL_CHAR, strlen(queryPath));
	READ_STR(queryPath);
}

int* getNearestIndexes(SPConfig config, SPPoint** queryFeatures, int len, KDTree tree)
{
	SP_CONFIG_MSG configMsg;
	int *ret, k, nearImgNum, iNum, *nearestImagesFeat, *nifTemp, *imgScores, i;
	if(!config){
		spLoggerPrintError(MSG_LOG_AUX_CFG_NULL, __FILE__, __func__, __LINE__);
		return NULL;
	}
	iNum = spConfigGetNumOfImages(config, &configMsg);
	k = spConfigGetNumOfSimilarFeatures(config, &configMsg);
	nearImgNum = spConfigGetNumOfSimilarImages(config, &configMsg);
	if(!(nearestImagesFeat = (int*)malloc((k+1)*sizeof(int)))){
		return NULL;
	}
	if(!(imgScores = (int*)malloc(iNum*sizeof(int)))){
		free(nearestImagesFeat);
		return NULL;
	}
	memset(imgScores, 0, iNum*sizeof(int));
	for(i=0;i<len;i++){
		if(spKDTreeKNN(tree, queryFeatures[i], k, nearestImagesFeat)<0){
			free(nearestImagesFeat);
			free(imgScores);
			return NULL;
		}
		nifTemp=nearestImagesFeat;
		while(!(*nifTemp==-1)){
			imgScores[*(nifTemp++)]++;
		}
	}
	ret =  getHighestIndexes(imgScores, iNum, nearImgNum);
	free(nearestImagesFeat);
	free(imgScores);
	return ret;
}

int* getHighestIndexes(int* buff, int len, int k)
{
	int i, *ret;
	SPBPQueue* queue;
	SP_BPQUEUE_MSG queueMsg;
	BPQueueElement tempElement;
	if(!(ret=(int*)malloc((k+1)*sizeof(int)))){
		spLoggerPrintError(MSG_LOG_AUX_ALLOC_ERR, __FILE__, __func__, __LINE__);
		return NULL;
	}
	if(!(queue = spBPQueueCreate(k))){
		/* No need to check for invalid argument return messages  thanks to this test*/
		free(ret);
		return NULL;
	}
	for(i=0;i<len;i++)
	{
		spBPQueueEnqueue(queue, i, 1.0/((double)(buff[i]+1)));
	}
	for(i=0;i<k;i++)
	{
		queueMsg = spBPQueuePeek(queue,&tempElement);
		if(queueMsg==SP_BPQUEUE_EMPTY) { break; }
		ret[i] = tempElement.index;
		spBPQueueDequeue(queue); /* No need to check if empty since we already checked it */
	}
	ret[i]=-1;
	spBPQueueDestroy(queue);
	return ret;
}

bool printNearestIndexes(SPConfig config, ImageProc* imPr, char* qPath, int* indexes)
{
	SP_CONFIG_MSG configMsg;
	char bestPath[MAX_PATH_LENGTH+1];
	if(!config){
		spLoggerPrintError(MSG_LOG_AUX_CFG_NULL, __FILE__, __func__, __LINE__);
		return false;
	}
	bool useMinGui = spConfigMinimalGui(config, &configMsg);
	if(!useMinGui)
	{
		printf(OUT_TITLE, qPath);
	}
	while(*indexes>=0)
	{
		memset(bestPath, NULL_CHAR, MAX_PATH_LENGTH+1);
		configMsg = spConfigGetImagePath(bestPath, config, *indexes);
		if(useMinGui)
		{
		imPr->showImage(bestPath);
		}else{
			printf("%s\n", bestPath);
		}
		indexes++;
	}
	return true;
}