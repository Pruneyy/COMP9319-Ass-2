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
	showBits(pages); printf("\n");
    
    Bits slice = newBits(bsigBits(r)); //get new slice
	Count j;		//bit index of page signature
	Count bspage = 0;	//bit slice page
    Count bitnum;	//bit index of slice
	Page pb = getPage(bsigFile(r),bspage); //get initial bit slice page
	q->nsigpages++;
	printf("sig pages = %d\n",q->nsigpages);
	for (j = 0;j < psigBits(r);j++) {	//for each bit in page signature
		/*if (j % maxBsigsPP(r) == 0) {	//if bit index is multiple of max bit slice per page, get next page
			pb = getPage(bsigFile(r),bspage);	//get next page
			q->nsigpages++;
			bspage++;	//increment pageID to next page
		}*/
		if (bitIsSet(querySig,j)) {	//if j'th bit in page sig is set
			getBits(pb,j,slice);	//get the j'th slice
			printf("j:%d  ",j); showBits(slice); printf("\n");
			q->nsigs++;
            for (bitnum = 0;bitnum < nPages(r);bitnum++) {	//for each bit in the slice
                printf("bitnum = %d\n",bitnum);
				if (!bitIsSet(slice,bitnum)) {					//if bit in slice is not set
                    unsetBit(pages, bitnum);					//unset the bit in pages
                }
            }
		}
	}
    // The printf below is primarily for debugging
	// Remove it before submitting this function
	printf("Matched Pages:"); showBits(q->pages); putchar('\n');
}

