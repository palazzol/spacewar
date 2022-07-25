/*
 * Spacewar - read in objects and set them up for the universe
 *
 * Copyright 1985 obo Systems, Inc.
 * Copyright 1985 Dan Rosenblatt
 */

#include <stdio.h>
#include "spacewar.h"
#include "universe.h"
#include "obj.h"

VOID objinit()
{
	FILE *fobj;
	struct obj *p=objlst;
	struct universe *puniv;

#ifdef DEBUG
	DBG("objinit()\n");
#endif

	if (!(fobj = fopen(SWOBJ,"r"))) {
	    perror(SWOBJ);
	    exit(1);
	}

	/* try to read the exact number of objects that there should be */
	for (p=objlst;p < objlst+MAXOBJ;++p)
	    if (fscanf(fobj,"%ld\t%hd\t%c\t%d\t%hd\t%ld\t%lf\t%lf\t%lf\t%lf\n",
	    &p->oj_mass,&p->oj_rad,&p->oj_rep,&p->oj_octr.ip_ofst,&p->oj_oprd,
	    &p->oj_orad,&p->oj_ocrpt,&p->oj_optx,&p->oj_opty,&p->oj_optz) != 10) {
		perror("bad object");
		exit(1);
	    }

	if (!feof(fobj) || ferror(fobj) || fclose(fobj)) {
	    perror(SWOBJ);
	    exit(1);
	}

	/* fix up orbital center and rotation matrix */
	for (p=objlst+MAXOBJ;p-- > objlst;) {

	    /* validate */
	    if (p->oj_octr.ip_ofst < 0 ||
	    p->oj_octr.ip_ofst >= p-objlst) {
		if (p != objlst) { /* first object is center of universe */
		    perror("bad oj_octr");
		    exit(1);
		}
	    }
	    switch(p->oj_rep) {
		case '.':
		    p->oj_rep = ' ';
		case '*': case 'O': case 'o': case '#':
		    break;
		default:
		    perror("bad oj_rep");
		    exit(1);
	    }

	    /* rotation matrix */
	    unity(p->oj_rmat);
	    xrot(p->oj_rmat,NEG(MUL(p->oj_optx,DEGTORAD)));
	    yrot(p->oj_rmat,NEG(MUL(p->oj_opty,DEGTORAD)));
	    zrot(p->oj_rmat,NEG(MUL(p->oj_optz,DEGTORAD)));

	    /* place into universe */
	    p->oj_octr.ip_ptr = univlst + p->oj_octr.ip_ofst;
	    puniv = univlst + (p - objlst);
	    puniv->uv_type = 'O';
	    puniv->uv_pctr = p->oj_rep;
	    puniv->uv_pstn = p->oj_pstn;
	    puniv->uv_mass = p->oj_mass;
	    puniv->uv_rad = p->oj_rad;
	    puniv->uv_ptr.uv_obj = p;
		p->oj_univ.ip_ptr = puniv;
	}

	/* update objects so that universe position is current */
	/* (necessary because of first-time players that crank */
	/* up a new instance of the game and how they are put  */
	/* into the universe the very first time)	       */
	updobjs();

#ifdef DEBUG
	VDBG("objinit return\n");
#endif
}
