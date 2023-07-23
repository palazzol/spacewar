/*
 * Spacewar - update damage from hull damage and do damage control
 *	      hull %dmg>=60 more damage, >=80 destroys
 *	      works for both crfts and alns
 *	      works for torp dmg >= 100
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
#include "build.h"
#include "aln.h"
#include "flds.h"
#include "obj.h"
#include "torp.h"

VOID upddmg()
{
	struct universe *puniv;
	register struct sys *psys,*pdmgcon;
	register struct crft *pcrft,*phit;
	struct aln *paln;
	struct torp *ptorp;
	struct login *plogin;
	int i;

#ifdef DEBUG
	DBG("upddmg()\n");
#endif

	for (puniv=univlst+MAXUNIVERSE;puniv-- > univlst;) {

	    /**********/
	    /* set up */
	    /**********/
	    switch(puniv->uv_type) {
		case 'A':
		    paln = puniv->uv_ptr.uv_aln;
		    if (paln->al_lhit.ip_ptr &&
		    paln->al_lhit.ip_ptr->uv_type == 'P')
			phit = paln->al_lhit.ip_ptr->uv_ptr.uv_crft;
		    else
			phit = NULL;
		    psys = paln->al_sys;
		    pcrft = NULL;
		    break;
		case 'P':
		    pcrft = puniv->uv_ptr.uv_crft;
		    if (pcrft->cr_lhit.ip_ptr &&
		    pcrft->cr_lhit.ip_ptr->uv_type == 'P')
			phit = pcrft->cr_lhit.ip_ptr->uv_ptr.uv_crft;
		    else
			phit = NULL;
		    psys = pcrft->cr_sys;
		    paln = NULL;
		    break;
		case 'T':
		    ptorp = puniv->uv_ptr.uv_torp;
		    if (ptorp->tp_dmg >= 100) {

			/* hit by a player, credit and report destruction */
			if (ptorp->tp_lhit.ip_ptr &&
			ptorp->tp_lhit.ip_ptr->uv_type == 'P') {
			    phit = ptorp->tp_lhit.ip_ptr->uv_ptr.uv_crft;
			    phit->cr_pnts += 200;
			    output(phit->cr_lgn,'B',0,0);
			    setrpt(phit);
			    rpt(phit,"Torpedo destroyed by you");
			    fnshrpt(phit,1);
			}

			/* report destroyed torpedo to firing player */
			if (ptorp->tp_fby.ip_ptr &&
			ptorp->tp_fby.ip_ptr->uv_type == 'P') {
			    phit = ptorp->tp_fby.ip_ptr->uv_ptr.uv_crft;
			    output(phit->cr_lgn,'B',0,0);
			    setrpt(phit);
			    rpt(phit,"Your torpedo destroyed");
			    fnshrpt(phit,1);
			}

			/* remove torpedo */
			removeobj(ptorp->tp_univ);
		    }
		    continue;
		default:
		    continue;
	    }

	    /*******************************/
	    /* hull damage >= 80% destroys */
	    /*******************************/
	    if (psys[HULL].s_dmg >= 80) {

		/* alien */
		if (paln) {

		    /* hit by another player, credit and report destruction */
		    if (phit) {
			phit->cr_kill += 1;
			output(phit->cr_lgn,'B',0,0);
			setrpt(phit);
			rpt(phit,"Alien destroyed by you");
			fnshrpt(phit,1);
		    }
		    removeobj(paln->al_univ);

		/* craft */
		} else {
		    pcrft->cr_dock.ip_ptr = NULL; /* assure destruction */
		    pcrft->cr_sens[1] = 1; /* assure destruction */
		    unplay(plogin=pcrft->cr_lgn); /* *pcrft gets zeroed out */
		    output(plogin,'E',0,0);
		    output(plogin,'B',0,0);
		    output(plogin,'C',0,"You are destroyed (hull broke up)\n");
		    output(plogin,'C',0,PROMPT);
		    output(plogin,0,0,0);
		}

	    /**************************************************************/
	    /* hull damage >= 60% causes gradual damage to all subsystems */
	    /**************************************************************/
	    } else if (psys[HULL].s_dmg >= 60) 
		damage(NULL,puniv,1.,DIV(FLOAT(RANDOM(100)),69000.),
		"Hull damage >=60%");

	    /******************/
	    /* damage control */
	    /******************/
	    pdmgcon = psys + DMGCON;

	    /* only if damage control subsystem is present */
	    if (pdmgcon->s_cap) {

		/* not fixing a specific system */
		if (!pdmgcon->s_lvl) {

		    /* reduce damage by 1% for randomly chosen subsystems */
		    for (i=pdmgcon->s_pct*(100-pdmgcon->s_dmg)/1500+1;i-- > 0;) {
			pdmgcon = psys + RANDOM(MSYS);
			if (pdmgcon->s_dmg <= 0 || pdmgcon->s_dmg == 45)
			    continue;  /* already as low as it can go */
			pdmgcon->s_dmg -= 1;
			if (pcrft)
			    biton(pcrft->cr_chng,
			    BIT_SDMG+(pdmgcon-psys)*flds[FLD_SDMG].f_grpw);
			if (pdmgcon == psys)
			    puniv->uv_mass = pdmgcon->s_dmg;
		    }
		
		/* fixing a specific system */
		} else {
		    i = pdmgcon->s_pct*(100-pdmgcon->s_dmg)/1500 + 1;
		    i = (i*2)/3; /* diminishing returns */
		    pdmgcon = psys + pdmgcon->s_lvl - 1;
		    if (pdmgcon->s_cap) { /* only if subsystem is present */
		        if (pdmgcon->s_dmg > 0 && pdmgcon->s_dmg != 45 && i) {
			    pdmgcon->s_dmg -= i;
			    biton(pcrft->cr_chng,
			    BIT_SDMG+(pdmgcon-psys)*flds[FLD_SDMG].f_grpw);
			    if (pdmgcon->s_dmg < 0)
				pdmgcon->s_dmg = 0;
			    if (pdmgcon->s_dmg < 45 && pdmgcon->s_dmg+i > 45)
				pdmgcon->s_dmg = 45;
			} else { /* can't be fixed any further */
			    psys[DMGCON].s_lvl = NULL;
			    biton(pcrft->cr_chng,
			    BIT_SLEVEL+DMGCON*flds[FLD_SLEVEL].f_grpw);
			}
			if (pdmgcon == psys)
			    puniv->uv_mass = pdmgcon->s_dmg;
		    }
		}
	    }
	}

#ifdef DEBUG
	VDBG("upddmg return\n");
#endif
}
