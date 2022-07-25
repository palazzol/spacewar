/*
 * Spacewar - inflict damage (proximity based)
 *	      reports damage to inflictee
 *	      reports to and credits inflictor
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
#include "flds.h"
#include "build.h"
#include "obj.h"
#include "torp.h"

VOID damage(patck,ptrgt,rng,dmg,msg)
struct universe *patck,*ptrgt;
double rng,dmg;
char *msg;
{
	register struct universe *puniv;
	struct universe *plhit,*prvatck;
	register struct sys *psys,*psysdmg;
	struct crft *pcrft;
	struct aln *paln;
	struct torp *ptorp;
	double vdist(),trgtdist;
	int inflict,savinflict,totinflict=0,sh,ishulldmg,sysdmg;
	char buf[80+1];
	dsplcmnt tmpdspl;

#ifdef DEBUG
	DBG("damage(#%3d,.,%f,%f,%s)\n",patck-univlst,rng,dmg,msg);
#endif

	/* is it from hull damage; last hit by */
	ishulldmg = (SUB(dmg,DIV(1.,690.)) <= 0.);
	plhit = patck;
	if (plhit && plhit->uv_type == 'T')
	    plhit = plhit->uv_ptr.uv_torp->tp_fby.ip_ptr;

	/******************************************/
	/* find all damageable stuff within range */
	/******************************************/
	for (puniv=univlst+MAXUNIVERSE;puniv-- > univlst;) {
	    if (!puniv->uv_type) continue;
	    if (puniv->uv_type == 'O') continue;

	    tmpdspl = vdisp(puniv,ptrgt,'d');
	    /*if (SUB(vdist(puniv->uv_pstn,ptrgt->uv_pstn),rng) > 0.) continue;*/
	    if (SUB(tmpdspl.dst,rng) > 0.) continue;

	    /*******************************/
	    /* figure shielding protection */
	    /*******************************/
	    psys = NULL;
	    pcrft = NULL;
	    paln = NULL;
	    ptorp = NULL;
	    switch(puniv->uv_type) {
		case 'T':
		    if (patck == puniv) continue;
		    ptorp = puniv->uv_ptr.uv_torp;
		    sh = 30;
		    if (plhit) ptorp->tp_lhit.ip_ptr = plhit;
		    break;
		case 'A':
		    paln = puniv->uv_ptr.uv_aln;
		    psys = paln->al_sys;
		    sh = 3;
		    if (plhit) {
			prvatck = paln->al_lhit.ip_ptr;
			paln->al_lhit.ip_ptr = plhit;
			if (plhit->uv_type != 'P' ||
			!plhit->uv_ptr.uv_crft->cr_dock.ip_ptr) {
			    paln->al_dly = 2;
			    paln->al_atck.ip_ptr = plhit;
			}
		    }
		    break;
		case 'P':
		    pcrft = puniv->uv_ptr.uv_crft;
		    psys = pcrft->cr_sys;
		    sh = 3;
		    if (plhit) pcrft->cr_lhit.ip_ptr = plhit;
		    break;
	    }
	    if (psys && psys[SHIELDS].s_cap)
		if (!ishulldmg) {/* no shields if caused by hull dmg */
		    int tmpsh;
		    tmpsh = psys[SHIELDS].s_lvl;
		    tmpsh *= psys[SHIELDS].s_pct;
		    tmpsh /= 100;
		    tmpsh *= psys[SHIELDS].s_cap;
		    tmpsh /= 100;
		    sh += tmpsh;
		}

	    /******************/
	    /* inflict damage */
	    /******************/
	    /* compute damage to inflict */
	    if (patck) {
		tmpdspl = vdisp(patck,puniv,'d');
		trgtdist = /*vdist(patck->uv_pstn,puniv->uv_pstn)*/ tmpdspl.dst;
	    } else
		trgtdist = 1.;
	    if (SUB(trgtdist,1.) < 0) trgtdist = 1.;
	    inflict = INT(DIV(MUL(125000.,dmg),trgtdist)) / (sh*sh);
	    if (inflict > 1000) inflict = 1000;
	    totinflict += savinflict = inflict;

	    /* apply damage */
	    if (psys) /* non-torp */
		while (inflict > 0) {

		    /* randomly choose an existing system */
		    psysdmg = psys + RANDOM(MSYS);
		    if (!psysdmg->s_cap) continue;

		    /* 35<=sysdmg<80 */
		    sysdmg = RANDOM(45) + 35;
		    if (sysdmg > inflict) sysdmg = inflict;
		    inflict -= sysdmg;

		    /* actual damage affected by subsystem ease of damage */
		    psysdmg->s_dmg += (sysdmg*psysdmg->s_edmg)/100;
		    if (psysdmg->s_dmg > 100 || psysdmg->s_dmg < 0)
			psysdmg->s_dmg = 100;

		    if (pcrft)
			biton(pcrft->cr_chng,BIT_SDMG+
			(psysdmg-psys)*flds[FLD_SDMG].f_grpw);
		    if (psys == psysdmg)
			puniv->uv_mass = psysdmg->s_dmg;
		}

	    else {/* torp */
		if (ptorp->tp_dmg + inflict >= 100)
		    ptorp->tp_dmg = 100;
		else
		    ptorp->tp_dmg += inflict;
		puniv->uv_mass = ptorp->tp_dmg;
	    }

	    /* report damage inflicted to inflictee */
	    if (pcrft) {
		output(pcrft->cr_lgn,'B',0,0);
		setrpt(pcrft);
		sprintf(buf,"%s damage: %d",msg,savinflict);
		rpt(pcrft,buf);

		/* report direction if not from hull damage and */
		/* not torpedo or torpedo fired-by still around */
		if (!ishulldmg && plhit) {
		    double tmpvec[3];
		    vcopy(pcrft->cr_lhpstn,plhit->uv_pstn);
		    /*vdiff(plhit->uv_pstn,puniv->uv_pstn,tmpvec);*/
		    tmpdspl = vdisp(plhit,puniv,'v');
		    rttosp(/*tmpvec*/tmpdspl.vec,tmpvec);
		    sprintf(buf,"Attacker @%ld %.1f %.1f",INT(tmpvec[0]),
		    DIV(tmpvec[1],DEGTORAD),DIV(tmpvec[2],DEGTORAD));
		    rpt(pcrft,buf);
		}
		fnshrpt(pcrft,1);

	    /* alien attack evaluation */
	    } else if (paln) {

		/* acccumulate damage caused by attacker */
		if (plhit) {
			if (plhit != prvatck) paln->al_aeval = 0;
			paln->al_aeval += savinflict;
		}
	    }
	}

	/* report to and credit inflictor */
	if (!ishulldmg && plhit && plhit->uv_type == 'P') {
	    pcrft = plhit->uv_ptr.uv_crft;
	    pcrft->cr_pnts += totinflict/10;
	    output(pcrft->cr_lgn,'B',0,0);
	    setrpt(pcrft);
	    sprintf(buf,"%s damage inflicted: %d",msg,totinflict);
	    rpt(pcrft,buf);
	    fnshrpt(pcrft,1);
	}
#ifdef DEBUG
	VDBG("damage return\n");
#endif
}
