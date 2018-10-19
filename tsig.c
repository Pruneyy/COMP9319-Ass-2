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
    // This is adapted from the pseudocode provided in the lecture slides by John Shepherd
    int nBits = 0;
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
    //Assert Tuple and Relation are not NULL
	assert(r != NULL && t != NULL);
	
	//Make a new tSig which is a bitString of 0
    Bits tSig = newBits(tsigBits(r));
    unsetAllBits(tSig);
    
    //Create a char array to hold each attribute
    char** attrs = tupleVals(r, t);
    
    //Iterate through each attribute, creating a codeWord at each one
    Count i;
    for(i = 0; i < nAttrs(r); i++) {
        Bits cWord = codeword(attrs[i],tsigBits(r),codeBits(r));
        
        //OR the codeWord with tSig for a "unique" tsig
        orBits(tSig, cWord);
        
        /*
        printf("after attr %d\ncWord\n",i);
	    showBits(cWord); 
	    printf("\n");
        printf("tSig\n");
	    showBits(tSig); 
	    printf("\n");
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
    // Assert query is not NULL
	assert(q != NULL);
	
	// Get query Bits
	Bits querySig = makeTupleSig(q->rel,q->qstring);
	
	// Get pages Bits (initialliy all 0)
    Bits pages = q->pages;
    
    // Get query Relation
    Reln r = q->rel;
    
    // Tracks pID of the tSig page
    PageID i;
    
    // Holds current tSig page
    Page p;
    
    // Tracks which tSig is being checked
    Count whichTsig = 0;
    
    // Tracks which tSig is being checked in the page
    Count ntsigs;
    
    // File handler for tSigs
    File f = tsigFile(r);
    
    // Empty bitString to store current tSig
    Bits matcher = newBits(tsigBits(r));

    // For every tSig page in the tSig File
    for(i = 0; i < nTsigPages(r); i++) {
    
        // Get next page in File
        p = getPage(f, i);
        
        // For all tSigs in current Page
        for (ntsigs = 0;ntsigs < pageNitems(p);ntsigs++) {
            
            // Get next tSig in Matcher
            getBits(p,ntsigs*tsigBits(r),matcher);
            
            // If the query is a subset of the Matcher
            if (isSubset(querySig,matcher)) {
                
                // Save the page ID to pages           
                setBit(pages,whichTsig/maxTupsPP(r));
            }
            
            // Go to next tSig
            whichTsig++;
        }
    }
	// The printf below is primarily for debugging
	// Remove it before submitting this function
	printf("Matched Pages:"); showBits(q->pages); putchar('\n');
}
