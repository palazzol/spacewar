/*
 * Spacewar - remove (nicely) an object from the universe (crft, torp, aln)
 *	      fixes (*)lhit,atck,dock,hom,auto
 *	      aliens are relocated (as if removed and replaced) when removed
 *	      torps are NOT removed if firing crft/aln removed
 *	      torps report a miss if fired at crft/torp/aln removed
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
#include "obj.h"
#include "torp.h"
#include "flds.h"
#include "aln.h"

VOID remove(prmv)
idxptr prmv;
{
	struct universe *puniv=univlst+MAXUNIVERSE;
	struct aln *paln;
	struct crft *pcrft;
	int i;
	struct torp *ptorp;

#ifdef DEBUG
	DBG("remove(#%d/'%c/%c')\n",prmv.ip_ptr-univlst,prmv.ip_ptr->uv_type,
	prmv.ip_ptr->uv_pctr);
#endif

	while (--puniv >= univlst) {
	    if (!puniv->uv_type) continue;
	    /*if (puniv == prmv.ip_ptr) continue;???*/

	    switch(puniv->uv_type) {

		case 'A':
		    paln = puniv->uv_ptr.uv_aln;
		    if (paln->al_lhit.ip_ptr == prmv.ip_ptr) {
			paln->al_lhit.ip_ptr = NULL;
			paln->al_aeval = 0;
		    }
		    if (paln->al_atck.ip_ptr == prmv.ip_ptr) {
			paln->al_atck.ip_ptr = NULL;
			paln->al_dly = 0;
		    }
		    break;

		case 'P':
		    pcrft = puniv->uv_ptr.uv_crft;
		    if (pcrft->cr_dock.ip_ptr == prmv.ip_ptr) {
			pcrft->cr_dock.ip_ptr = NULL;
			pcrft->cr_scrn[7][15] = '?';
		    }
		    for (i=0;i < MHOM;++i)
			if (pcrft->cr_hom[i].ip_ptr == prmv.ip_ptr) {
			    pcrft->cr_hom[i].ip_ptr = NULL;
			    biton(pcrft->cr_chng,BIT_HOMCHAN+i);
			}
		    if (pcrft->cr_auto.ip_ptr == prmv.ip_ptr) {
			pcrft->cr_auto.ip_ptr = NULL;
			biton(pcrft->cr_chng,BIT_AUTOFFWD);
		    }
		    if (pcrft->cr_lhit.ip_ptr == prmv.ip_ptr)
			pcrft->cr_lhit.ip_ptr = NULL;
		    break;

		case 'T':
		    ptorp = puniv->uv_ptr.uv_torp;
		    if (ptorp->tp_lhit.ip_ptr == prmv.ip_ptr)
			ptorp->tp_lhit.ip_ptr = NULL;
		    if (ptorp->tp_fby.ip_ptr == prmv.ip_ptr) {
			/**** don't remove (hee hee hee)****/
			/****idxptr xrmv;		****/
			/****xrmv.ip_ptr = puniv;	****/
			/****remove(xrmv);		****/
			/****continue;			****/
			ptorp->tp_fby.ip_ptr = NULL;
		    }
		    if (ptorp->tp_aim.ip_ptr == prmv.ip_ptr) {
			if (ptorp->tp_fby.ip_ptr &&
			ptorp->tp_fby.ip_ptr->uv_type == 'P') {
			    pcrft = ptorp->tp_fby.ip_ptr->uv_ptr.uv_crft;
			    output(pcrft->cr_lgn,'B',0,0);
			    setrpt(pcrft);
			    rpt(pcrft,"Torpedo missed - target went away");
			    fnshrpt(pcrft,1);
			}
			remove(ptorp->tp_univ);
			continue;
		    }
		    break;
	    }
	}

	/* relocate rather than remove aliens */
	if (prmv.ip_ptr->uv_type == 'A') {
	    paln = prmv.ip_ptr->uv_ptr.uv_aln;

	    /* zero out velocity, thrust, attack variables, subsystem damage */
	    vinit(paln->al_vel);
	    vinit(paln->al_thr);
	    paln->al_dly = 0;
	    paln->al_lhit.ip_ptr = NULL;
	    paln->al_atck.ip_ptr = NULL;
	    paln->al_aeval = 0;
	    for (i=0;i<MSYS;++i)
		paln->al_sys[i].s_dmg = 0;
	    prmv.ip_ptr->uv_mass = 0;

	    /* relocate */
	    for (i=0;i<3;++i)
		paln->al_pstn[i] = MUL(paln->al_pstn[i],-2.);
	    vchngd(paln->al_univ.ip_ptr);

	/* actually remove the object */
	} else {
	    if (prmv.ip_ptr->uv_type == 'T')
		binit((char *)prmv.ip_ptr->uv_ptr.uv_torp,sizeof(struct torp));
	    else if (prmv.ip_ptr->uv_type == 'P')
		binit((char *)prmv.ip_ptr->uv_ptr.uv_crft,sizeof(struct crft));
	    vrmv(prmv.ip_ptr);
	    binit((char *)prmv.ip_ptr,sizeof(*prmv.ip_ptr));
	}


#ifdef DEBUG
	VDBG("remove return\n");
#endif
}
