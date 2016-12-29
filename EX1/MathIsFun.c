#include "MathIsFun.h"
/*
 * Calculates the largest integer less or equal than the square root of x.
 * funSqrt(10) = 3
 * funSqrt(16) = 4
 * @param x - An integer for which the function applies
 * @return the value of |_sqrt(x)_|, if x is negative then the function
 * returns -1.
 */
int funSqrt(int x); // ==> Declaration is always in the beginning of the file.

int funPow(int x, int n, int d)
{
	int temp=1, half;
	if(n==0)			// x^0(mod d) = 1 for every x,d
		return 1;
	if(x<0)
		x=((x%d)+d)%d; 	// if x<0 then x%d is between -(d-1) and 0, adding d puts it between 1 and d, and finally applying %d to it all puts it between 0 and d-1, all while not changing the value modulu d of x
	if(n%2)
		temp = x;
	half=funPow(x, n/2, d);
	return half*half*temp%d;
}

int funSqrt(int x)
{
	int r = 1, d = 1;
	if(x<0)
		return -1;
	if(x==0)
		return 0;
	while(d<x)			//find the largest power of 2 lower than x in O(log(x)) time
		d = d*2;
	d/=2;
	while(1)
	{
		if(r*r<=x && x <(r+1)*(r+1))
			break;
		if(r*r<x)		// if we got here and entered the if block it means that both r^2 and (r+1)^2 are less than x...
		{
			r += d;
		}else{			// ... otherwise both are larger than x
			r -= d;
		}
		d /= 2;
	}
	return r;
}

bool funPrimeCheck(int x)
{
	if(x<=1)			// the lowest prime is 2, thus we don't need to check inputs which are less that 2
		return 0;
	int root = funSqrt(x), dev;
	for(dev = 2; dev <= root; dev++)
		if(!(x%dev))
			return 0;
	return 1;
}

bool funPalindromeCheck(int x)
{
	int d=1;
	if(x<0)
		return 0;
	while(d<=x)		// finds the largest power of 10 lower than x in O(log(x)) time
		d*=10;
	d/=10;
	while(1)
	{
		if(x <= 9)	// if x is 1 digit long
			return 1;
		if(x/d != x%10) // check if first and last digits are identical and return 0 (flase) if not
			return 0;
		x = (x%d)/10;	// remove first and last digits from x
		d/=100;			// update d to have the same number of characters as x
	}
	return 1;
}