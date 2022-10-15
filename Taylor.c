#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

#define fourPI 0x6488
#define twoPI 0x3244
#define PI 0x1923
#define shift_amount 11
#define shift_mask ((1 << shift_amount) - 1)  //shift_mask = 0x07FF
#define shift_amount_mult 22
#define shift_mask_mult ((1 << shift_amount_mult) - 1)

short short_sin(short x);
float fixed2float(short a);
short multiply(short a, short b);
short divide_by_int(short a, int b);

int main() {

	short x;				//initialize input variable	
	FILE *fp = fopen("sine.csv", "w");	//and output files

	if (fp == NULL) {			//Error checking
		printf("Error opening file");	//
		exit(1);			//
	}	

	/* --- Output short_sin(x) and sin(x) from -4pi to 4pi and print results to sine.txt --- */
	for (x = (-4*PI); x <= (4*PI); x += 0x0010){				
		/* --- Convert and output 16 bit fixed point numbers to floats for printing to file --- */
		fprintf(fp, "%3.3f, %3.3f, %3.3f\n", fixed2float(x), fixed2float(short_sin(x)), sin(fixed2float(x)));
		
	}

	fclose(fp);				//close file
	
	return 0;
}

short multiply(short a, short b) {

	float decimal_result = 0;

	int multiply_result = a * b / 2048;	//Need to remove factor of 2048 when multiplying	
	return multiply_result;
}

short divide_by_int(short a, int b){

	short div_result = a / b;
	return div_result;

}

float fixed2float(short num){
	float result = 0;		//Initialize return float value
	bool neg = false;		//and sign flag
	
	if (num & 0x8000) {		//If most significant bit is 1
		neg = true;		//then value is known to be negative.
		num ^= 0xFFFF;		//If negative, sign flag is set and
		num += 0x0001;		//apply two's compliment.
		
	}
	/* --- Separate whole number and decimal, convert to float, then add them in preparation for printing --- */
	result = ((float)(num >> shift_amount)) + ((float)(num & shift_mask) / 2048);

	/* --- if negative return result * -1 --- */
	return (neg) ? -1*result : result;
}

short short_sin(short x) {
	int i;								//iterator
	short coef[] = {0, 0x0800, 0, 0xF800, 0, 0x0800, 0, 0xF800};	//{0,1,0,-1,...} sin coefficients
	short xcoef[] = {0x0800, 0, 0, 0, 0, 0, 0, 0};			//used to create array of (x^n)/n!
	short ans[8];							//output array, good for checking results
	short a = 0, out = 0;
	
	/* --- x is outside of range -3*PI/2 to 3*PI/2 get it in range --- */
	if (x >= (3*PI/2) || x <= (-3*PI/2)) {
		if (x >= (3*PI/2)) {
			while (x >= (3*PI/2)) x -= twoPI;
		}
		else if (x <= (-3*PI/2)) {
			while (x <= (-3*PI/2)) x += twoPI;
		}
	}

	/* --- Use symmetry if x is inside previous range but outside of -PI/2 to PI/2 ---*/
	if ((x >= (PI/2)) && (x <= (3*PI/2))) {
		x -= PI;
		x *= -1;
	}	
	if ((x <= (-PI/2)) && (x >= (-3*PI/2))) {
		x += PI;
		x *= -1;
	}
	
	/* --- Use Taylor series to approximate value between -PI/2 to PI/2 --- */
	for (i = 1; i < 8; i++) {
		xcoef[i] = multiply(xcoef[i-1], divide_by_int(x, i));	//xcoef(i-1)*(x/i)
		ans[i] = multiply(xcoef[i], coef[i]);			//multiply by sine coefs
		out += ans[i];						//sum the ans array for approximation
		}
	return out;
	}
	


