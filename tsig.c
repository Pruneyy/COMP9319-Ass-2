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
/* ===pseudocode from spec===
Tsig = AllZeroBits
for each attribute A in tuple T {
    CW = codeword for A
    Tsig = Tsig OR CW
}
*/
Bits makeTupleSig(Reln r, Tuple t) //TODO
{
    //printf("make tuple sig\n");
	assert(r != NULL && t != NULL);
    Bits tSig = newBits(tsigBits(r));
    unsetAllBits(tSig);
    //showBits(tSig); printf("\n");
    char** attrs = tupleVals(r, t);
    Count i;
    for(i = 0;i < nAttrs(r);i++) {
        Bits cWord = codeword(attrs[i],tsigBits(r),codeBits(r));
        
        orBits(tSig, cWord);
        /*printf("after attr %d\ncWord\n",i);
	    showBits(cWord); printf("\n");
        printf("tSig\n");
	    showBits(tSig); printf("\n");
        */
    }
	return tSig;
}

// find "matching" pages using tuple signatures


// This takes a query as an input --> Does that mean we need a function that converts a query to a tuple?
/* ===pseudocode from spec===
QuerySig = makeTupleSig(Query)
Pages = AllZeroBits
foreach Tsig in tsigFile {
    if (Tsig matches QuerySig) {
        PID = data page for tuple corresponding to Tsig
        include PID in Pages
    }
}
*/
void findPagesUsingTupSigs(Query q)//TODO
{
	assert(q != NULL);
	Bits querySig = makeTupleSig(q->rel,q->qstring); //get query Bits
    Bits pages = q->pages;                           //get pages Bits, initially all 0
    Reln r = q->rel;                                 //get query Relation
	//setAllBits(q->pages); // remove this
    //set up looping
    PageID i; //tracks the PageID of the tsig page
    Page p;     //contains the current tsig page
    Count whichTsig = 0; //tracks which tsig is currently being checked
    Count ntsigs;        //tracks which tsig is currently being checked in the page
    File f = tsigFile(r);//handle of the tsig File
    Bits matcher = newBits(tsigBits(r)); //empty bits used to store current tsig

    for(i = 0;i < nTsigPages(r);i++) {//for all tsig pages in tsig File
        p = getPage(f, i);     //get next page in file
        for (ntsigs = 0;ntsigs < pageNitems(p);ntsigs++) { //for all tsig in page
            getBits(p,ntsigs*tsigBits(r),matcher);      //get next tsig in matcher
            if (isSubset(querySig,matcher)) {           //if query is a subset/match of matcher
                setBit(pages,whichTsig/maxTupsPP(r));    //calculate data page id and save to pages
            }
            whichTsig++;    //increment tsig tracker to next tsig
        }
    }
	// The printf below is primarily for debugging
	// Remove it before submitting this function
	printf("Matched Pages:"); showBits(q->pages); putchar('\n');
}
