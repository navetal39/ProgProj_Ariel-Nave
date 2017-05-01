#include "SPLogger.h"

//File open mode
#define SP_LOGGER_OPEN_MODE "w"

// Global variable holding the logger
SPLogger logger = NULL;

struct sp_logger_t {
	FILE* outputChannel; //The logger file
	bool isStdOut; //Indicates if the logger is stdout
	SP_LOGGER_LEVEL level; //Indicates the level
};

SP_LOGGER_MSG spLoggerCreate(const char* filename, SP_LOGGER_LEVEL level) {
	if (logger != NULL) { //Already defined
		return SP_LOGGER_DEFINED;
	}
	logger = (SPLogger) malloc(sizeof(*logger));
	if (logger == NULL) { //Allocation failure
		return SP_LOGGER_OUT_OF_MEMORY;
	}
	logger->level = level; //Set the level of the logger
	if (filename == NULL) { //In case the filename is not set use stdout
		logger->outputChannel = stdout;
		logger->isStdOut = true;
	} else { //Otherwise open the file in write mode
		logger->outputChannel = fopen(filename, SP_LOGGER_OPEN_MODE);
		if (logger->outputChannel == NULL) { //Open failed
			free(logger);
			logger = NULL;
			return SP_LOGGER_CANNOT_OPEN_FILE;
		}
		logger->isStdOut = false;
	}
	return SP_LOGGER_SUCCESS;
}

void spLoggerDestroy() {
	if (!logger) {
		return;
	}
	if (!logger->isStdOut) {//Close file only if not stdout
		fclose(logger->outputChannel);
	}
	free(logger);//free allocation
	logger = NULL;
}

SP_LOGGER_MSG spLoggerPrintError(const char* msg, const char* file,
		const char* function, const int line)
{
	if(!logger) { return SP_LOGGER_UNDIFINED; }
	if(!msg || !file || !function || line < 0) { return SP_LOGGER_INVAlID_ARGUMENT; }
	FPRINTF_AND_CHECK(logger->outputChannel, LOG_MSG_TTL_F, LOG_MSG_TTL_ERR);
	FPRINTF_AND_CHECK(logger->outputChannel, LOG_MSG_FLD_FIL, file);
	FPRINTF_AND_CHECK(logger->outputChannel, LOG_MSG_FLD_FNC, function);
	FPRINTF_AND_CHECK(logger->outputChannel, LOG_MSG_FLD_LIN, line);
	FPRINTF_AND_CHECK(logger->outputChannel, LOG_MSG_FLD_MSG, msg);
	return SP_LOGGER_SUCCESS;
}

SP_LOGGER_MSG spLoggerPrintWarning(const char* msg, const char* file,
		const char* function, const int line)
{
	if(!logger) { return SP_LOGGER_UNDIFINED; }
	if(!msg || !file || !function || line < 0) { return SP_LOGGER_INVAlID_ARGUMENT; }
	if(logger->level == SP_LOGGER_ERROR_LEVEL)
	{
		return SP_LOGGER_SUCCESS; 
	}	
	FPRINTF_AND_CHECK(logger->outputChannel, LOG_MSG_TTL_F, LOG_MSG_TTL_WRN);
	FPRINTF_AND_CHECK(logger->outputChannel, LOG_MSG_FLD_FIL, file);
	FPRINTF_AND_CHECK(logger->outputChannel, LOG_MSG_FLD_FNC, function);
	FPRINTF_AND_CHECK(logger->outputChannel, LOG_MSG_FLD_LIN, line);
	FPRINTF_AND_CHECK(logger->outputChannel, LOG_MSG_FLD_MSG, msg);
	return SP_LOGGER_SUCCESS;
}

SP_LOGGER_MSG spLoggerPrintInfo(const char* msg)
{
	if(!logger) { return SP_LOGGER_UNDIFINED; }
	if(!msg) { return SP_LOGGER_INVAlID_ARGUMENT; }
	if(logger->level == SP_LOGGER_ERROR_LEVEL ||
		logger->level == SP_LOGGER_WARNING_ERROR_LEVEL)
	{
		return SP_LOGGER_SUCCESS; 
	}	
	FPRINTF_AND_CHECK(logger->outputChannel, LOG_MSG_TTL_F, LOG_MSG_TTL_INF);
	FPRINTF_AND_CHECK(logger->outputChannel, LOG_MSG_FLD_MSG, msg);
	return SP_LOGGER_SUCCESS;
}

SP_LOGGER_MSG spLoggerPrintDebug(const char* msg, const char* file,
		const char* function, const int line)
{
	if(!logger) { return SP_LOGGER_UNDIFINED; }
	if(!msg || !file || !function || line < 0) { return SP_LOGGER_INVAlID_ARGUMENT; }
	if(logger->level != SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL)
	{
		return SP_LOGGER_SUCCESS;
	}	
	FPRINTF_AND_CHECK(logger->outputChannel, LOG_MSG_TTL_F, LOG_MSG_TTL_DBG);
	FPRINTF_AND_CHECK(logger->outputChannel, LOG_MSG_FLD_FIL, file);
	FPRINTF_AND_CHECK(logger->outputChannel, LOG_MSG_FLD_FNC, function);
	FPRINTF_AND_CHECK(logger->outputChannel, LOG_MSG_FLD_LIN, line);
	FPRINTF_AND_CHECK(logger->outputChannel, LOG_MSG_FLD_MSG, msg);
	return SP_LOGGER_SUCCESS;
}

SP_LOGGER_MSG spLoggerPrintMsg(const char* msg)
{
	if(!logger) { return SP_LOGGER_UNDIFINED; }
	if(!msg) { return SP_LOGGER_INVAlID_ARGUMENT; }
	FPRINTF_AND_CHECK(logger->outputChannel, "%s\n", msg);
	return SP_LOGGER_SUCCESS;
}