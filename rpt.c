/*
 * Spacewar - routines to report to a playing screen
 *
 * Copyright 1985 obo Systems, Inc.
 * Copyright 1985 Dan Rosenblatt
 */

#include "spacewar.h"
#ifndef VMS
#include <sys/types.h>
#else /* BSD SYSIII SYSV */
#include <types.h>
#endif /* VMS */
#include "universe.h"
#include "login.h"
#include "sys.h"
#include "crft.h"
#include "flds.h"

static char nxtrpt;

VOID rpt(pcrft,msg)
struct crft *pcrft;
char *msg;
{
	if (nxtrpt >= flds[FLD_REPORT].f_maxg)
		return;
	output(pcrft->cr_lgn,'H',(nxtrpt<<8)|FLD_REPORT,msg);
	++nxtrpt;
}

/*ARGSUSED*/
VOID setrpt(pcrft)
struct crft *pcrft;
{
	nxtrpt = 0;
}

VOID fnshrpt(pcrft,flsh)
register struct crft *pcrft;
int flsh;
{
	int i;

	for (i=nxtrpt;i <= pcrft->cr_lrpt;++i)
		output(pcrft->cr_lgn,'H',(i<<8)|FLD_REPORT,"");
	pcrft->cr_lrpt = nxtrpt - 1;
	if (flsh) output(pcrft->cr_lgn,0,0,0);
}
