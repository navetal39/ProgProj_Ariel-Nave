#include "unit_test_util.h"
#include "../SPConfig.h"
#include <string.h>

#define TEST_PATH_DEFAULTS			"./unit_tests/test_configs/test_defaults.config"
#define TEST_PATH_LINE_FORMAT		"./unit_tests/test_configs/test_invalid_line_%d.config"
#define TEST_PATH_CONSTRAINTS_INT	"./unit_tests/test_configs/test_invalid_val_int.config"
#define TEST_PATH_CONSTRAINTS_SPT	"./unit_tests/test_configs/test_invalid_val_split.config"
#define TEST_PATH_CONSTRAINTS_STR	"./unit_tests/test_configs/test_invalid_val_str.config"
#define TEST_PATH_MISSING			"./unit_tests/test_configs/test_missing_%d.config"
#define TEST_PATH_GOOD				"./unit_tests/test_configs/test_valid.config"


/* Tests to see that all the module reacts accordingly when an illegal line
is present in the file. */
static bool test_line_format() {
	SP_CONFIG_MSG msg;
	char path[128] = { '\0' };
	int i;
	for(i=0;i<5;i++)
	{
		sprintf(path, TEST_PATH_LINE_FORMAT, i);
		ASSERT_TRUE(spConfigCreate(path, &msg) == NULL);
		ASSERT_TRUE(msg == SP_CONFIG_INVALID_STRING);
	}
	return true;
}
/* Tests to see that all the module reacts accordingly when a variable
in the file do not meet it's constraints */
static bool test_var_constraints() {
	SP_CONFIG_MSG msg;
	ASSERT_TRUE(spConfigCreate(TEST_PATH_CONSTRAINTS_INT, &msg) == NULL);
	ASSERT_TRUE(msg == SP_CONFIG_INVALID_INTEGER);
	ASSERT_TRUE(spConfigCreate(TEST_PATH_CONSTRAINTS_SPT, &msg) == NULL);
	ASSERT_TRUE(msg == SP_CONFIG_INVALID_STRING);
	ASSERT_TRUE(spConfigCreate(TEST_PATH_CONSTRAINTS_STR, &msg) == NULL);
	ASSERT_TRUE(msg == SP_CONFIG_INVALID_STRING);
	return true;
}
/* Tests to see that all the module reacts accordingly when one of the variables
without a default value is missing from the file */
static bool test_missing_values() {
	SP_CONFIG_MSG msg, cmpMsg;
	char path[128] = { '\0' };
	int i;
	for(i=0;i<4;i++)
	{
		sprintf(path, TEST_PATH_MISSING, i);
		ASSERT_TRUE(spConfigCreate(path, &msg) == NULL);
		switch(i)
		{
			case 0:
				cmpMsg = SP_CONFIG_MISSING_PREFIX;
				break;
			case 1:
				cmpMsg = SP_CONFIG_MISSING_SUFFIX;
				break;
			case 2:
				cmpMsg = SP_CONFIG_MISSING_NUM_IMAGES;
				break;
			case 3:
				cmpMsg = SP_CONFIG_MISSING_DIR;
				break;
			default:
				break;
		}
		ASSERT_TRUE(msg==cmpMsg);
	}
	return true;
}
/* Tests to see that all the config variables are updated correctly to their
default values when they are not present in the file. */
static bool test_default_values() {
	SP_CONFIG_MSG msg;
	SPConfig cfg;
	ASSERT_TRUE((cfg=spConfigCreate(TEST_PATH_DEFAULTS, &msg)) != NULL);

	ASSERT_TRUE(spConfigGetPCADim(cfg, &msg)==DEFAULT_PCA_DIM);
	ASSERT_TRUE(msg==SP_CONFIG_SUCCESS);

	ASSERT_TRUE(!(strcmp(spConfigGetPCAFile(cfg, &msg), DEFAULT_PCA_FILE)));
	ASSERT_TRUE(msg==SP_CONFIG_SUCCESS);

	ASSERT_TRUE(spConfigGetNumOfFeatures(cfg, &msg)==DEFAULT_FEATURES_NUM);
	ASSERT_TRUE(msg==SP_CONFIG_SUCCESS);

	ASSERT_TRUE(spConfigGetNumOfSimilarImages(cfg, &msg)==DEFAULT_SIM_IMG_NUM);
	ASSERT_TRUE(msg==SP_CONFIG_SUCCESS);

	ASSERT_TRUE(spConfigGetSplitMethod(cfg, &msg)==DEFAULT_SPLIT_METHOD);
	ASSERT_TRUE(msg==SP_CONFIG_SUCCESS);

	ASSERT_TRUE(spConfigGetNumOfSimilarFeatures(cfg, &msg)==DEFAULT_KNN);
	ASSERT_TRUE(msg==SP_CONFIG_SUCCESS);

	ASSERT_TRUE(spConfigGetLogLevel(cfg, &msg)==DEFAULT_LOG_LEVEL);
	ASSERT_TRUE(msg==SP_CONFIG_SUCCESS);

	ASSERT_TRUE(!(strcmp(spConfigGetLogFile(cfg, &msg), DEFAULT_LOG_FILE)));
	ASSERT_TRUE(msg==SP_CONFIG_SUCCESS);

	ASSERT_TRUE(spConfigIsExtractionMode(cfg, &msg)==DEFAULT_EXT_MODE);
	ASSERT_TRUE(msg==SP_CONFIG_SUCCESS);

	ASSERT_TRUE(spConfigMinimalGui(cfg, &msg)==DEFAULT_MIN_GUI);
	ASSERT_TRUE(msg==SP_CONFIG_SUCCESS);


	spConfigDestroy(cfg);
	return true;
}
/* Tests to see that all the config variables are updated correctly when 
they are present in the file */
static bool test_good_values() {
	SP_CONFIG_MSG msg;
	SPConfig cfg;
	cfg=spConfigCreate(TEST_PATH_GOOD, &msg);
	ASSERT_TRUE((cfg) != NULL);

	ASSERT_TRUE(!(strcmp(spConfigGetImgDir(cfg, &msg), "./dir/")));
	ASSERT_TRUE(msg==SP_CONFIG_SUCCESS);

	ASSERT_TRUE(!(strcmp(spConfigGetImgPrefix(cfg, &msg), "img")));
	ASSERT_TRUE(msg==SP_CONFIG_SUCCESS);

	ASSERT_TRUE(!(strcmp(spConfigGetImgSuffix(cfg, &msg), ".png")));
	ASSERT_TRUE(msg==SP_CONFIG_SUCCESS);

	ASSERT_TRUE(spConfigGetNumOfImages(cfg, &msg)==14);
	ASSERT_TRUE(msg==SP_CONFIG_SUCCESS);

	ASSERT_TRUE(spConfigGetPCADim(cfg, &msg)==19);
	ASSERT_TRUE(msg==SP_CONFIG_SUCCESS);

	ASSERT_TRUE(!(strcmp(spConfigGetPCAFile(cfg, &msg), "./PCA_file.pca")));
	ASSERT_TRUE(msg==SP_CONFIG_SUCCESS);

	ASSERT_TRUE(spConfigGetNumOfFeatures(cfg, &msg)==16);
	ASSERT_TRUE(msg==SP_CONFIG_SUCCESS);

	ASSERT_TRUE(spConfigGetNumOfSimilarImages(cfg, &msg)==7);
	spConfigGetSplitMethod(cfg, &msg);

	ASSERT_TRUE(spConfigGetNumOfSimilarFeatures(cfg, &msg)==17);
	ASSERT_TRUE(msg==SP_CONFIG_SUCCESS);

	ASSERT_TRUE(spConfigGetLogLevel(cfg, &msg)==3);
	ASSERT_TRUE(msg==SP_CONFIG_SUCCESS);

	ASSERT_TRUE(!(strcmp(spConfigGetLogFile(cfg, &msg), "/tmp/spcbir_log.txt")));
	ASSERT_TRUE(msg==SP_CONFIG_SUCCESS);

	ASSERT_TRUE(spConfigIsExtractionMode(cfg, &msg)==false);
	ASSERT_TRUE(msg==SP_CONFIG_SUCCESS);

	ASSERT_TRUE(spConfigMinimalGui(cfg, &msg)==true);
	ASSERT_TRUE(msg==SP_CONFIG_SUCCESS);


	spConfigDestroy(cfg);
	return true;
}
/* Tests to see that the module reacts as expected when given
invalid arguments */
static bool test_bad_args() {
	SP_CONFIG_MSG msg;
	ASSERT_TRUE(spConfigCreate(NULL, &msg) == NULL);
	ASSERT_TRUE(msg == SP_CONFIG_INVALID_ARGUMENT);
	ASSERT_TRUE(spConfigCreate(NULL, &msg) == NULL);
	return true;
}

int main() {
	RUN_TEST(test_line_format);
	RUN_TEST(test_var_constraints);
	RUN_TEST(test_missing_values);
	RUN_TEST(test_default_values);
	RUN_TEST(test_good_values);
	RUN_TEST(test_bad_args);
	return 0;
}