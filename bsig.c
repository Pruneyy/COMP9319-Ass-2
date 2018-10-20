// bsig.c ... functions on Tuple Signatures (bsig's)
// part of SIMC signature files
// Written by John Shepherd, September 2018

#include "defs.h"
#include "reln.h"
#include "query.h"
#include "bsig.h"

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
void findPagesUsingBitSlices(Query q)
{
	assert(q != NULL);
	//TODO
	setAllBits(q->pages); // remove this
}

