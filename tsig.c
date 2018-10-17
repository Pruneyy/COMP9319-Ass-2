// tsig.c ... functions on Tuple Signatures (tsig's)
// part of SIMC signature files
// Written by John Shepherd, September 2018

#include <unistd.h>
#include <string.h>
#include "defs.h"
#include "tsig.h"
#include "reln.h"
#include "hash.h"
#include "bits.h"

/*
bits codeword(char *attr_value, int m, int k)
{
    int nbits = 0; // count of set bits
    bits cword = 0; // assuming m <= 32 bits
    srandom(hash(attr_value));
    while (nbits < k) {
        int i = random() % m;
        if (((1 << i) & cword) == 0) {
            cword |= (1 << i);
            nbits++;
        }
    }
    return cword; // m-bits with k 1-bits and m-k 0-bits
}
*/

Bits codeword(char *attrValue, int m, int k)
{
    int nBits = 0;
    //It says assuming m < 32 bits, will this always be true?
    Bits cWord = newBits(m);
    srandom(hash_any(attrValue, strlen(attrValue)));
    while (nBits < k) {
        int i = random() % m;
        if (!bitIsSet(cWord, i)) {
            setBit(cWord, i);
            nBits++;
        }
    }
    return cWord;
}

// make a tuple signature

Bits makeTupleSig(Reln r, Tuple t)
{
	assert(r != NULL && t != NULL);

	//TODO
	return NULL;
}

// find "matching" pages using tuple signatures


// This takes a query as an input --> Does that mean we need a function that converts a query to a tuple?
void findPagesUsingTupSigs(Query q)
{
	assert(q != NULL);
	//TODO

	setAllBits(q->pages); // remove this

	// The printf below is primarily for debugging
	// Remove it before submitting this function
	printf("Matched Pages:"); showBits(q->pages); putchar('\n');
}
