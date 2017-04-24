/* TODO add headers for extract mode stuff */


#include <cstring>
extern "C" {
	#include "SPPoint.h"
	#include "SPConfig.h"
}
/* Error messages and instructions */
#define ERR_CMD_ARGS "Invalid command line: use -c <config_filename>\n"
#define ERR_CFG_OPEN "The%sconfiguration file %s couldn't be open\n"
#define ERR_CFG_OPEN_D " default "
#define ERR_CFG_OPEN_C " "
#define INST_QUERY "Please enter an image path:\n"
#define OUT_TITLE "Best candidates for - %s - are:\n"
/* Defaults */
#define DEF_CFG_FILE "spcbir.config"
/* Constants */
#define RET_SUCCESS 0
#define RET_FAIL -1
#define QUERY_INDEX 0
#define MAX_PATH_LENGTH 1024
#define SEP_LINE "\n"
#define TERM_SIGN "<>"
#define CMD_OPTION_C "-c"
#define FEATS_FILE_SUFFIX ".feats"
/* Macros */
#define SHOULD_RUN(qp) strcmp((qp), TERM_SIGN)
#define ERASE_QUEUE(queue) do{\
		while(!spBPQueueIsEmpty((queue))){\
			spBPQueueDequeue((queue));\
		}\
		spBPQueueDestroy((queue));\
	}while(0);
#define READ_STR(buff) scanf("%s", (buff));
/* Structs */
typedef struct sp_img_sift_score{
	int index;
	int score;
}siftScore;

/**
 * A function to be used for sorting image scores in order to fins closest images using local features.
 * 
 * @param x - a pointer to a siftScore struct
 * @param y - a poitner to a siftScore struct
 * 
 * returns the difference between y's score and x's score
 */
int scoreComp(const void* x, const void* y);

/**
 * Given a path to a configuration file, this method will initialize a config struct from the data
 * read from said file.
 * if the configuration file couldn't be opened, the function prints an error message. Additionally,
 * error messages may be printed from inside the diffrent spConfig methods used to initialize the struct.
 * 
 * @param cfgPath - a string representation of the path to the config file
 * 
 * return
 * An initialized SPConfig struct with all the system configuration variables set according to the data in the
 * given file, or NULL if an error occured.
 */
SPConfig initConfig(char* cfgPath);

/**
 * Given a valid configuration SPConfig struct, the method will attempt to initialize the logger using the
 * information found inside the SPConfig struct and return the SP_LOGGER_MSG the initialization yielded.
 * 
 * @param config - a valid SPConfig object
 * 
 * return
 * a SP_LOGGER_MSG obtained from spLoggerCreate during the logger initialization process
 */
SP_LOGGER_MSG initLog(SPConfig config);

/**
 * Given a valid configuration SPConfig struct and a pointer to an integer, the method will extract all features of
 * all the images in the images folder (the extraction method, num. of images, folder name and naming format is all
 * given by the config file) and return an array containing them. The length of this array is stored inside the int
 * pointed at by the 2nd argument.
 * 
 * @param config - a valid SPConfig object
 * @param totalLen - a pointer to an integer that'll store the length of the returned array
 * 
 * return
 * NULL if an error occured or an array of SPPoint*s that represent the extracted features, with the point's indexes
 * matching the image they have been extracted from.
 */
SPPoint** getFeatures(SPConfig config, int* totalLen);

/**
 * Given a valid configuration SPConfig struct and an array of integers, the method will extract all features of
 * all the images (ACTUAL IMAGES) in the images folder (num. of images, folder name and naming format is all
 * given by the config file) and return an array of arrays containing them, such that each array in the returned array
 * holds all the features of the image of the same index. The lengths of each of the "child arrays" are stored in the
 * matching indexes in the array that is the 2nd argument
 * 
 * @param config - a valid SPConfig object
 * @param totalLen - an array of integers that'll store the lengths of the returned arrays
 * 
 * return
 * NULL if an error occured or an array of arrays of SPPoint*s that represent the extracted features, with the point's indexes
 * matching the image they have been extracted from, and each array is stored in that same index in the big array.
 */
SPPoint*** extractDatabaseFeaturesI(SPConfig config, int dbFeatsLens[]);

/**
 * Given a valid configuration SPConfig struct and an array of integers, the method will extract all features of
 * all the images (FROM .feats FILES) in the images folder (num. of images, folder name and naming format is all
 * given by the config file) and return an array of arrays containing them, such that each array in the returned array
 * holds all the features of the image of the same index. The lengths of each of the "child arrays" are stored in the
 * matching indexes in the array that is the 2nd argument
 * 
 * @param config - a valid SPConfig object
 * @param lengths - an array of integers that'll store the lengths of the returned arrays
 * 
 * return
 * NULL if an error occured or an array of arrays of SPPoint*s that represent the extracted features, with the point's indexes
 * matching the image they have been extracted from, and each array is stored in that same index in the big array.
 */
SPPoint*** extractDatabaseFeaturesF(SPConfig config, int lengths[]);

/**
 * Given a valid configuration SPConfig struct, an index and a pointer to an integer, the method will extract all features
 * the image who's index is the 2nd argument (FROM .feats FILE) (folder name and naming format is all * given by the config
 * file) and return an array containing them. The length of this array is stored at the integer pointed at by the 3rd argument.
 * 
 * @param config - a valid SPConfig object
 * @param index - the indesx of the image who's features are to be extracted.
 * @param length - a pointer to an integer that'll contain the length of the returned array.
 * 
 * return
 * NULL if an error occured or an arrays of SPPoint*s that represent the extracted features, with the point's indexes
 * matching the image they have been extracted from.
 */
SPPoint** getImageFeaturesF(SPConfig config, int index, int* length);

/**
 * Given a FILE* (opened in write mode), an integer dim and an index the method will attempt to extract, recreate and return a feature of size dim and
 * index index from the current reading position at the file, moving said position accordingly.
 * USE ONLY A CORRECT FILE* (matching a file of features, positioned at the beginning of a feature), NOTHING IS GURRANTEED OTHERWISE!
 * 
 * @param file - a correctly positioned, read enabled FILE* of a features file.
 * @param dim - the dimension of the feature to be extracted.
 * @param index - the index of the points that'll represent the extracted feature.
 * 
 * return
 * NULL if an error occured, or a pointer to SPPoint representing the extracted feature.
 */
SPPoint* getImageFeatureF(FILE* file, int dim, int index)

/**
 * Given a valid configuration SPConfig struct, an array of arrays of features and an array of integers, the method will
 * store the all the features, such that for each array in the main array all of it's points are stored in thir own file, matching their
 * indexes. The 3rd arguments holds the lengths of the respective array.
 * The method assumes all features are of the same dimension and in each sub-array also of the same index.
 * The path to the image directory, at which the features will be stored, as well as the naming convensions are given by the config file.
 * 
 * @param config - a valid SPConfig object.
 * @param feats - an array of arrays of features.
 * @param lengths - an array of integers that stores the lengths of the arrays in feats.
 * 
 * return
 * false if an error occured, true otherwise.
 */
bool storeDatabaseFeaturesF(SPConfig config, SPPoint* feats[][], int lengths[]);

/**
 * Given a valid configuration SPConfig struct, an arrays of features and an integer, the method will * store the all the featuresin one 
 * file, the name of which matches their indexes. The 3rd arguments holds the length of the array.
 * The method assumes all features are of the same dimension index.
 * The path to the image directory, at which the features will be stored, as well as the naming convensions are given by the config file.
 * 
 * @param config - a valid SPConfig object.
 * @param feats - an array of features.
 * @param lengths - an integer that stores the length of feats.
 * 
 * return
 * false if an error occured, true otherwise.
 */
bool storeImageFeaturesF(SPConfig config, SPPoint* feats[], int length);

/**
 * Given a FILE* and a feature (opened in read mode), the method will write the information of the feature into the file at it's current
 * position in such a way that getImageFeatureF will be able to extract it later.
 * 
 * @param file - a correctly positioned, write enabled FILE* of a features file.
 * @param feature - the feature to be stored.
 * 
 * return
 * false if an error occured, true otherwise.
 */
bool storeImageFeatureF(FILE* file, SPPoint* feature);

/**
 * Given a buffer of characters, the function prints instructions to the user, asking him/her to enter a path to
 * a query image, and then reads it's input into said buffer.
 * 
 * @param queryPath - a buffer to store the user's input.
 */
void getQueryPath(char* queryPath);

/**
 * Given a valid configuration SPConfig struct and an array of non-negative integers terminated with -1, the method
 * will display the images who's indexes are in the integer array, or print their paths, in the order at which they are
 * listed in the array. The output type depends on the information inside the SPConfig object.
 * 
 * @param config - a valid SPConfig object
 * @param indexes[] - an array of integers, all of which's members are non-negative except for the last one, which is -1.
 */
void printNearestIndexes(SPConfig config, int* indexes);

/**
 * Destroys all the points pointed at by a pointer in the array of point pointers "pointArray",
 * then frees pointArray
 * 
 * Given an array of pointers to points and the length of said array, the function will destroy
 * all the points pointed at by a pointer in the points pointer array using spPointDestroy()
 * and then free the array itself.
 * 
 * @param pointArray - the array of pointers to the points to be destroyed
 * @param arraylength - the length of the point pointers array
 */
void destroyPointsArray(SPPoint** pointArray, int arrayLength);

/******************************/
/* OLD STUFF - MAY BE REMOVED */
/******************************/

/**
 * For each array pointed to by a member of arraysArray, this function destroys all points pointed to by
 * a pointer in said array, then frees it. Finally, it frees arraysArray itself.
 * 
 * Given an array of arrays of pointers to points, the length of said array and the lengths of the feature arrays
 * (if the flag is 1), the function will destroy each of the points array using destroyPointsArray, then free
 * the "main" array itself
 * 
 * @param arraysArray - the array of arrays of pointers to the points to be destroyed
 * @param arraylength - the length of the array array
 * @param arraysLengths - if isFeaturesArray == 1, arraysLengths should contain the lengths of the features arrays.
 */
void destroyPointsArrayArray(SPPoint*** arraysArray, int arrayLength, int* arraysLengths);