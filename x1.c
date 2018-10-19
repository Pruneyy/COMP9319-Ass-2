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
	
	printf("Put b in page, then retrieve in b2\n");
	Page p = newPage();
	putBits(p, 0, b);
	Bits b2 = newBits(64);
	getBits(p, 0, b2);
	printf("b2: "); 
	showBits(b2); 
	printf("\n");

	printf("test Bitwise or\n");
	printf("Original b:");
	showBits(b);
	unsetAllBits(b2);
	printf("Setting some bits in b2: \n");
	setBit(b2, 2);
	setBit(b2, 3);
	setBit(b2, 4);
	setBit(b2, 7);
	setBit(b2, 8);
	setBit(b2, 13);
	setBit(b2, 14);
	setBit(b2, 20);
	showBits(b2); 
	printf("\n");
	orBits(b,b2);
	printf("orBits(b,b2) b: "); 
	showBits(b); 
	printf("\n");
	return 0;
}
