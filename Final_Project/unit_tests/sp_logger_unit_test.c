#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "unit_test_util.h" //SUPPORTING MACROS ASSERT_TRUE/ASSERT_FALSE etc..
#include "../SPLogger.h"

/* Note: The supplied tests (and matching expected output files) have been modified in order to cover a wider range of possible
	behaviors. */

// This is a helper function which checks if two files are identical
static bool identicalFiles(const char* fname1, const char* fname2) {
	FILE *fp1, *fp2;
	fp1 = fopen(fname1, "r");
	fp2 = fopen(fname2, "r");
	char ch1 = EOF, ch2 = EOF;

	if (fp1 == NULL) {
		return false;
	} else if (fp2 == NULL) {
		fclose(fp1);
		return false;
	} else {
		ch1 = getc(fp1);
		ch2 = getc(fp2);

		while ((ch1 != EOF) && (ch2 != EOF) && (ch1 == ch2)) {
			ch1 = getc(fp1);
			ch2 = getc(fp2);
		}
		fclose(fp1);
		fclose(fp2);
	}
	if (ch1 == ch2) {
		return true;
	} else {
		return false;
	}
}

static bool basicLoggerTest() {
	
	//testing all prints for undefined logger...
	ASSERT_TRUE(spLoggerPrintError("A","sp_logger_unit_test.c",__func__,__LINE__) == SP_LOGGER_UNDIFINED);
	ASSERT_TRUE(spLoggerPrintWarning("A","sp_logger_unit_test.c",__func__,__LINE__) == SP_LOGGER_UNDIFINED);
	ASSERT_TRUE(spLoggerPrintInfo("A") == SP_LOGGER_UNDIFINED);
	ASSERT_TRUE(spLoggerPrintDebug("A","sp_logger_unit_test.c",__func__,__LINE__) == SP_LOGGER_UNDIFINED);

	//destroying
	spLoggerDestroy();
	return true;
}

static bool basicLoggerErrorTest() {
	const char* expectedFile = "unit_tests/expected_logs/basicLoggerErrorTestExp.log";
	const char* testFile = "unit_tests/output_logs/basicLoggerErrorTest.log";

	//testing for undefined logger...
	ASSERT_TRUE(spLoggerPrintError("MSG_1", "sp_logger_unit_test.c", __func__, __LINE__) == SP_LOGGER_UNDIFINED);

	//testins success in creating log...
	ASSERT_TRUE(spLoggerCreate(testFile, SP_LOGGER_ERROR_LEVEL) == SP_LOGGER_SUCCESS);

	//testing for success...
	ASSERT_TRUE(spLoggerPrintError("MSG_2", "sp_logger_unit_test.c", __func__, __LINE__) == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintWarning("MSG_3", "sp_logger_unit_test.c", __func__, __LINE__) == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintInfo("MSG_4") == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintDebug("MSG_5", "sp_logger_unit_test.c", __func__, __LINE__) == SP_LOGGER_SUCCESS);

	//testing for invalid argument if msg/file/function are NULL or line is negative...
	ASSERT_TRUE(spLoggerPrintError(NULL, "sp_logger_unit_test.c", __func__, __LINE__) == SP_LOGGER_INVAlID_ARGUMENT);
	ASSERT_TRUE(spLoggerPrintError("MSG_6", NULL, __func__, __LINE__) == SP_LOGGER_INVAlID_ARGUMENT);
	ASSERT_TRUE(spLoggerPrintError("MSG_7", "sp_logger_unit_test.c", NULL, __LINE__) == SP_LOGGER_INVAlID_ARGUMENT);
	ASSERT_TRUE(spLoggerPrintError("MSG_8", "sp_logger_unit_test.c", __func__, -1) == SP_LOGGER_INVAlID_ARGUMENT);

	//destroying...
	spLoggerDestroy();
	ASSERT_TRUE(identicalFiles(testFile, expectedFile));
	return true;
}

static bool basicLoggerWarningTest() {
	const char* expectedFile = "unit_tests/expected_logs/basicLoggerWarningTestExp.log";
	const char* testFile = "unit_tests/output_logs/basicLoggerWarningTest.log";

	//testing for undefined logger...
	ASSERT_TRUE(spLoggerPrintWarning("MSG_1", "sp_logger_unit_test.c", __func__, __LINE__) == SP_LOGGER_UNDIFINED);

	//testins success in creating log...
	ASSERT_TRUE(spLoggerCreate(testFile, SP_LOGGER_WARNING_ERROR_LEVEL) == SP_LOGGER_SUCCESS);

	//testing for success...
	ASSERT_TRUE(spLoggerPrintError("MSG_2", "sp_logger_unit_test.c", __func__, __LINE__) == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintWarning("MSG_3", "sp_logger_unit_test.c", __func__, __LINE__) == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintInfo("MSG_4") == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintDebug("MSG_5", "sp_logger_unit_test.c", __func__, __LINE__) == SP_LOGGER_SUCCESS);

	//testing for invalid argument if msg/file/function are NULL or line is negative...
	ASSERT_TRUE(spLoggerPrintWarning(NULL, "sp_logger_unit_test.c", __func__, __LINE__) == SP_LOGGER_INVAlID_ARGUMENT);
	ASSERT_TRUE(spLoggerPrintWarning("MSG_6", NULL, __func__, __LINE__) == SP_LOGGER_INVAlID_ARGUMENT);
	ASSERT_TRUE(spLoggerPrintWarning("MSG_7", "sp_logger_unit_test.c", NULL, __LINE__) == SP_LOGGER_INVAlID_ARGUMENT);
	ASSERT_TRUE(spLoggerPrintWarning("MSG_8", "sp_logger_unit_test.c", __func__, -1) == SP_LOGGER_INVAlID_ARGUMENT);

	//destroying...
	spLoggerDestroy();
	ASSERT_TRUE(identicalFiles(testFile, expectedFile));
	return true;
}

static bool basicLoggerInfoTest() {
	const char* expectedFile = "unit_tests/expected_logs/basicLoggerInfoTestExp.log";
	const char* testFile = "unit_tests/output_logs/basicLoggerInfoTest.log";

	//testing for undefined logger...
	ASSERT_TRUE(spLoggerPrintInfo("MSG_1") == SP_LOGGER_UNDIFINED);

	//testins success in creating log...
	ASSERT_TRUE(spLoggerCreate(testFile, SP_LOGGER_INFO_WARNING_ERROR_LEVEL) == SP_LOGGER_SUCCESS);

	//testing for invalid argument if msg is NULL...
	ASSERT_TRUE(spLoggerPrintInfo(NULL) == SP_LOGGER_INVAlID_ARGUMENT);

	//testing for success...
	ASSERT_TRUE(spLoggerPrintError("MSG_2", "sp_logger_unit_test.c", __func__, __LINE__) == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintWarning("MSG_3", "sp_logger_unit_test.c", __func__, __LINE__) == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintInfo("MSG_4") == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintDebug("MSG_5", "sp_logger_unit_test.c", __func__, __LINE__) == SP_LOGGER_SUCCESS);

	//destroying...
	spLoggerDestroy();
	ASSERT_TRUE(identicalFiles(testFile, expectedFile));
	return true;
}

static bool basicLoggerDebugTest() {
	const char* expectedFile = "unit_tests/expected_logs/basicLoggerDebugTestExp.log";
	const char* testFile = "unit_tests/output_logs/basicLoggerDebugTest.log";

	//testing for undefined logger...
	ASSERT_TRUE(spLoggerPrintDebug("MSG_1", "sp_logger_unit_test.c", __func__, __LINE__) == SP_LOGGER_UNDIFINED);

	//testins success in creating log...
	ASSERT_TRUE(spLoggerCreate(testFile, SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL) == SP_LOGGER_SUCCESS);

	//testing for success...
	ASSERT_TRUE(spLoggerPrintError("MSG_2", "sp_logger_unit_test.c", __func__, __LINE__) == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintWarning("MSG_3", "sp_logger_unit_test.c", __func__, __LINE__) == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintInfo("MSG_4") == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintDebug("MSG_5", "sp_logger_unit_test.c", __func__, __LINE__) == SP_LOGGER_SUCCESS);

	//testing for invalid argument if msg/file/function are NULL or line is negative...
	ASSERT_TRUE(spLoggerPrintDebug(NULL, "sp_logger_unit_test.c", __func__, __LINE__) == SP_LOGGER_INVAlID_ARGUMENT);
	ASSERT_TRUE(spLoggerPrintDebug("MSG_6", NULL, __func__, __LINE__) == SP_LOGGER_INVAlID_ARGUMENT);
	ASSERT_TRUE(spLoggerPrintDebug("MSG_7", "sp_logger_unit_test.c", NULL, __LINE__) == SP_LOGGER_INVAlID_ARGUMENT);
	ASSERT_TRUE(spLoggerPrintDebug("MSG_8", "sp_logger_unit_test.c", __func__, -1) == SP_LOGGER_INVAlID_ARGUMENT);

	//destroying...
	spLoggerDestroy();
	ASSERT_TRUE(identicalFiles(testFile, expectedFile));
	return true;
}


static bool basicLoggerPrintMsgTest() {
	const char* expectedFile = "unit_tests/expected_logs/basicLoggerMsgTestExp.log";
	const char* testFile = "unit_tests/output_logs/basicLoggerMsgTest.log";

	//testing for undefined logger...
	ASSERT_TRUE(spLoggerPrintMsg("MSG_1") == SP_LOGGER_UNDIFINED);

	//testins success in creating log...
	ASSERT_TRUE(spLoggerCreate(testFile, SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL) == SP_LOGGER_SUCCESS);

	//testing for invalid argument if msg is NULL...
	ASSERT_TRUE(spLoggerPrintMsg(NULL) == SP_LOGGER_INVAlID_ARGUMENT);

	//testing for success...
	ASSERT_TRUE(spLoggerPrintMsg("MSG_2") == SP_LOGGER_SUCCESS);

	//destroying...
	spLoggerDestroy();
	ASSERT_TRUE(identicalFiles(testFile, expectedFile));
	return true;
}
int main() {
	RUN_TEST(basicLoggerTest);
	RUN_TEST(basicLoggerErrorTest);
	RUN_TEST(basicLoggerWarningTest);
	RUN_TEST(basicLoggerInfoTest);
	RUN_TEST(basicLoggerDebugTest);
	RUN_TEST(basicLoggerPrintMsgTest);
	return 0;
}

