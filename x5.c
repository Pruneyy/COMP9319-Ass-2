// test bsig/bits data types

#include <stdio.h>
#include "defs.h"
#include "reln.h"
#include "tuple.h"
#include "psig.h"
#include "bits.h"
#include "bsig.h"

int main(int argc, char **argv)
{
	Reln r = openRelation("R");
	printf("r = %p\n", r);
	printf("Tuple: ");
	Tuple t = readTuple(r, stdin);
	Bits b = makePageSig(r, t);
	showBits(b); putchar('\n');

	printf("t = %p\n", t);
	printf("Query: ");
	Tuple q = readTuple(r, stdin);
	printf("q = %p\n", q);
	Bits bq = makePageSig(r, q);
	
	printf("bq = %p\n", bq);
	showBits(bq); putchar('\n');
	if (isSubset(bq,b))
		printf("Matches\n");
	else
		printf("No match\n");
	return 0;
}
