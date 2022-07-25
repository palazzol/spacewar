/*
 * Spacewar - update craft direction, sensors, and homing channels
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
#include "aln.h"
#include "crft.h"
#include "flds.h"
#include "obj.h"
#include "torp.h"

extern double vlen(),vdist();

VOID upddsh()
{
	register struct crft *pcrft;
	register struct universe *puniv;
	double tmpvec[3];
	int i,sens0,sens1;
	long l;
	dsplcmnt tmpdspl;

#ifdef DEBUG
	DBG("upddsh()\n");
#endif

	for (pcrft=crftlst+MAXCRFT;pcrft-- > crftlst;) {
	    if (!pcrft->cr_htyp) continue;

	    /********************************************/
	    /* direction based on autopilot/faceforward */
	    /********************************************/
	    if (pcrft->cr_ffwd)
		if (vlen(pcrft->cr_vel) == 0.) {
		    pcrft->cr_ffwd = NULL;
		    biton(pcrft->cr_chng,BIT_AUTOFFWD);
		} else
		    rttosp(pcrft->cr_vel,tmpvec);
	    else if (pcrft->cr_auto.ip_ptr) {
		/*vdiff(pcrft->cr_auto.ip_ptr->uv_pstn,pcrft->cr_pstn,tmpvec);*/
		tmpdspl = vdisp(pcrft->cr_auto.ip_ptr,pcrft->cr_univ.ip_ptr,'v');
		rttosp(/*tmpvec*/tmpdspl.vec,tmpvec);
	    }

	    /* update screen by checking rounded value */
	    if (pcrft->cr_ffwd || pcrft->cr_auto.ip_ptr) {
		if (INT(DIV(MUL(pcrft->cr_dir[1],10.),DEGTORAD)) !=
		INT(DIV(MUL(tmpvec[1],10.),DEGTORAD)))
		    biton(pcrft->cr_chng,BIT_DIR1);
		pcrft->cr_dir[1] = tmpvec[1];
		if (INT(DIV(MUL(pcrft->cr_dir[2],10.),DEGTORAD)) !=
		INT(DIV(MUL(tmpvec[2],10.),DEGTORAD)))
		    biton(pcrft->cr_chng,BIT_DIR2);
		pcrft->cr_dir[2] = tmpvec[2];
		fixdir(pcrft);
	    }

	    /***********/
	    /* sensors */
	    /***********/
	    sens0 = sens1 = 0;
	    for (puniv=univlst+MAXUNIVERSE;puniv-- > univlst;) {
		if (!puniv->uv_type) continue;
		if (puniv == pcrft->cr_univ.ip_ptr) continue;
		tmpdspl = vdisp(puniv,pcrft->cr_univ.ip_ptr,'d');
		l = INT(/*vdist(puniv->uv_pstn,pcrft->cr_pstn)*/tmpdspl.dst);
		if (l <= pcrft->cr_vdst)
		    sens0 += 1;
		if (l <= 5000L && puniv->uv_type != 'O')
		    sens1 += 1;
	    }
	    if (sens0 != pcrft->cr_sens[0]) {
		pcrft->cr_sens[0] = sens0;
		biton(pcrft->cr_chng,BIT_SSEE);
	    }
	    if (sens1 != pcrft->cr_sens[1]) {
		pcrft->cr_sens[1] = sens1;
		biton(pcrft->cr_chng,BIT_SBAD);
	    }

	    /*******************/
	    /* homing channels */
	    /*******************/
	    for (i=0;i<MHOM;++i) {
		if (!pcrft->cr_hom[i].ip_ptr) continue;
		tmpdspl = vdisp(pcrft->cr_hom[i].ip_ptr,pcrft->cr_univ.ip_ptr,'d');
		l = INT(/*vdist(pcrft->cr_hom[i].ip_ptr->uv_pstn,pcrft->cr_pstn)*/tmpdspl.dst);
		if (l != pcrft->cr_hdst[i]) {
		    pcrft->cr_hdst[i] = l;
		    biton(pcrft->cr_chng,BIT_HOMCHAN+i);
		}
	    }
	}

#ifdef DEBUG
	VDBG("upddsh return\n");
#endif
}
