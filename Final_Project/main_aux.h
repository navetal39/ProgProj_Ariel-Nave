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
#define MAX_QUERY_LENGTH 1024
#define SEP_RES ", "
#define SEP_LINE "\n"
#define TERM_SIGN "<>"
#define CMD_OPTION_C "-c"
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
SP_LOGGER_MSG initLog(SPConfig config)

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
 * Given an array of arrays of pointers to points, the length of said array, a boolean flag that says wether
 * the given array is an array of features arrays (1) or histograms (0) and the lengths of the feature arrays
 * (if the flag is 1), the function will destroy each of the points array using destroyPointsArray, then free
 * the "main" array itself
 * 
 * @param arraysArray - the array of arrays of pointers to the points to be destroyed
 * @param arraylength - the length of the array array
 * @param isFeaturesArray - a flag saying wether arraysArray contains features array (1) or histograms (0)
 * @param arraysLengths - if isFeaturesArray == 1, arraysLengths should contain the lengths of the features arrays.
 */
void destroyPointsArrayArray(SPPoint*** arraysArray, int arrayLength, int isFeaturesArrays, int* arraysLengths);