// bsig.c ... functions on Tuple Signatures (bsig's)
// part of SIMC signature files
// Written by John Shepherd, September 2018

#include "defs.h"
#include "reln.h"
#include "query.h"
#include "bsig.h"
#include "psig.h"

/*
Qsig = makePageSig(Query)
Pages = AllOneBits
for each i in 0..pm-1 {
    if (Psig bit[i] is 1) {
        Slice = get i'th bit slice from bsigFile
        zero bits in Pages which are zero in Slice
    }
}
*/
void findPagesUsingBitSlices(Query q) //TODO
{
	assert(q != NULL);
	// Get query Relation
    Reln r = q->rel;
    
	// Get query Bits
	Bits querySig = makePageSig(q->rel,q->qstring);
	
    // Full bitString to store Page
    Bits pages = q->pages;
    setAllBits(pages);
    
    Bits slice = newBits(bsigBits(r));
	Count j;
	Count bspage = 0;
    Count bitnum;
	Page pb = getPage(bsigFile(r),bspage);
	for (j = 0;j < psigBits(r);j++) {
		if (j % maxBsigsPP(r) == 0) {
			pb = getPage(bsigFile(r),bspage);
		}
		if (bitIsSet(querySig,j)) {
			getBits(pb,(j%maxBsigsPP(r))*bsigBits(r),slice);
            for (bitnum = 0;bitnum < bsigBits(r);bitnum++) {
                if (!bitIsSet(slice,bitnum)) {
                    unsetBit(pages, bitnum);
                }
            }
		}
	}
    // The printf below is primarily for debugging
	// Remove it before submitting this function
	printf("Matched Pages:"); showBits(q->pages); putchar('\n');
}

