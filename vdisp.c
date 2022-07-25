/*
 * Spacewar - return vector displacement
 *			  (buffered to reduce recomputation)
 *
 * Copyright 1986 obo Systems, Inc.
 * Copyright 1986 Dan Rosenblatt
 */

#ifndef VMS
#include <sys/types.h>
#else /* BSD SYSIII SYSV */
#include <types.h>
#endif /* VMS */
#include "spacewar.h"
#include "universe.h"
#include "login.h"
#include "sys.h"
#include "aln.h"
#include "crft.h"
#include "obj.h"
#include "torp.h"


dsplcmnt vdisp(p1,p2,which)
register struct universe *p1,*p2;
char which;
{
	dsplcmnt d;
	register dsplcmnt *pd;
	double vlen();
	int i,p1off;
	int negate=0;
	char *malloc();


	/* use higher of two pointers to cut storage in half ==> negate xyz */
	if (p2 > p1) {
	    struct universe *ptmp;
	    negate = 1;
	    ptmp = p1;
	    p1 = p2;
	    p2 = ptmp;
	}

	p1off = p1 - univlst;

	/* allocate storage if not there already */
	if (!p1->uv_dspl) {
	    if (!(pd = p1->uv_dspl = (dsplcmnt *)
	    malloc(sizeof(dsplcmnt) * (p1off+1)))) {
		perror("vdisp malloc");
		if (negate)
		    vdiff(p2->uv_pstn,p1->uv_pstn,d.vec);
		else
		    vdiff(p1->uv_pstn,p2->uv_pstn,d.vec);
		d.dst = vlen(d.vec);
		return(d);
	    }
	    for (i=p1off;i-- > 0;)
		pd++->valid = 0; /* init to not valid */
	}

	/* point to displacement info and recompute if not valid */
	pd = p1->uv_dspl + (p2 - univlst);
	vdiff(p1->uv_pstn,p2->uv_pstn,pd->vec);
	pd->valid |= VECVALID;
	if ((which == 'd' || which == 'b') && !(pd->valid&DSTVALID)) {
	    pd->dst = vlen(pd->vec);
	    pd->valid |= DSTVALID;
	}

	if (negate) {
	    d.valid = 0;
	    if (which == 'v' || which == 'b') {
		d.vec[0] = -pd->vec[0];
		d.vec[1] = -pd->vec[1];
		d.vec[2] = -pd->vec[2];
		d.valid |= VECVALID;
	    }
	    if (which == 'd' || which == 'b') {
		d.dst = pd->dst;
		d.valid |= DSTVALID;
	    }
	    return(d);
	} else {
	    return(*pd);
	}
}

/* mark all displacements w.r.t. puniv as invalid */
VOID vchngd(puniv)
struct universe *puniv;
{
	register dsplcmnt *pd;
	int poff = (puniv - univlst);
	register int i;

	/* up to diagonal */
	if (pd = puniv->uv_dspl) {
	    for (i=poff;i-- > 0;)
		pd++->valid = 0;
	}

	/* 90 degrees after reaching diagonal */
	while (++puniv < univlst+MAXUNIVERSE)
	    if (pd = puniv->uv_dspl)
		pd[poff].valid = 0;

}

VOID vrmv(puniv)
struct universe *puniv;
{
	if (puniv->uv_dspl)
	    free(puniv->uv_dspl);
	puniv->uv_dspl = 0;
	vchngd(puniv);
}
