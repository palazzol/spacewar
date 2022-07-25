/*
 * Spacewar - alien intelligence
 *	      thrust toward and attack
 *	      find a player to attack/randomly de-assign
 *	      if not attacking, randomly thrust toward objects
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
#include "aln.h"
#include "build.h"
#include "flds.h"
#include "login.h"
#include "crft.h"
#include "torp.h"
#include "obj.h"

static VOID thr(paln,tmpvec)
struct aln *paln;
double *tmpvec;
{
	rttosp(tmpvec,tmpvec);
	tmpvec[0] = FLOAT(RANDOM(50)+20);
	sptort(tmpvec,tmpvec);
	vdiff(tmpvec,paln->al_vel,paln->al_thr);
}

VOID updaln()
{
	register struct aln *paln;
	struct sys *psys;
	register struct universe *puniv;
	double vdist(),ftmp,tmpvec[3];
	long l;
	int i;
	dsplcmnt tmpdspl;

#ifdef DEBUG
	DBG("updaln()\n");
#endif

	for (paln=alnlst+MAXALN;paln-- > alnlst;paln->al_dly -= 1) {
	    psys = paln->al_sys;
	    if (paln->al_atck.ip_ptr) {
		/*ftmp = vdist(paln->al_atck.ip_ptr->uv_pstn,paln->al_pstn);*/
		tmpdspl = vdisp(paln->al_atck.ip_ptr,paln->al_univ.ip_ptr,'d');
		ftmp = tmpdspl.dst;
	    }

	    /* not attacking or delaying or no energy+phasers to attack with */
	    if (!paln->al_atck.ip_ptr || paln->al_dly > 0 ||
	    !psys[DILITH].s_cap || !psys[PHASERS].s_cap)
		goto noatck;

	    /* too far away to attack; less chance if closer */
	    if (SUB(ftmp,2500.) > 0.)
		goto noatck;
	    if (RANDOM(100) > 15 + INT(ftmp)/25)
		goto noatck;

	    /* attack and set delay */
	    if ((l = INT(ftmp)/(RANDOM(50)+1)) > 100L)
		l = 100L;
	    if (paln->al_aeval > 500)
		l += paln->al_aeval/20;
	    if (l > psys[DILITH].s_lvl && psys[DILITH].s_lvl < 50)
		goto noatck;
	    if (l > psys[DILITH].s_lvl)
		l = psys[DILITH].s_lvl;
	    psys[DILITH].s_lvl -= l;
	    l *= psys[PHASERS].s_pct;
	    l /= 100L;
	    l *= 100L - psys[PHASERS].s_dmg;
	    l /= 100L;
	    l *= psys[PHASERS].s_cap;
	    l /= 100L;
	    /*for (puniv=univlst+MAXUNIVERSE;puniv-- > univlst;)
		if (puniv->uv_type == 'A' && puniv->uv_ptr.uv_aln == paln)
		    damage(puniv,paln->al_atck.ip_ptr,1.,FLOAT(l),
		    "Phaser attack");
	    */
	    damage(paln->al_univ.ip_ptr,paln->al_atck.ip_ptr,1.,FLOAT(l),
	    "Phaser attack");
	    paln->al_dly = 3;
noatck:

	    /* find an attackee */
	    if (paln->al_atck.ip_ptr)
		;
	    else if (RANDOM(100) < 10) {	/* find a close one */
		for (puniv=univlst+MAXUNIVERSE;puniv-- > univlst;)
		    if (puniv->uv_type == 'P' &&
		    !puniv->uv_ptr.uv_crft->cr_dock.ip_ptr) {
			tmpdspl = vdisp(paln->al_univ.ip_ptr,puniv,'d');
			if (SUB(tmpdspl.dst,500.) < 0.) {
			    paln->al_atck.ip_ptr = puniv;
			    paln->al_dly = 50;
			    ftmp = tmpdspl.dst;
			    break;
			}
		    }
	    } else if (RANDOM(100) > 90) {	/* choose a random one */
		puniv = univlst + RANDOM(MAXUNIVERSE-MAXOBJ) + MAXOBJ;
		if (puniv->uv_type == 'P' &&
		!puniv->uv_ptr.uv_crft->cr_dock.ip_ptr) {
		    paln->al_atck.ip_ptr = puniv;
		    paln->al_dly = 50;
		    /*ftmp = vdist(puniv->uv_pstn,paln->al_pstn);*/
		    tmpdspl = vdisp(puniv,paln->al_univ.ip_ptr,'d');
		    ftmp = tmpdspl.dst;
		}
	    }

	    /* randomly de-assign; decreasing probability based on aeval */
	    if (paln->al_atck.ip_ptr &&
	    RANDOM(5000) < 500 - paln->al_aeval)
		paln->al_atck.ip_ptr = NULL;
	    if (paln->al_aeval > 0)
		paln->al_aeval -= 1; /* decay of evaluation (lose interest) */

	    /* get closer to attackee by warping */
	    if (paln->al_atck.ip_ptr && psys[WARP].s_cap &&
	    (l=500L+15L*INT(SQRT(ftmp))) > 1170L && psys[WARP].s_lvl >= l) {
		psys[WARP].s_lvl -= l;
		paln->al_dly = 5;
		for (i=0;i<3;++i)
		    paln->al_pstn[i] = ADD(paln->al_atck.ip_ptr->uv_pstn[i],
		    FLOAT(RANDOM(600) - 300));
		vchngd(paln->al_univ.ip_ptr);
		for (puniv=univlst+MAXUNIVERSE;puniv-- > univlst;) {
		    register struct crft *pcrft;
		    if (puniv->uv_type != 'P') continue;
		    pcrft = puniv->uv_ptr.uv_crft;
		    if (pcrft->cr_auto.ip_ptr &&
		    pcrft->cr_auto.ip_ptr->uv_type == 'A' &&
		    pcrft->cr_auto.ip_ptr->uv_ptr.uv_aln == paln) {
			pcrft->cr_auto.ip_ptr = NULL;
			biton(pcrft->cr_chng,BIT_AUTOFFWD);
		    }
		    if (pcrft->cr_dock.ip_ptr &&
		    pcrft->cr_dock.ip_ptr->uv_type == 'A' &&
		    pcrft->cr_auto.ip_ptr->uv_ptr.uv_aln == paln) {
			pcrft->cr_dock.ip_ptr = NULL;
			pcrft->cr_scrn[7][15] = '?';
		    }
		}
	    }

	    /* compute thrust */
	    /* not attacking, go toward an object */
	    if (!paln->al_atck.ip_ptr && paln->al_dly <= 0) {
		puniv = univlst + RANDOM(MAXOBJ);
		/*vdiff(puniv->uv_pstn,paln->al_pstn,tmpvec);*/
		tmpdspl = vdisp(puniv,paln->al_univ.ip_ptr,'v');
		thr(paln,/*tmpvec*/tmpdspl.vec);
		paln->al_dly = 200;

	    /* attacking, go toward attackee */
	    } else if (paln->al_atck.ip_ptr) {

		if (paln->al_dly < -60) { /* long wait ==> break off */
		    /*vdiff(paln->al_pstn,paln->al_atck.ip_ptr->uv_pstn,tmpvec);*/
		    tmpdspl = vdisp(paln->al_univ.ip_ptr,paln->al_atck.ip_ptr,'v');
		    thr(paln,/*tmpvec*/tmpdspl.vec);
		    paln->al_dly = 20;
		    paln->al_atck.ip_ptr = NULL;
		} else {
		    /*ftmp = vdist(paln->al_pstn,paln->al_atck.ip_ptr->uv_pstn);*/
		    tmpdspl = vdisp(paln->al_univ.ip_ptr,paln->al_atck.ip_ptr,'d');
		    ftmp = tmpdspl.dst;
		    for (i=0;i<3;++i)
			tmpvec[i] = ADD(paln->al_pstn[i],paln->al_vel[i]);
		    ftmp = SUB(ftmp,vdist(tmpvec,paln->al_atck.ip_ptr->uv_pstn));
		    if (ftmp <= 0.) { /* getting further away */
			/*vdiff(paln->al_atck.ip_ptr->uv_pstn,paln->al_pstn,tmpvec);*/
			tmpdspl = vdisp(paln->al_atck.ip_ptr,paln->al_univ.ip_ptr,'v');
			thr(paln,/*tmpvec*/tmpdspl.vec);
		    }
		}
	    }
	}

#ifdef DEBUG
	VDBG("updaln return\n");
#endif
}
