#include <msp430.h> 
#include"IQmathLib.h"
/*
 * main.c
 */
int main(void) {
	float x, y, z1, z2;
	_iq28 xx, yy, zz; // floating point
	x = 1.0;    y = 7.0;    z1 = x/y;        // floating point division
	// fixed point
	xx = _IQ28(1.0);   // convert floating point to fixed point
	yy = _IQ28(7.0);   // convert floating point to fixed point
	zz = _IQ28div(xx, yy); // fixed point division
	z2 = _IQ28toF(zz);  // convert fixed point to floating point
	while(1);
}
