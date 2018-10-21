// query.c ... query scan functions
// part of SIMC signature files
// Manage creating and using Query objects
// Written by John Shepherd, September 2018

#include "defs.h"
#include "query.h"
#include "reln.h"
#include "tuple.h"
#include "bits.h"
#include "tsig.h"
#include "psig.h"
#include "bsig.h"

// check whether a query is valid for a relation
// e.g. same number of attributes

int checkQuery(Reln r, char *q)
{
	if (*q == '\0') return 0;
	char *c;
	int nattr = 1;
	for (c = q; *c != '\0'; c++)
		if (*c == ',') nattr++;
	return (nattr == nAttrs(r));
}

// take a query string (e.g. "1234,?,abc,?")
// set up a QueryRep object for the scan

Query startQuery(Reln r, char *q, char sigs)
{
	Query new = malloc(sizeof(QueryRep));
	assert(new != NULL);
	if (!checkQuery(r,q)) return NULL;
	new->rel = r;
	new->qstring = q;
	new->nsigs = new->nsigpages = 0;
	new->ntuples = new->ntuppages = new->nfalse = 0;
	new->pages = newBits(nPages(r));
	switch (sigs) {
	case 't': findPagesUsingTupSigs(new); break;
	case 'p': findPagesUsingPageSigs(new); break;
	case 'b': findPagesUsingBitSlices(new); break;
	default:  setAllBits(new->pages); break;
	}
	new->curpage = 0;
	return new;
}

// scan through selected pages (q->pages)
// search for matching tuples and show each
// accumulate query stats

/* ===FROM SPEC===
foreach PID in 0 .. npages-1 {
    if (PID is not set in MatchingPages)
        ignore this page
    for each tuple T in page PID {
        if (T matches the query string)
            display it as a query result
    }
    if (no tuples in page PID are results)
        count it as a false match page
}
*/
void scanAndDisplayMatchingTuples(Query q) //TODO
{
	assert(q != NULL);
	Count i;
	for (i = 0; i < nPages(q->rel); i++) {
	    if (bitIsSet(q->pages, i)) {
	        Page p = getPage(dataFile(q->rel), i);
			q->ntuppages++;
			Count j;
			Bool match = FALSE;
			Tuple query = q->qstring;
			for (j = 0; j < pageNitems(p); j++) {
				Tuple T = getTupleFromPage(q->rel, p, j);
				if (tupleMatch(q->rel, T, query)) {
					showTuple(q->rel, T);
					match = TRUE;
				}
				q->ntuples++;
			}
			if (!match) {
				q->nfalse++;
			}
			/*if (no tuples in page PID are results)
        		count it as a false match page*/
        }
    }
}

// print statistics on query

void queryStats(Query q)
{
	printf("# signatures read:   %d\n", q->nsigs);
	printf("# sig pages read:    %d\n", q->nsigpages);
	printf("# tuples examined:   %d\n", q->ntuples);
	printf("# data pages read:   %d\n", q->ntuppages);
	printf("# false match pages: %d\n", q->nfalse);
}

// clean up a QueryRep object and associated data

void closeQuery(Query q)
{
	free(q->pages);
	free(q);
}

