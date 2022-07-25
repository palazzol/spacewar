/*
 * Spacewar - updates movement of non-objects
 *	      from rocket thrust
 *	      from gravity
 *	      checks and carries out crashing into objects
 *	      docked crfts are updated
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
#include "login.h"
#include "obj.h"
#include "crft.h"
#include "aln.h"
#include "torp.h"
#include "build.h"
#include "flds.h"

extern double vlen();

VOID updmov()
{
	register struct universe *puniv,*pobj;
	register struct sys *psys;
	double *ppstn,*pvel,*pthr;
	double accel[3],ftmp,tmpvec[3];
	char *pchng;
	long l,m,sub;
	register int i;
	dsplcmnt tmpdspl;

#ifdef DEBUG
	DBG("updmov()\n");
#endif

	for (puniv=univlst+MAXUNIVERSE;puniv-- > univlst;) {

	    /**********/
	    /* set up */
	    /**********/
	    ppstn = puniv->uv_pstn;
	    sub = 0L;
	    switch(puniv->uv_type) {
		case 'A':
		    pvel = puniv->uv_ptr.uv_aln->al_vel;
		    pthr = puniv->uv_ptr.uv_aln->al_thr;
		    psys = puniv->uv_ptr.uv_aln->al_sys;
		    pchng = NULL;
		    break;
		case 'P':
		    if (puniv->uv_ptr.uv_crft->cr_dock.ip_ptr) continue;
		    pvel = puniv->uv_ptr.uv_crft->cr_vel;
		    pthr = puniv->uv_ptr.uv_crft->cr_thr;
		    psys = puniv->uv_ptr.uv_crft->cr_sys;
		    pchng = puniv->uv_ptr.uv_crft->cr_chng;
		    break;
		case 'T':
		    pvel = puniv->uv_ptr.uv_torp->tp_vel;
		    pthr = puniv->uv_ptr.uv_torp->tp_thr;
		    psys = NULL;
		    pchng = NULL;
		    break;
		default:
		    continue;
	    }

	    /***********************************/
	    /* acceleration from rocket thrust */
	    /***********************************/
	    rttosp(pthr,accel);
	    if (accel[0] != 0.) {
		l = INT(accel[0]);

		/* limited by how much fuel is available and maximum */
		/* acceleration (reduced by damage to rockets) */
		if (psys && psys[ROCKETS].s_cap) {
		    if (l > psys[ROCKETS].s_lvl) {
			sub += l - psys[ROCKETS].s_lvl;
			l = psys[ROCKETS].s_lvl;
		    }
		    m = psys[ROCKETS].s_cap;
		    m *= 100L - psys[ROCKETS].s_dmg;
		    m /= 100L;
		    if (l > m) {
			sub += l - m;
			l = m;
		    }
		}

		/* limited by maximum hull acceleration */
		if (psys && l > psys[HULL].s_cap) {
		    sub += l - psys[HULL].s_cap;
		    l = psys[HULL].s_cap;
		} else if (puniv->uv_type == 'T' && l > MAXTTHR) {
		    sub += l - MAXTTHR;
		    l = MAXTTHR;
		}

		/* reduce fuel (only for Players) */
		if (l && psys && psys[ROCKETS].s_cap) {
		    if (pchng) {
			psys[ROCKETS].s_lvl -= l;
			biton(pchng,BIT_SLEVEL+ROCKETS*flds[FLD_SLEVEL].f_grpw);
		    }
		}

		/* reduce thrust */
		accel[0] = SUB(accel[0],FLOAT(sub));
		sptort(accel,accel);
		for (i=0;i<3;++i) {
		    pthr[i] = SUB(pthr[i],accel[i]);

		    /* close enough to zero to be zero */
		    if ((ftmp=pthr[i]) < 0.) ftmp = NEG(ftmp);
		    if (SUB(ftmp,1.) < 0.) pthr[i] = 0.;
		}
	    }

	    /*****************************/
	    /* acceleration from gravity */
	    /*****************************/
	    if (!pchng || !puniv->uv_ptr.uv_crft->cr_plvl)
		for (pobj=univlst+MAXOBJ;pobj-- > univlst;) {

		    /* vector and distance to object */
		    /*vdiff(pobj->uv_pstn,ppstn,tmpvec);*/
		    /*ftmp = vlen(tmpvec);*/
		    tmpdspl = vdisp(pobj,puniv,'b');
		    ftmp = tmpdspl.dst;

		    /* crash */
		    if ((l = INT(ftmp)) < 2*(pobj->uv_rad+1)) {
			struct crft *pcrft;
			if (puniv->uv_type == 'T') {
			    if (puniv->uv_ptr.uv_torp->tp_fby.ip_ptr &&
			    puniv->uv_ptr.uv_torp->
			    tp_fby.ip_ptr->uv_type == 'P') {
				setrpt(pcrft=puniv->uv_ptr.uv_torp->
				tp_fby.ip_ptr->uv_ptr.uv_crft);
				output(pcrft->cr_lgn,'B',0,0);
				rpt(pcrft,"Your torpedo crashed");
				fnshrpt(pcrft,1);
			    }
			    if (puniv->uv_ptr.uv_torp->tp_lhit.ip_ptr &&
			    puniv->uv_ptr.uv_torp->
			    tp_lhit.ip_ptr->uv_type == 'P') {
				setrpt(pcrft=puniv->uv_ptr.uv_torp->
				tp_lhit.ip_ptr->uv_ptr.uv_crft);
				output(pcrft->cr_lgn,'B',0,0);
				rpt(pcrft,"Torpedo destroyed by you (crashed)");
				fnshrpt(pcrft,1);
			    }
			} else if (puniv->uv_type == 'A') {
			    if (puniv->uv_ptr.uv_aln->al_lhit.ip_ptr &&
			    puniv->uv_ptr.uv_aln->
			    al_lhit.ip_ptr->uv_type == 'P') {
				setrpt(pcrft=puniv->uv_ptr.uv_aln->
				al_lhit.ip_ptr->uv_ptr.uv_crft);
				output(pcrft->cr_lgn,'B',0,0);
				rpt(pcrft,"Alien destroyed by you (crashed)");
				fnshrpt(pcrft,1);
			    }
			}
			if (pchng) {
			    struct login *plgn;
			    pcrft = puniv->uv_ptr.uv_crft;
			    pcrft->cr_dock.ip_ptr = NULL; /* assure destruc. */
			    pcrft->cr_sens[1] = 1; /* assure destruction */
			    unplay(plgn=pcrft->cr_lgn);
			    output(plgn,'B',0,0);
			    output(plgn,'E',0,0);
			    output(plgn,'C',0,"You have crashed");
			    output(plgn,'C',0,PROMPT);
			    output(plgn,0,0,0);
			} else {
			    idxptr prmv;
			    prmv.ip_ptr = puniv;
			    remove(prmv);
			}
			goto nxtuobj;
		    }

		    /* acceleration from gravity */
		    if (l < 250L) { /* cube root of 10*greatest_mass_object */
			ftmp = CUBE(ftmp);
			for (i=0;i<3;++i)
			    accel[i] = ADD(accel[i],
			    DIV(MUL(pobj->uv_mass,tmpdspl.vec[i]),ftmp));
		    }
		}

	    /* show acceleration if Player */
	    if (pchng)
		if ((l=INT(vlen(accel))) != psys[HULL].s_lvl) {
		    psys[HULL].s_lvl = l;
		    biton(pchng,BIT_SLEVEL+HULL*flds[FLD_SLEVEL].f_grpw);
		}

	    /*******************************************/
	    /* position and velocity from acceleration */
	    /*******************************************/
	    for (i=0;i<3;++i) {

		/* close enough to zero to be zero */
		if ((ftmp=accel[i]) < 0.) ftmp = NEG(ftmp);
		if (SUB(ftmp,.1) < 0.) accel[i] = 0.;

		ftmp = ADD(ADD(ppstn[i],pvel[i]),DIV(accel[i],2.));
		if (pchng && INT(ftmp) != INT(ppstn[i]))
		    biton(pchng,BIT_PN1+i);
		ppstn[i] = ftmp;

		ftmp = ADD(pvel[i],accel[i]);
		if (pchng && INT(MUL(ftmp,50.)) != INT(MUL(pvel[i],50.)))
		    biton(pchng,BIT_VEL1+i);
		pvel[i] = ftmp;

		/* close enough to zero to be zero */
		if (ftmp < 0.) ftmp = NEG(ftmp);
		if (SUB(ftmp,.1) < 0.) pvel[i] = 0.;
	    }
	    vchngd(puniv);
nxtuobj:;
	}

	/**********/
	/* docked */
	/**********/
	for (puniv=univlst+MAXUNIVERSE;puniv-- > univlst;) {
	    if (puniv->uv_type != 'P') continue;
	    pvel = puniv->uv_ptr.uv_crft->cr_vel;
	    if (pobj = puniv->uv_ptr.uv_crft->cr_dock.ip_ptr) {
		/*vdiff(pobj->uv_pstn,puniv->uv_pstn,tmpvec);*/
		tmpdspl = vdisp(pobj,puniv,'v');
		vcopy(tmpvec,tmpdspl.vec);
		for (i=0;i<3;++i) {

		    /* close enough to zero to be zero */
		    if ((ftmp=tmpvec[i]) < 0.) ftmp = NEG(ftmp);
		    if (SUB(ftmp,.1) < 0.) tmpvec[i] = 0.;

		    if (INT(MUL(tmpvec[i],50.)) != INT(MUL(pvel[i],50.)))
			biton(puniv->uv_ptr.uv_crft->cr_chng,BIT_VEL1+i);
		    pvel[i] = tmpvec[i];

		    if (INT(pobj->uv_pstn[i]) != INT(puniv->uv_pstn[i]))
			biton(puniv->uv_ptr.uv_crft->cr_chng,BIT_PN1+i);
		    puniv->uv_pstn[i] = pobj->uv_pstn[i];
		}
		vchngd(puniv);
	    }
	}

#ifdef DEBUG
	VDBG("updmov return\n");
#endif
}
