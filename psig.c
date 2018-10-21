// psig.c ... functions on page signatures (psig's)
// part of SIMC signature files
// Written by John Shepherd, September 2018

#include "defs.h"
#include "reln.h"
#include "query.h"
#include "psig.h"
#include "hash.h"

Bits pagecodeword(char *attrValue, int m, int k)
{
    // This is adapted from the pseudocode provided in the lecture slides by John Shepherd
    int nBits = 0;
    Bits cWord = newBits(m);
    if (strcmp(attrValue,"?") != 0) {
        srandom(hash_any(attrValue, strlen(attrValue)));
        while (nBits < k) {
            int i = random() % m;
            if (!bitIsSet(cWord, i)) {
                setBit(cWord, i);
                nBits++;
            }
        }
    }
    return cWord;
}

/*
pSig = |00000000000000000000000000000000| -> 32 bits
CWtSig 1 = |10010100| -> 8 bits
CWtSig 2 = |01010101| -> 8 bits
CWtSig 3 = |00000000| -> 8 bits
CWtSig 4 = |11110000| -> 8 bits
CWpSig = pSig OR CWtSig1/2/3/4 |10010100010101010000000011110000|
*/
/*
Get current page signature --> Get Tuple we're inserting --> OR old page sig with tuple --> Save as new page sig
*/
Bits makePageSig(Reln r, Tuple t) //TODO
{
    //Assert Tuple and Relation are not NULL
	assert(r != NULL && t != NULL);
	
	//Make a new pSig which is a bitString of 0
    Bits pSig = newBits(psigBits(r));
    unsetAllBits(pSig);
    
    //Create a char array to hold each attribute
    char** attrs = tupleVals(r, t);
    
    //Iterate through each tuple, OR'ing codeword with pSig each time
    Count i;
    for(i = 0; i < nAttrs(r); i++) {
        Bits cWord = pagecodeword(attrs[i],psigBits(r),codeBits(r));    
            
        //OR the matcher with pSig for a "unique" pSig
        orBits(pSig, cWord);
        
    }
	return pSig;
}

/*
QuerySig = makePageSig(Query)
Pages = AllZeroBits
foreach Psig in psigFile {
    if (Psig matches QuerySig) {
        PID = data page corresponding to Psig
        include PID in Pages
    }
}
*/

void findPagesUsingPageSigs(Query q) //TODO
{

	assert(q != NULL);
	
    // Get query Relation
    Reln r = q->rel;
    
	// Get query Bits
	Bits querySig = makePageSig(q->rel,q->qstring);
	
    // Empty bitString to store Page
    Bits pages = q->pages;
    
    // Tracks pID of the pSig page
    PageID i;
    
    // Holds current pSig page
    Page p;
    
    // Tracks which pSig is being checked
    Count whichPsig = 0;
    
    // Tracks which pSig is being checked in the page
    Count npsigs;

    // File handler for pSigs
    File f = psigFile(r);
    
    // Empty bitString to store current pSig
    Bits matcher = newBits(psigBits(r));

    // For every pSig in the pSig File
    for(i = 0; i < nPsigPages(r); i++) {
        // Get next page in File
        p = getPage(f, i);
        q->nsigpages++;
        // For all pSigs in current Page
        for (npsigs = 0;npsigs < pageNitems(p);npsigs++) {
            
            // Get next pSig in Matcher
            getBits(p,npsigs*psigBits(r),matcher);
            q->nsigs++;
            // If the query is a subset of the Matcher
            if (isSubset(querySig,matcher)) {
                
                // Save the page ID to pages           
                setBit(pages,whichPsig);
            }
            
            // Go to next pSig
            whichPsig++;
        }
    }
 
    // The printf below is primarily for debugging
	// Remove it before submitting this function
	//printf("Matched Pages:"); showBits(q->pages); putchar('\n');
}

