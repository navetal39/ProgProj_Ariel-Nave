#include <string.h>
#include <stdio.h>
#include "SPConfigUtil.h"

int spConfigUtilCountDigits(int num)
{
	int count = 0;
	do{
		num /= 10;
		count++;
	}while(num!=0);
	return count;
}

SP_CONFIG_UTIL_MSG spConfigUtilParseLine(char* line, char** var, char** val)
{
	char* temp[4];
	int tempLen;
	temp[0] = strtok(line, WHITESPACE_CHARS);
	temp[1] = strtok(NULL, WHITESPACE_CHARS);
	temp[2] = strtok(NULL, WHITESPACE_CHARS);
	temp[3] = strtok(NULL, WHITESPACE_CHARS);
	/* Empty/Comment line: */
	if(temp[0]==NULL || temp[0][0] == COMMENT_CHAR) {
		return SP_CONFIG_UTIL_EMPTY_LINE;
	}
	if(temp[0] != NULL && temp[1] == NULL) {
		*var = strtok(temp[0],  SET_CHAR_STR);
		*val = strtok(NULL, SET_CHAR_STR);
		if(*var == NULL || *val == NULL){
			return SP_CONFIG_UTIL_BAD_LINE;
		}
	}
	if(temp[1] != NULL && temp[2] == NULL) {
		tempLen = strlen(temp[0]);
		if(temp[0][tempLen-1] == SET_CHAR && temp[1][0] != SET_CHAR) {
			temp[0][tempLen-1] = NULL_CHAR;
		}
		if(temp[0][tempLen-1] != SET_CHAR && temp[1][0] == SET_CHAR) {
			temp[1]++;
		}
		if((temp[0][tempLen-1] == SET_CHAR && temp[1][0] == SET_CHAR)||
			(temp[0][tempLen-1] != SET_CHAR && temp[1][0] != SET_CHAR)) {
			return SP_CONFIG_UTIL_BAD_LINE;
		}
		*var = temp[0];
		*val = temp[1];
	}
	if(temp[2] != NULL && temp[3] == NULL) {
		if(strcmp(temp[1], SET_CHAR_STR)) {
			return SP_CONFIG_UTIL_BAD_LINE;
		}else{
			*var = temp[0];
			*val = temp[2];
		}
	}
	if(temp[3] != NULL) {
		return SP_CONFIG_UTIL_BAD_LINE;
	}
	return SP_CONFIG_UTIL_SUCCESS;
}

SP_CONFIG_UTIL_MSG spConfigUtilIsBadInt(char* var)
{
	return 0; /* TODO this */
}

void spConfigUtilPrintUnset(int unset)
{
	/* TODO this too */
}