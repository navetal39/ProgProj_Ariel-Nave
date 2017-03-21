#include <stdbool.h>
#include <stdbool.h>
#include <ctype.h> // for isdigit
#include "auxil.h"

bool isNum(char* buf)
{
	bool ret = true;
	int i, len = strlen(buf);
	for(i = 0; i<=len; i++)
	{
		if(!isdigit(buf[i]))
		{
			ret = false;
			break;
		}
	}
	return ret;
}