#include "MathIsFun.h"
#include <stdio.h>

int main()
{
	char input;
	int in1, in2, in3;
	int booleanRet;
	printf("Welcome to Math Is Fun - beta version\nSupported operations are:\n1 - Power Calculation\n2 - Prime Check\n3 - Palindrome Check\nPlease enter operation number (1/2/3): \n");
	input = getchar();
	switch(input)
	{
		case '1':
			printf("Please enter three space separated numbers: \n");
			scanf("%d %d %d", &in1, &in2, &in3);
			printf("res = %d\n", funPow(in1, in2, in3));
			break;
		case '2': case '3':
			printf("Please enter an integer: \n");
			scanf("%d", &in1);
			if(input == '2')
			{
				booleanRet = funPrimeCheck(in1);
			}else{
				booleanRet = funPalindromeCheck(in1);
			}
			if(booleanRet)
			{
				printf("res = true\n");
			}else{
				printf("res = false\n");
			}
			break;
		default:
			printf("invalid input\n");
	}
}