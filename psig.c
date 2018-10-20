// psig.c ... functions on page signatures (psig's)
// part of SIMC signature files
// Written by John Shepherd, September 2018

#include "defs.h"
#include "reln.h"
#include "query.h"
#include "psig.h"
/*
pSig = |00000000000000000000000000000000| -> 32 bits
CWtSig 1 = |10010100| -> 8 bits
CWtSig 2 = |01010101| -> 8 bits
CWtSig 3 = |00000000| -> 8 bits
CWtSig 4 = |11110000| -> 8 bits
CWpSig = pSig OR CWtSig1/2/3/4
*/

Bits makePageSig(Reln r, Tuple t) //TODO
{
    //Assert Tuple and Relation are not NULL
	assert(r != NULL && t != NULL);
	
	//Make a new pSig which is a bitString of 0
    Bits pSig = newBits(psigBits(r));
    unsetAllBits(pSig);
    
    // File handler for tSigs
    File f = tsigFile(r);

    // Tracks pID of the tSig page
    PageID i = 0;
    
    // Get current Page
    Page p = getPage(f, i);  
    
    // Empty bitString to store current tSig
    Bits matcher = newBits(tsigBits(r));
       
    //Iterate through each tuple, OR'ing codeword with pSig each time
    Count ntsigs;
    for (ntsigs = 0;ntsigs < pageNitems(p); ntsigs++) {
        getBits(p,ntsigs*tsigBits(r),matcher);    
            
        //OR the matcher with pSig for a "unique" pSig
        orBits(pSig, matcher);
        //So I think I'm meant to shift pSig across by matcher bits each time? Otherwise we just keep OR'ing and overwriting the same bits?
        //orBits(pSig>>ntSigs*8, matcher); ?
        
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
// I THINK THIS PSEUDOCODE IS FOR THE RELN.C FILE
/*
new Tuple is inserted into page PID
Psig = makePageSig(Tuple)
PPsig = fetch page signature for data page PID from psigFile
merge Psig and PPsig giving a new PPsig
update page signature for data page PID in psigFile
*/
void findPagesUsingPageSigs(Query q) //TODO
{
	assert(q != NULL);
	
	// Get query Bits
	Bits querySig = makePageSig(q->rel,q->qstring);
	
    // Empty bitString to store Page
    Bits pages = newBits(psigBits(r));
    
    // File handler for pSigs
    File f = psigFile(r);
    
    // For every pSig in the pSig File
    Count i;
    for(i = 0; i < nPsigPages(r); i++) {
    
    
	//setAllBits(q->pages); // remove this
}

