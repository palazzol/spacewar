/*
 * Spacewar - torpedo intelligence
 *	      thrust torward target
 *	      miss/blow up logic
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
#include "crft.h"
#include "aln.h"
#include "torp.h"
#include "obj.h"

VOID updtorp()
{
	register struct torp *ptorp;
	double tmpvec[3],*pvel,vlen();
	long l,m;
	int i;
	dsplcmnt tmpdspl;

#ifdef DEBUG
	DBG("updtorp()\n");
#endif

	for (ptorp=torplst+MAXTORP;ptorp-- > torplst;) {
	    if (!ptorp->tp_aim.ip_ptr) continue;

	    /* vector and distance to target */
	    /*vdiff(ptorp->tp_aim.ip_ptr->uv_pstn,ptorp->tp_pstn,tmpvec);*/
	    /*l = INT(vlen(tmpvec));*/
	    tmpdspl = vdisp(ptorp->tp_aim.ip_ptr,ptorp->tp_univ.ip_ptr,'b');
	    l = INT(tmpdspl.dst);

	    /* target is getting further away or is real close */
	    if (l > ptorp->tp_dist || l < 50L) {

		/* too far away */
		if (ptorp->tp_dist < 5000L && l > 5000L) {
		    /*idxptr prmv;*/
		    if (ptorp->tp_fby.ip_ptr &&
		    ptorp->tp_fby.ip_ptr->uv_type == 'P') {
			struct crft *pcrft=
			ptorp->tp_fby.ip_ptr->uv_ptr.uv_crft;
			output(pcrft->cr_lgn,'B',0,0);
			setrpt(pcrft);
			rpt(pcrft,"Your torpedo missed");
			fnshrpt(pcrft,1);
		    }
		    removeobj(ptorp->tp_univ);
		    /*for (prmv.ip_ptr=univlst+MAXUNIVERSE;prmv.ip_ptr-- > univlst;)
			if (prmv.ip_ptr->uv_type == 'T' &&
			prmv.ip_ptr->uv_ptr.uv_torp == ptorp) {
			    removeobj(prmv);
			    break;
			}*/
		    continue;

		/* real close */
		} else if (l < 50L) {
		    /*idxptr prmv;*/
		    if (ptorp->tp_fby.ip_ptr &&
		    ptorp->tp_fby.ip_ptr->uv_type == 'P') {
			struct crft *pcrft=
			ptorp->tp_fby.ip_ptr->uv_ptr.uv_crft;
			output(pcrft->cr_lgn,'B',0,0);
			setrpt(pcrft);
			rpt(pcrft,"Your torpedo detonated");
			fnshrpt(pcrft,1);
		    }
		    damage(ptorp->tp_univ.ip_ptr,ptorp->tp_univ.ip_ptr,
		    100.,200.,"Torpedo attack");
		    removeobj(ptorp->tp_univ);
		    /*for (prmv.ip_ptr=univlst+MAXUNIVERSE;prmv.ip_ptr-- > univlst;)
			if (prmv.ip_ptr->uv_type == 'T' &&
			prmv.ip_ptr->uv_ptr.uv_torp == ptorp) {
			    damage(prmv.ip_ptr,ptorp->tp_pstn,100.,200.,
			    "Torpedo attack");
			    removeobj(prmv);
			    break;
			}*/
		    continue;
		}
	    }
	    ptorp->tp_dist = l;

	    /****************************/
	    /* compute thrust to target */
	    /****************************/
	    switch(ptorp->tp_aim.ip_ptr->uv_type) {
		case 'P':
		    pvel = ptorp->tp_aim.ip_ptr->uv_ptr.uv_crft->cr_vel;
		    break;
		case 'T':
		    pvel = ptorp->tp_aim.ip_ptr->uv_ptr.uv_torp->tp_vel;
		    break;
		case 'A':
		    pvel = ptorp->tp_aim.ip_ptr->uv_ptr.uv_aln->al_vel;
		    break;
	    }
	    rttosp(/*tmpvec*/tmpdspl.vec,tmpvec);
	    for (m=0;l > 0L && l > m;) {
		l -= m + MAXTTHR/2;
		m += MAXTTHR;
	    }
	    if (m < MAXTTHR/2) m = MAXTTHR/2;
	    tmpvec[0] = FLOAT(m);
	    sptort(tmpvec,tmpvec);
	    for (i=0;i<3;++i)
		ptorp->tp_thr[i] = ADD(SUB(tmpvec[i],ptorp->tp_vel[i]),pvel[i]);
	}

#ifdef DEBUG
	VDBG("updtorp return\n");
#endif
}
