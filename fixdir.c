/*
 * Spacewar - normalize direction vector and recompute rotation matrix
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
#include "sys.h"
#include "crft.h"
#include "flds.h"

VOID fixdir(pcrft)
register struct crft *pcrft;
{
	double tmp;

	unity(pcrft->cr_rmat);
	tmp = FMOD(ADD(pcrft->cr_dir[1],TWOPI),TWOPI);
	if (tmp != pcrft->cr_dir[1])
	    biton(pcrft->cr_chng,BIT_DIR1);
	pcrft->cr_dir[1] = tmp;
	zrot(pcrft->cr_rmat,SUB(DIV(PI,2.),tmp));
	tmp = FMOD(ADD(pcrft->cr_dir[2],TWOPI),TWOPI);
	if (tmp != pcrft->cr_dir[2])
	    biton(pcrft->cr_chng,BIT_DIR2);
	pcrft->cr_dir[2] = tmp;
	xrot(pcrft->cr_rmat,tmp);
}
