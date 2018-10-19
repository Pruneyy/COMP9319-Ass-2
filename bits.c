// bits.c ... functions on bit-strings
// part of SIMC signature files
// Bit-strings are arbitrarily long byte arrays
// Least significant bits (LSB) are in array[0]
// Most significant bits (MSB) are in array[nbytes-1]

// Written by John Shepherd, September 2018

#include <assert.h>
#include "defs.h"
#include "bits.h"
#include "page.h"

typedef struct _BitsRep {
	Count  nbits;		  // how many bits
	Count  nbytes;		  // how many bytes in array
	Byte   bitstring[1];  // array of bytes to hold bits
	                      // actual array size is nbytes
} BitsRep;

// create a new Bits object

Bits newBits(int nbits)
{
	Count nbytes = iceil(nbits,8);
	Bits new = malloc(2*sizeof(Count) + nbytes);
	new->nbits = nbits;
	new->nbytes = nbytes;
	memset(&(new->bitstring[0]), 0, nbytes);
	return new;
}

// release memory associated with a Bits object

void freeBits(Bits b) //TODO
{
    free(b);
}

// check if the bit at position is 1

Bool bitIsSet(Bits b, int position)//TODO
{
	assert(b != NULL);
	assert(0 <= position && position < b->nbits);
	if (b->bitstring[position/8] & (1 << (position%8))) {
	   return TRUE;
    } else {
       return FALSE;
    }
}

// check whether one Bits b1 is a subset of Bits b2
// B1 is a subset of B2 if ALL elements of B1 are also in B2
Bool isSubset(Bits b1, Bits b2)//TODO
{
    int i = 0;
	assert(b1 != NULL && b2 != NULL);
	assert(b1->nbytes == b2->nbytes);
	for (i = 0; i < b1->nbits; i++) {
	    // If B1 bit is active && the same B2 bit is NOT active return false, else continue
	    if (bitIsSet(b1, i) && !bitIsSet(b2, i)){
            return FALSE;
        }
    }
    return TRUE;
}

// set the bit at position to 1

void setBit(Bits b, int position)//TODO
{
	assert(b != NULL);
	assert(0 <= position && position < b->nbits);
	b->bitstring[position/8] |= (1 << (position%8));
}

// set all bits to 1

void setAllBits(Bits b)//TODO
{
    int i = 0;
	assert(b != NULL);
    for (i = 0; i < b->nbits; i++) {
        setBit(b, i);
    }
}

// set the bit at position to 0

void unsetBit(Bits b, int position)//TODO
{
	assert(b != NULL);
	assert(0 <= position && position < b->nbits);
	b->bitstring[position/8] &= ~(1 << (position%8));
}

// set all bits to 0

void unsetAllBits(Bits b)//TODO
{
    int i = 0;
	assert(b != NULL);
    for (i = 0; i < b->nbits; i++) {
        unsetBit(b, i);
    }
}

// bitwise AND ... b1 = b1 & b2

void andBits(Bits b1, Bits b2)//TODO
{
    int i = 0;
	assert(b1 != NULL && b2 != NULL);
	assert(b1->nbytes == b2->nbytes);
    for (i = 0; i < b1->nbits; i++) {
        if (bitIsSet(b1, i) && bitIsSet(b2, i)) {
            b1->bitstring[i/8] |= (1 << (i%8));
        } else {
            b1->bitstring[i/8] &= ~(1 << (i%8));
        }
    }
}

// bitwise OR ... b1 = b1 | b2

void orBits(Bits b1, Bits b2)//TODO
{
    int i = 0;
	assert(b1 != NULL && b2 != NULL);
	assert(b1->nbytes == b2->nbytes);
	for (i = 0; i < b1->nbits; i++) {
	    if ((bitIsSet(b1, i) || bitIsSet(b2, i))) {
	        b1->bitstring[i/8] |= (1 << (i%8));
        }
    }
}


// get a bit-string (of length b->nbytes)
// from specified position in Page buffer
// and place it in a BitsRep structure

void getBits(Page p, Offset pos, Bits b) //TODO
{
   Byte* start = addrInPage(p, pos, b->nbytes);
   int i;
   for (i = 0; i < b->nbytes; i++) {
      memcpy(&(b->bitstring[i]),&(start[i]),sizeof(Byte));
   }
}

// copy the bit-string array in a BitsRep
// structure to specified position in Page buffer

void putBits(Page p, Offset pos, Bits b) //TODO
{
	Byte* start = addrInPage(p, pos, b->nbytes);
   int i;
   for (i = 0; i < b->nbytes; i++) {
      memcpy(&(start[i]),&(b->bitstring[i]),sizeof(Byte));
   }
}

// show Bits on stdout
// display in order MSB to LSB
// do not append '\n'

void showBits(Bits b)
{
	assert(b != NULL);
    //printf("(%d,%d)",b->nbits,b->nbytes);
	for (int i = b->nbytes-1; i >= 0; i--) {
		for (int j = 7; j >= 0; j--) {
			Byte mask = (1 << j);
			if (b->bitstring[i] & mask)
				putchar('1');
			else
				putchar('0');
		}
	}
}
