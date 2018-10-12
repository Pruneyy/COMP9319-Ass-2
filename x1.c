// test Bits dt

#include <stdio.h>
#include "defs.h"
#include "reln.h"
#include "tuple.h"
#include "bits.h"

int main(int argc, char **argv)
{
    // Create bitstring
    printf("New bitstring b of size 64:\n");
	Bits b = newBits(64);
	printf("b: "); 
	showBits(b); 
	printf("\n");
	
	// Set entire bitstring
	printf("Setting entire bitstring to 1:\n"); 
	setAllBits(b);
	printf("b: "); 
	showBits(b); 
	printf("\n");
	
	
	// Unset entire bitstring
	printf("Unsetting entire bitstring: \n");
	unsetAllBits(b);
	printf("b: "); 
	showBits(b); 
	printf("\n");
	
	// Set bits 7, 15, 23, 31, 39, 47, 55, 63
	printf("Setting bits every 8th bit: \n");
	setBit(b, 7);
	setBit(b, 15);
	setBit(b, 23);
	setBit(b, 31);
	setBit(b, 39);
	setBit(b, 47);
	setBit(b, 55);
	setBit(b, 63);
	printf("b: "); 
	showBits(b); 
	printf("\n");
	/*	
	setBit(b, 5);
	printf("t=1: "); showBits(b); printf("\n");
	setBit(b, 0);
	setBit(b, 40);
	setBit(b, 59);
	printf("t=2: "); showBits(b); printf("\n");
	if (bitIsSet(b,5)) printf("Bit 5 is set\n");
	if (bitIsSet(b,10)) printf("Bit 10 is set\n");
	setAllBits(b);
	printf("t=3: "); showBits(b); printf("\n");
	unsetBit(b, 40);
	printf("t=4: "); showBits(b); printf("\n");
	if (bitIsSet(b,20)) printf("Bit 20 is set\n");
	if (bitIsSet(b,40)) printf("Bit 40 is set\n");
	setBit(b, 59);
	*/
	return 0;
}
