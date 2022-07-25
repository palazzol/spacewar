/*
 * Spacewar - update energy subsystems (solar, antm, dilith, shield, warp)
 *	      works for both aliens and crafts
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
#include "obj.h"
#include "aln.h"
#include "crft.h"
#include "build.h"
#include "flds.h"
#include "torp.h"

static long prtlvl(),strlvl();

VOID updsys()
{
	register struct universe *puniv;
	register struct obj *pobj;
	register struct sys *psys,*psh;
	char *pchng;
	double vdist(),ftmp;
	long i,elvl;
	dsplcmnt tmpdspl;

#ifdef DEBUG
	DBG("updsys()\n");
#endif

	for (puniv=univlst+MAXUNIVERSE;puniv-- > univlst;) {

	    /**********/
	    /* set up */
	    /**********/
	    switch(puniv->uv_type) {
		case 'A':
		    psys = puniv->uv_ptr.uv_aln->al_sys;
		    pchng = NULL;
		    break;
		case 'P':
		    psys = puniv->uv_ptr.uv_crft->cr_sys;
		    pchng = puniv->uv_ptr.uv_crft->cr_chng;
		    break;
		default:
		    continue;
	    }
	    elvl = 0L;

	    /****************/
	    /* solar energy */
	    /****************/
	    if (psys[SOLARE].s_cap) {
		for (i=0,pobj=objlst+MAXOBJ;pobj-- > objlst;) {
		    if (pobj->oj_rep != '*') continue;
		    /*ftmp = vdist(univlst[pobj-objlst].uv_pstn,puniv->uv_pstn);*/
		    tmpdspl = vdisp(&univlst[pobj-objlst],puniv,'d');
		    ftmp = tmpdspl.dst;
		    i += INT(DIV(1.e9,SQUARE(ftmp)));
		}
		elvl += strlvl(psys,SOLARE,prtlvl(psys,SOLARE,i),pchng);
	    }

	    /*********************/
	    /* antimatter energy */
	    /*********************/
	    if (i=psys[ANTME].s_cap)
		elvl += strlvl(psys,ANTME,prtlvl(psys,ANTME,i),pchng);

	    /* dilithium storage - increase from solar and antimatter	*/
	    /* no dilithium ==> energy flow but no storage		*/
	    /* shields consume energy - reduce shields if not enough	*/
	    /* remaining gets stored into dilithium (if present)	*/
	    if (psys[DILITH].s_cap)
		elvl += psys[DILITH].s_lvl;
	    if ((psh=psys+SHIELDS)->s_cap) {

		/* alien shields to 75+hulldamage+? if */
		/* attacking otherwise 25+hulldamage   */
		if (!pchng) {
		    psh->s_lvl = 25 + puniv->uv_mass;
		    if (puniv->uv_ptr.uv_aln->al_atck.ip_ptr)
			psh->s_lvl += 50 + puniv->uv_ptr.uv_aln->al_aeval/20;
		    if (psh->s_lvl > 100) psh->s_lvl = 100;
		}

		/* energy required to maintain shields (gag awful !@#$%^&) */
		/* capacity reduced by %level and %subsystem, again reduced*/
		/* by some type of squaring of %damage; keep reducing lvl  */
		/* until energy required does not exceed energy available  */
		for (;;) {
		    i = psh->s_cap;
		    i *= psh->s_pct;
		    i *= psh->s_lvl;
		    i /= (100L*100L);
		    i *= 10201L;
		    i /= 10201L + (psh->s_dmg-202L)*psh->s_dmg;
		    if (i <= elvl) break;
		    psh->s_lvl -= 1;
		    if (pchng)
			biton(pchng,BIT_SLEVEL+SHIELDS*flds[FLD_SLEVEL].f_grpw);
		}
		elvl -= i;
	    }
	    if (psys[DILITH].s_cap)
		strlvl(psys,DILITH,elvl,pchng);

	    /***************/
	    /* warp energy */
	    /***************/
	    if (psys[WARP].s_cap) {
		int savpct=psys[WARP].s_pct,savdmg=psys[WARP].s_dmg;
		i = psys[WARP].s_cap;
		i *= psys[WARP].s_pct;
		i /= 100L*100L*2L;
		i += psys[WARP].s_lvl;

		/* maximum level (capacity) unaffected by %lvl or %dmg */
		psys[WARP].s_pct = 100;
		psys[WARP].s_dmg = 0;
		strlvl(psys,WARP,i,pchng);
		psys[WARP].s_pct = savpct;
		psys[WARP].s_dmg = savdmg;
	    }
	}

#ifdef DEBUG
	VDBG("updsys return\n");
#endif
}

/* pro-rate level for %, %dmg */
static long prtlvl(psys,isys,lvl)
register struct sys *psys;
int isys;
long lvl;
{
	psys += isys;
	if (psys->s_cap) {
	    lvl = (lvl*psys->s_pct)/100;
	    lvl = (lvl*(100-psys->s_dmg))/100;
	} else
	    lvl = 0L;
	return(lvl);
}

/* limit lvl to capacity (capacity reduced by %dmg) */
/* store new level and indicate if changed	    */
static long strlvl(psys,isys,lvl,pchng)
struct sys *psys;
int isys;
long lvl;
char *pchng;
{
	long maxcap;

	psys += isys;
	if (psys->s_cap) {
	    maxcap = psys->s_cap;
	    maxcap *= psys->s_pct;
	    maxcap *= 100L-psys->s_dmg;
	    maxcap /= 100L*100L;
	    if (lvl > maxcap)
		lvl = maxcap;
	    if (lvl != psys->s_lvl) {
		psys->s_lvl = lvl;
		if (pchng)
		    biton(pchng,BIT_SLEVEL+isys*flds[FLD_SLEVEL].f_grpw);
	    }
	} else
	    lvl = 0L;
	return(lvl);
}
