// reln.c ... functions on Relations
// part of SIMC signature files
// Written by John Shepherd, September 2018

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "defs.h"
#include "reln.h"
#include "page.h"
#include "tuple.h"
#include "tsig.h"
#include "bits.h"
#include "hash.h"
#include "psig.h"
// open a file with a specified suffix
// - always open for both reading and writing

File openFile(char *name, char *suffix)
{
	char fname[MAXFILENAME];
	sprintf(fname,"%s.%s",name,suffix);
	File f = open(fname,O_RDWR|O_CREAT,0644);
	assert(f >= 0);
	return f;
}

// create a new relation (five files)
// data file has one empty data page

Status newRelation(char *name, Count nattrs, float pF,
                   Count tk, Count tm, Count pm, Count bm)
{
	Reln r = malloc(sizeof(RelnRep));
	RelnParams *p = &(r->params);
	assert(r != NULL);
	p->nattrs = nattrs;
	p->pF = pF,
	p->tupsize = 28 + 7*(nattrs-2);
	Count available = (PAGESIZE-sizeof(Count));
	p->tupPP = available/p->tupsize;
	p->tk = tk; 
	if (tm%8 > 0) tm += 8-(tm%8); // round up to byte size
	p->tm = tm; p->tsigSize = tm/8; p->tsigPP = available/(tm/8);
	if (pm%8 > 0) pm += 8-(pm%8); // round up to byte size
	p->pm = pm; p->psigSize = pm/8; p->psigPP = available/(pm/8);
	if (p->psigPP < 2) { free(r); return -1; }
	if (bm%8 > 0) bm += 8-(bm%8); // round up to byte size
	p->bm = bm; p->bsigSize = bm/8; p->bsigPP = available/(bm/8);
	if (p->bsigPP < 2) { free(r); return -1; }
	r->infof = openFile(name,"info");
	r->dataf = openFile(name,"data");
	r->tsigf = openFile(name,"tsig");
	r->psigf = openFile(name,"psig");
	r->bsigf = openFile(name,"bsig");
	addPage(r->dataf); p->npages = 1; p->ntups = 0;
	addPage(r->tsigf); p->tsigNpages = 1; p->ntsigs = 0;
	addPage(r->psigf); p->psigNpages = 1; p->npsigs = 0;
	addPage(r->bsigf); p->bsigNpages = 1; p->nbsigs = p->pm; // replace this
	// Create a file containing "pm" all-zeroes bit-strings,
    // each of which has length "bm" bits
	//TODO
	closeRelation(r);
	return 0;
}

// check whether a relation already exists

Bool existsRelation(char *name)
{
	char fname[MAXFILENAME];
	sprintf(fname,"%s.info",name);
	File f = open(fname,O_RDONLY);
	if (f < 0)
		return FALSE;
	else {
		close(f);
		return TRUE;
	}
}

// set up a relation descriptor from relation name
// open files, reads information from rel.info

Reln openRelation(char *name)
{
	Reln r = malloc(sizeof(RelnRep));
	assert(r != NULL);
	r->infof = openFile(name,"info");
	r->dataf = openFile(name,"data");
	r->tsigf = openFile(name,"tsig");
	r->psigf = openFile(name,"psig");
	r->bsigf = openFile(name,"bsig");
	read(r->infof, &(r->params), sizeof(RelnParams));
	return r;
}

// release files and descriptor for an open relation
// copy latest information to .info file
// note: we don't write ChoiceVector since it doesn't change

void closeRelation(Reln r)
{
	// make sure updated global data is put in info file
	lseek(r->infof, 0, SEEK_SET);
	int n = write(r->infof, &(r->params), sizeof(RelnParams));
	assert(n == sizeof(RelnParams));
	close(r->infof); close(r->dataf);
	close(r->tsigf); close(r->psigf); close(r->bsigf);
	free(r);
}

// insert a new tuple into a relation
// returns page where inserted
// returns NO_PAGE if insert fails completely

PageID addToRelation(Reln r, Tuple t)
{
	assert(r != NULL && t != NULL && strlen(t) == tupSize(r));
	Page p;  PageID pid;
	RelnParams *rp = &(r->params);
	// add tuple to last page
	pid = rp->npages-1;
	p = getPage(r->dataf, pid);
	// check if room on last page; if not add new page
	if (pageNitems(p) == rp->tupPP) {
		addPage(r->dataf);
		rp->npages++;
		pid++;
		free(p);
		p = newPage();
		if (p == NULL) return NO_PAGE;
	}
	addTupleToPage(r, p, t);
	rp->ntups++;  //written to disk in closeRelation()
	putPage(r->dataf, pid, p);

	// compute tuple signature and add to tsigf //TODO

	// Get last page ID
	PageID pidtsig = rp->tsigNpages - 1;
	
	// Get last page
	Page ptsig = getPage(r->tsigf, pidtsig);
	
	// Check if last page is full
	if (pageNitems(ptsig) == rp->tsigPP) {
	    
	    // If the page is full, make a new page
		addPage(r->tsigf);					
		rp->tsigNpages++;
		pidtsig++;
		free(ptsig);
		ptsig = newPage();
		if (ptsig == NULL) {
		    return NO_PAGE;
	    }
	}
	
	// Get offset within page
	Offset pos = rp->ntsigs % rp->tsigPP;
	
	// Get tSig Bits
	Bits tsig = makeTupleSig(r,t);
	
	// Put tSig in the page
	putBits(ptsig, pos, tsig);
	
	// Update relation
	rp->ntsigs++;

	// Update page with number of tuple sigs
	addOneItem(ptsig);

    // Update page file
	putPage(r->tsigf,pidtsig,ptsig);



	/*
	new Tuple is inserted into page PID
		Psig = makePageSig(Tuple)
	PPsig = fetch page signature for data page PID from psigFile
	merge Psig and PPsig giving a new PPsig
	update page signature for data page PID in psigFile
	*/
	// Compute page signature of tuple
	Bits psig = makePageSig(r,t);

	// Get last page ID
	PageID pidpsig = rp->psigNpages - 1;

	// File handler for tSigs
    File f = psigFile(r);

    // Get last signature page
    Page pp = getPage(f, pidpsig);
    
	// Offset to find the last signature
	Offset ppos = pid % maxPsigsPP(r);
	// Check if data page pid is on last page
	if (nPsigs(r) < pid + 1) {
	    
	    // If the pid is larger than number of psigs, insert new psig
		// If psig page is full, make new page
		if (pageNitems(pp) == maxPsigsPP(r)) {
			addPage(r->psigf);
			rp->psigNpages++;
			pidpsig++;
			free(pp);
			pp = newPage();
			if (pp == NULL) {
				return NO_PAGE;
			}
		}
		Bits blankpsig = newBits(psigBits(r));
		rp->npsigs++;
		putBits(pp, ppos, blankpsig);

		// Update page with number of page sigs
		addOneItem(pp);
	}
	
    // Get last signature in last page signature
	Bits PPsig = newBits(psigBits(r));
    getBits(pp, ppos, PPsig);

	//bit-wise OR the old and new page signatures
	orBits(PPsig, psig);

	// Put pSig in the page
	putBits(pp, ppos, PPsig);
	
    // Update page file
	putPage(r->psigf,pidpsig,pp);
	//TODO



	/*
	PID = data page where new Tuple inserted
	Psig = makePageSig(Tuple)
	for each i in  0..pm-1 {
		if (Psig bit[i] is 1) {
			Slice = get i'th bit slice from bsigFile
			set the PID'th bit in Slice
			write updated Slice back to bsigFile
		}
	}
	*/
	// use page signature to update bit-slices

	Bits slice = newBits(bsigBits(r));	//make empty slice
	Count j;	//tracks bit position of page sig
	PageID bspage = 0;	//tracks bit-slice page id
	Page pb = getPage(bsigFile(r),bspage);	//get initial page
	for (j = 0;j < psigBits(r);j++) {	//for all bits in page sig
		/*if (j % maxBsigsPP(r) == 0) {	//if bit index is a multiple of max bit sigs per page, get the next bit sig page
			putPage(bsigFile(r),j/maxBsigsPP(r),pb); //save previous page
			pb = getPage(bsigFile(r),bspage);	//get next page
			bspage++;	//increment PageID to next page
		}
		*/
		if (bitIsSet(PPsig,j)) {	//if bit in page sig is set
			unsetAllBits(slice);
			getBits(pb,j,slice);	//get bit slice corresponding to page sig bit position index
			setBit(slice, pid);		//set the slice bit at the data page id of the inserted tuple
			putBits(pb,j,slice);	//save the slice to the page
		}
	}
	putPage(bsigFile(r),0,pb);
	return nPages(r)-1;
}

// displays info about open Reln (for debugging)

void relationStats(Reln r)
{
	RelnParams *p = &(r->params);
	printf("Global Info:\n");
	printf("Dynamic:\n");
    printf("  #items:  tuples: %d  tsigs: %d  psigs: %d  bsigs: %d\n",
			p->ntups, p->ntsigs, p->npsigs, p->nbsigs);
    printf("  #pages:  tuples: %d  tsigs: %d  psigs: %d  bsigs: %d\n",
			p->npages, p->tsigNpages, p->psigNpages, p->bsigNpages);
	printf("Static:\n");
    printf("  tups   #attrs: %d  size: %d bytes  max/page: %d\n",
			p->nattrs, p->tupsize, p->tupPP);
	printf("  sigs   bits/attr: %d\n", p->tk);
	printf("  tsigs  size: %d bits (%d bytes)  max/page: %d\n",
			p->tm, p->tsigSize, p->tsigPP);
	printf("  psigs  size: %d bits (%d bytes)  max/page: %d\n",
			p->pm, p->psigSize, p->psigPP);
	printf("  bsigs  size: %d bits (%d bytes)  max/page: %d\n",
			p->bm, p->bsigSize, p->bsigPP);
}
