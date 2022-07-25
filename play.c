/*
 * Spacewar - routine to execute user playing commands
 *
 * Copyright 1985 obo Systems, Inc.
 * Copyright 1985 Dan Rosenblatt
 */

#ifndef VMS
#include <sys/types.h>
#include <dbm.h>
#else /* BSD SYSIII SYSV */
#include <types.h>
#include "dbm.h"
#endif /* VMS */
#include "spacewar.h"
#include "universe.h"
#include "login.h"
#include "sys.h"
#include "crft.h"
#include "flds.h"
#include "build.h"
#include "aln.h"
#include "obj.h"
#include "torp.h"
#include "ucmd.h"

extern VOID fixdir();
extern double vdist(),vlen();
static int okdir();
static VOID fixaf();

static struct {
	char	*pc_cmd;
	char	pc_mincmdl;
} pcmds[] = {
/* 0 */	{"erase",1},	{"angle",2},	{"right",2},	{"left",3},
/* 4 */	{"up",2},	{"down",3},	{"vdist",1},	{"home",1},
/* 8 */	{"unhome",2},	{"faceforward",2},{"autopilot",2},{"shields",2},
/* 12 */{"stopthrust",2},{"thrust",2},	{"warp",2},	{"damage",3},
/* 16 */{"leave",3},	{"dock",3},	{"radio",2},	{"who",2},
/* 20 */{"report",3},	{"phasers",1},	{"torpedo",2},	{"fix",2},
/* 24 */{"-",1},	{"sensors",2},	{"lockon",2},	{"redraw",3}
};

VOID play(plogin)
register struct login *plogin;
{
	int i,j;
	long l,m;
	register struct crft *pcrft;
	struct torp *ptorp;
	register struct universe *puniv;
	struct login *plgn;
	char buf[128];
	double ftmp,tmpvec[3],tmpvec2[3];
	struct ucmdkey getuckey;
	datum dbmkey,dbmdata;
	char ucargs[9][20+1],inuc=0;
	dsplcmnt tmpdspl;

#ifdef DEBUG
	DBG("play(#%d/%s)\n",plogin-loginlst,plogin->ln_name);
#endif

	/* if not yet placed into the universe, try doing so */
	if (!plogin->ln_play.ip_ptr) {
	    if (plinit(plogin)) {
		pcrft = plogin->ln_play.ip_ptr->uv_ptr.uv_crft;
		sprintf(plogin->ln_input,"Wlcm abrd the %s captain %s",
		plogin->ln_crft,plogin->ln_name);
		i = 0; /* force erase, not redraw */
		goto erase;
	    }
#ifdef DEBUG
	    VDBG("play return\n");
#endif
	    return;
	}
	pcrft = plogin->ln_play.ip_ptr->uv_ptr.uv_crft;

	/* move last command indicator */
douc:	output(plogin,'H',(pcrft->cr_lcmd<<8)|FLD_LSTCMD," ");
	if (++pcrft->cr_lcmd >= flds[FLD_LSTCMD].f_maxg)
	    pcrft->cr_lcmd = 0;
	output(plogin,'H',(pcrft->cr_lcmd<<8)|FLD_LSTCMD,"#");

	/* find command */
	for (i=sizeof(pcmds)/sizeof(pcmds[0]);i-- > 0;)
	    if (!strncmp(plogin->ln_input,pcmds[i].pc_cmd,pcmds[i].pc_mincmdl))
		break;
	if (i < 0) {
badinp:	    sprintf(buf,"??? %s",plogin->ln_input);
badinp2:    output(plogin,'H',(pcrft->cr_lcmd<<8)|FLD_COMMAND,buf);
	    inuc = 0;
	    goto done;
	}


	switch(i) {

	    /*********/
	    /* erase */
	    /*********/
	    case 0:
erase:		/* erase screen, redraw background */
		output(plogin,'E',0,0);
redraw:		background(pcrft);

		/* indicate all fields changed so they get re-displayed */
		for (j=0;j < sizeof(pcrft->cr_chng);++j)
		    pcrft->cr_chng[j] = ~0;
		pcrft->cr_scrn[0][0] = NULL;

		if (i == 0) { /* redraw instead of erase */

		    /* reset last report and command markers */
		    pcrft->cr_lrpt = 0;
		    pcrft->cr_lcmd = 0;

		    /* restore last command marker */
		    output(plogin,'H',(pcrft->cr_lcmd<<8)|FLD_LSTCMD,"#");
		}

		break;

	    /*********/
	    /* angle */
	    /*********/
	    case 1:
		if (sscanf(plogin->ln_input,"%*s %lf",&ftmp) != 1)
		    goto badinp;

		/* must be 0<ang<=180 (or 360 if privileged) */
		if (ftmp <= 0. || SUB(ftmp,360.) > 0. ||
		(SUB(ftmp,180.) > 0 && !pcrft->cr_plvl))
		    goto badinp;

		pcrft->cr_vang = MUL(ftmp,DEGTORAD);
		biton(pcrft->cr_chng,BIT_VANGL);

		/* recompute viewing distance if not privileged */
		if (!pcrft->cr_plvl) {
		    pcrft->cr_vdst =
		    INT(DIV(VANGVDST,SQUARE(pcrft->cr_vang)));
		    biton(pcrft->cr_chng,BIT_VDIST);
		}
		break;

	    /*********/
	    /* right */
	    /*********/
	    case 2:
		if (!okdir(plogin,pcrft,&ftmp)) goto badinp;
		pcrft->cr_dir[1] = SUB(pcrft->cr_dir[1],ftmp);
		biton(pcrft->cr_chng,BIT_DIR1);
		fixdir(pcrft);
		fixaf(pcrft);
		break;

	    /********/
	    /* left */
	    /********/
	    case 3:
		if (!okdir(plogin,pcrft,&ftmp)) goto badinp;
		pcrft->cr_dir[1] = ADD(pcrft->cr_dir[1],ftmp);
		biton(pcrft->cr_chng,BIT_DIR1);
		fixdir(pcrft);
		fixaf(pcrft);
		break;

	    /******/
	    /* up */
	    /******/
	    case 4:
		if (!okdir(plogin,pcrft,&ftmp)) goto badinp;
		pcrft->cr_dir[2] = SUB(pcrft->cr_dir[2],ftmp);
		biton(pcrft->cr_chng,BIT_DIR2);
		fixdir(pcrft);
		fixaf(pcrft);
		break;

	    /********/
	    /* down */
	    /********/
	    case 5:
		if (!okdir(plogin,pcrft,&ftmp)) goto badinp;
		pcrft->cr_dir[2] = ADD(pcrft->cr_dir[2],ftmp);
		biton(pcrft->cr_chng,BIT_DIR2);
		fixdir(pcrft);
		fixaf(pcrft);
		break;

	    /**********************/
	    /* vdist (privileged) */
	    /**********************/
	    case 6:
		if (!pcrft->cr_plvl)
		    goto badinp;
		if (sscanf(plogin->ln_input,"%*s %ld",&pcrft->cr_vdst) != 1)
		    goto badinp;
		biton(pcrft->cr_chng,BIT_VDIST);
		break;

	    /********/
	    /* home */
	    /********/
	    case 7:
		if (sscanf(plogin->ln_input,"%*s %d",&i) != 1)
		    goto badinp;
		if (i < 1 || i > MHOM)
		    goto badinp;
		i -= 1;

		/* must be autopiloted on something */
		if (!pcrft->cr_auto.ip_ptr) {
		    sprintf(buf,"%.22s - not autopiloted",plogin->ln_input);
		    goto badinp2;
		}

		pcrft->cr_hom[i].ip_ptr = pcrft->cr_auto.ip_ptr;
		/*pcrft->cr_hdst[i] = INT(vdist(pcrft->cr_pstn,
		pcrft->cr_auto.ip_ptr->uv_pstn));*/
		tmpdspl = vdisp(pcrft->cr_univ.ip_ptr,pcrft->cr_auto.ip_ptr,'d');
		pcrft->cr_hdst[i] = INT(tmpdspl.dst);
		biton(pcrft->cr_chng,BIT_HOMCHAN+i);
		break;

	    /**********/
	    /* unhome */
	    /**********/
	    case 8:
		if (sscanf(plogin->ln_input,"%*s %d",&i) != 1)
		    goto badinp;
		if (i < 1 || i > MHOM)
		    goto badinp;
		i -= 1;

		/* must be homed in */
		if (!pcrft->cr_hom[i].ip_ptr) {
		    sprintf(buf,"%.25s - not homed in",plogin->ln_input);
		    goto badinp2;
		}

		pcrft->cr_hom[i].ip_ptr = NULL;
		pcrft->cr_hdst[i] = NULL;
		biton(pcrft->cr_chng,BIT_HOMCHAN+i);
		break;

	    /***************/
	    /* faceforward */
	    /***************/
	    case 9:
		if (vlen(pcrft->cr_vel) == 0.) {
		    sprintf(buf,"%.27s - not moving",plogin->ln_input);
		    goto badinp2;
		}
		pcrft->cr_ffwd = 1;
		pcrft->cr_auto.ip_ptr = NULL;
		rttosp(pcrft->cr_vel,pcrft->cr_dir);
		biton(pcrft->cr_chng,BIT_AUTOFFWD);
		biton(pcrft->cr_chng,BIT_DIR1);
		biton(pcrft->cr_chng,BIT_DIR2);
		fixdir(pcrft);
		break;

	    /*************/
	    /* autopilot */
	    /*************/
	    case 10:
		if (sscanf(plogin->ln_input,"%*s%c",&i) != 1) /* null */
		    i = 0;
		else if (sscanf(plogin->ln_input,"%*s %d",&i) != 1)
		    goto badinp;

		/* to a specific homing channel; privileges allow */
		/* a negative # for absolute universe objects */
		if (i) {
		    if (i > MHOM || (i < 1 && !pcrft->cr_plvl))
			goto badinp;
		    if (i < 1) {
			if ((i=(-i-1)) >= MAXUNIVERSE || !univlst[i].uv_type)
			    goto badinp;
			else
			    pcrft->cr_auto.ip_ptr = univlst + i;
		    } else {
			i -= 1;
			if (!pcrft->cr_hom[i].ip_ptr) {
			    sprintf(buf,"%.25s - not homed in",plogin->ln_input);
			    goto badinp2;
			}
			pcrft->cr_auto.ip_ptr = pcrft->cr_hom[i].ip_ptr;
		    }

		    /* if docked, shift to docked object (because docking */
		    /* loses autos, docks, and torps but not homing)      */
		    puniv = pcrft->cr_auto.ip_ptr;
		    if (puniv->uv_type == 'P' &&
		    puniv->uv_ptr.uv_crft->cr_dock.ip_ptr)
			pcrft->cr_auto.ip_ptr =
			puniv->uv_ptr.uv_crft->cr_dock.ip_ptr;
		    /* but not to player's own craft */
		    if (pcrft->cr_auto.ip_ptr == plogin->ln_play.ip_ptr)
			pcrft->cr_auto.ip_ptr = NULL;

		/* to closest to center/closest object */
		} else {

doauto:		    /* use viewing distance and smaller of */
		    /* 5 degrees, half of viewing angle    */
		    ftmp = MUL(10.,DEGTORAD);
		    if (SUB(pcrft->cr_vang,ftmp) < 0.)
			ftmp = pcrft->cr_vang;
		    ftmp = DIV(ftmp,2.);
		    l = pcrft->cr_vdst;
		    pcrft->cr_auto.ip_ptr = NULL;

		    /* select based on closest to center/closest */
		    for (puniv = univlst+MAXUNIVERSE;puniv-- > univlst;) {
			if (!puniv->uv_type) continue;
			if (puniv == plogin->ln_play.ip_ptr) continue;
			/*vdiff(puniv->uv_pstn,pcrft->cr_pstn,tmpvec);*/
			tmpdspl = vdisp(puniv,pcrft->cr_univ.ip_ptr,'v');
			vecmul(/*tmpvec*/tmpdspl.vec,pcrft->cr_rmat,tmpvec);
			rttosp(tmpvec,tmpvec);
			m = INT(tmpvec[0]);
			if ((tmpvec[2] == ftmp && m <= l) ||
			(SUB(tmpvec[2],ftmp) <= 0. && m <= pcrft->cr_vdst)) {
			    pcrft->cr_auto.ip_ptr = puniv;
			    ftmp = tmpvec[2];
			    l = m;
			}
		    }
		}

		biton(pcrft->cr_chng,BIT_AUTOFFWD);
		if (!pcrft->cr_auto.ip_ptr) {
		    sprintf(buf,"%.24s - nothing there",plogin->ln_input);
		    goto badinp2;
		} else {
		    pcrft->cr_ffwd = NULL;
		    /*vdiff(pcrft->cr_auto.ip_ptr->uv_pstn,pcrft->cr_pstn,
		    pcrft->cr_dir);*/
		    tmpdspl = vdisp(pcrft->cr_auto.ip_ptr,pcrft->cr_univ.ip_ptr,'v');
		    rttosp(/*pcrft->cr_dir*/tmpdspl.vec,pcrft->cr_dir);
		    biton(pcrft->cr_chng,BIT_DIR1);
		    biton(pcrft->cr_chng,BIT_DIR2);
		    fixdir(pcrft);
		}
		break;

	    /***********/
	    /* shields */
	    /***********/
	    case 11:
		if (!pcrft->cr_sys[SHIELDS].s_cap) {
		    sprintf(buf,"%.27s - no shields",plogin->ln_input);
		    goto badinp2;
		}
		if (sscanf(plogin->ln_input,"%*s %d",&i) != 1)
		    goto badinp;

		/* 0<=arg<=100 */
		if (i < 0 || i > 100)
		    goto badinp;

		pcrft->cr_sys[SHIELDS].s_lvl = i;
		biton(pcrft->cr_chng,BIT_SLEVEL+SHIELDS*flds[FLD_SLEVEL].f_grpw);
		break;

	    /**************/
	    /* stopthrust */
	    /**************/
	    case 12:
		if (!pcrft->cr_sys[ROCKETS].s_cap) {
		    sprintf(buf,"%.27s - no rockets",plogin->ln_input);
		    goto badinp2;
		}
		if (vlen(pcrft->cr_thr) == 0.) {
		    sprintf(buf,"%.20s - no pending thrust",plogin->ln_input);
		    goto badinp2;
		}
		vinit(pcrft->cr_thr);
		break;

	    /**********/
	    /* thrust */
	    /**********/
	    case 13:
		if (!pcrft->cr_sys[ROCKETS].s_cap) {
		    sprintf(buf,"%.27s - no rockets",plogin->ln_input);
		    goto badinp2;
		}
		if (!pcrft->cr_sys[ROCKETS].s_lvl) {
		    sprintf(buf,"%.30s - no fuel",plogin->ln_input);
		    goto badinp2;
		}
		vinit(tmpvec);
		if ((i=sscanf(plogin->ln_input,"%*s %lf %lf %lf %lf%c",&ftmp,
		tmpvec+0,tmpvec+1,tmpvec+2,&j)) < 1 || i > 4)
		    goto badinp;

		/* first number is thrust in direction pointed	*/
		/* others are cartesian components		*/
		pcrft->cr_dir[0] = ftmp;
		sptort(pcrft->cr_dir,tmpvec2);
		for (i=0;i<3;++i)
		    pcrft->cr_thr[i] =
		    ADD(pcrft->cr_thr[i],ADD(tmpvec[i],tmpvec2[i]));

		/* turn off docking */
		if (puniv=pcrft->cr_dock.ip_ptr) {

		    /* fix other player's screen if docked with another craft */
		    if (puniv->uv_type == 'P')
			puniv->uv_ptr.uv_crft->cr_scrn[7][15] = '?';

		    /* place craft away from docked object */
		    /* according to direction of thrust    */
		    rttosp(pcrft->cr_thr,tmpvec);
		    tmpvec[0] =  ADD(MUL(FLOAT(puniv->uv_rad+1),2.),1.);
		    sptort(tmpvec,tmpvec);
		    for (i=0;i<3;++i)
			pcrft->cr_pstn[i] = ADD(pcrft->cr_pstn[i],tmpvec[i]);
		    vchngd(pcrft->cr_univ.ip_ptr);
		    pcrft->cr_dock.ip_ptr = NULL;
		    pcrft->cr_scrn[7][15] = '?';
		}

		break;

	    /********/
	    /* warp */
	    /********/
	    case 14:
		if (!pcrft->cr_sys[WARP].s_cap) {
		    sprintf(buf,"%.30s - no warp",plogin->ln_input);
		    goto badinp2;
		}
		if (sscanf(plogin->ln_input,"%*s %lf",&ftmp) != 1 || ftmp == 0.)
		    goto badinp;
		ftmp = MUL(ftmp,1000.);

		/* check energy required */
		l = 500L + 15L * INT(SQRT((ftmp < 0.) ? NEG(ftmp) : ftmp));
		if (l > pcrft->cr_sys[WARP].s_lvl) {
		    sprintf(buf,"%.18s - need at least %ld",plogin->ln_input,l);
		    goto badinp2;
		}

		/* reduce warp energy level */
		pcrft->cr_sys[WARP].s_lvl -= l;
		biton(pcrft->cr_chng,BIT_SLEVEL+WARP*flds[FLD_SLEVEL].f_grpw);

		/* new position; % and %dmg affect accuracy */
		pcrft->cr_dir[0] = ftmp;
		sptort(pcrft->cr_dir,tmpvec);
		ftmp = DIV(MUL(ftmp,FLOAT(100-pcrft->cr_sys[WARP].s_pct+
		pcrft->cr_sys[WARP].s_dmg)),100.);
		for (i=0;i<3;++i)
		    pcrft->cr_pstn[i] = ADD(pcrft->cr_pstn[i],
		    ADD(tmpvec[i],MUL(ftmp,DIV(FLOAT(RANDOM(200)-100),100.))));
		vchngd(pcrft->cr_univ.ip_ptr);
		biton(pcrft->cr_chng,BIT_PN1);
		biton(pcrft->cr_chng,BIT_PN2);
		biton(pcrft->cr_chng,BIT_PN3);

		/* turn off auto/dock for this and other crafts on this craft */
		if (pcrft->cr_dock.ip_ptr) {
		    pcrft->cr_dock.ip_ptr = NULL;
		    pcrft->cr_scrn[7][15] = '?';
		}
		if (pcrft->cr_auto.ip_ptr) {
		    pcrft->cr_auto.ip_ptr = NULL;
		    biton(pcrft->cr_chng,BIT_AUTOFFWD);
		}
		for (puniv=univlst+MAXUNIVERSE;puniv-- > univlst;) {
		    register struct crft *pcrft2;
		    if (puniv->uv_type != 'P') continue;
		    pcrft2 = puniv->uv_ptr.uv_crft;
		    if (pcrft2->cr_dock.ip_ptr == plogin->ln_play.ip_ptr) {
			pcrft2->cr_dock.ip_ptr = NULL;
			pcrft2->cr_scrn[7][15] = '?';
			pcrft->cr_scrn[7][15] = '?';
		    }
		    if (pcrft2->cr_auto.ip_ptr == plogin->ln_play.ip_ptr) {
			pcrft2->cr_auto.ip_ptr = NULL;
			biton(pcrft2->cr_chng,BIT_AUTOFFWD);
		    }
		}

		break;

	    /***********************/
	    /* damage (privileged) */
	    /***********************/
	    case 15:
		if (!pcrft->cr_plvl)
		    goto badinp;
		if (sscanf(plogin->ln_input,"%*s %d %d",&i,&j) != 2)
		    goto badinp;

		/* must be a valid, existing system */
		if (i < 0 || i >= MSYS || !pcrft->cr_sys[i].s_cap)
		    goto badinp;

		/* 0<=%dmg<=100 */
		if (j < 0 || j > 100)
		    goto badinp;

		pcrft->cr_sys[i].s_dmg = j;
		if (i == HULL)
		    plogin->ln_play.ip_ptr->uv_mass = j;
		biton(pcrft->cr_chng,BIT_SDMG+i*flds[FLD_SDMG].f_grpw);
		break;

	    /*********/
	    /* leave */
	    /*********/
	    case 16:
		/* only if privileged or nothing bad */
		/* nearby or docked with an object */
		if (pcrft->cr_plvl || pcrft->cr_sens[1] == 0 ||
		((puniv=pcrft->cr_dock.ip_ptr) && puniv->uv_type == 'O')) {
		    output(plogin,'H',(pcrft->cr_lcmd<<8)|FLD_COMMAND,
		    plogin->ln_input);
		    unplay(plogin);
		    output(plogin,'E',0,0);
		    output(plogin,'C',0,PROMPT);
		    output(plogin,0,0,0);
		    return;
		}
		sprintf(buf,"%.14s - not docked and BAD != 0",plogin->ln_input);
		goto badinp2;

	    /********/
	    /* dock */
	    /********/
	    case 17:
		/* find the closest, dock'able object */
		l = 10000L;
		pcrft->cr_dock.ip_ptr = NULL;
		for (puniv=univlst+MAXUNIVERSE;puniv-- > univlst;) {
		    double *pvel;
		    switch(puniv->uv_type) {
			case 0: continue;
			case 'P':
			    if (puniv == plogin->ln_play.ip_ptr)
				continue;
			    if (puniv->uv_ptr.uv_crft->cr_dock.ip_ptr)
				continue;
			    pvel = puniv->uv_ptr.uv_crft->cr_vel;
			    break;
			case 'T':
			    pvel = puniv->uv_ptr.uv_torp->tp_vel;
			    break;
			case 'A':
			    pvel = puniv->uv_ptr.uv_aln->al_vel;
			    break;
			case 'O':
			    if (puniv->uv_pctr == ' ' || puniv->uv_pctr == '*')
				continue;
			    pvel = puniv->uv_ptr.uv_obj->oj_vel;
			    break;
		    }
		    /*m = INT(vdist(pcrft->cr_pstn,puniv->uv_pstn));*/
		    tmpdspl = vdisp(pcrft->cr_univ.ip_ptr,puniv,'d');
		    m = INT(tmpdspl.dst);
		    if (m < l && m <= 5*(puniv->uv_rad+1)) {
			pcrft->cr_dock.ip_ptr = puniv;
			l = m;
			vcopy(tmpvec,pvel);
		    }
		}
		if (!pcrft->cr_dock.ip_ptr) {
		    sprintf(buf,"%.25s - too far away",plogin->ln_input);
		    goto badinp2;
		}
		puniv = pcrft->cr_dock.ip_ptr;

		/* must also be within range next movement iteration */
		for (i=0;i<3;++i)
		    tmpvec[i] = SUB(ADD(puniv->uv_pstn[i],tmpvec[i]),
		    ADD(pcrft->cr_pstn[i],pcrft->cr_vel[i]));
		if (INT(vlen(tmpvec)) > 5*(puniv->uv_rad+1)) {
		    sprintf(buf,"%.22s - moving too fast",plogin->ln_input);
		    pcrft->cr_dock.ip_ptr = NULL;
		    goto badinp2;
		}

		/* dock */
		pcrft->cr_scrn[7][15] = '?';
		vinit(pcrft->cr_thr);
		vcopy(pcrft->cr_pstn,puniv->uv_pstn);
		vchngd(pcrft->cr_univ.ip_ptr);
		biton(pcrft->cr_chng,BIT_PN1);
		biton(pcrft->cr_chng,BIT_PN2);
		biton(pcrft->cr_chng,BIT_PN3);
		output(plogin,0,0,0);

		/* fix autopilots, docks, torp aim, alien attack */
		for (puniv=univlst+MAXUNIVERSE;puniv-- > univlst;) {
		    register struct crft *pcrft2;
		    struct aln *paln;
		    switch(puniv->uv_type) {
			case 0: continue;
			case 'P':
			    pcrft2 = puniv->uv_ptr.uv_crft;
			    if (pcrft2->cr_auto.ip_ptr ==
			    plogin->ln_play.ip_ptr) {

				/* shift autopilot to item docked with */
				/* if its not an object and not itself */
				if (pcrft->cr_dock.ip_ptr->uv_type != 'O' &&
				pcrft->cr_dock.ip_ptr != puniv)
				    pcrft2->cr_auto.ip_ptr =
				    pcrft->cr_dock.ip_ptr;

				/* its an object or itself - lose autopilot */
				else
				    pcrft2->cr_auto.ip_ptr = NULL;
				biton(pcrft2->cr_chng,BIT_AUTOFFWD);
				pcrft2->cr_scrn[7][15] = '?';
			    }

			    if (pcrft2->cr_dock.ip_ptr ==
			    plogin->ln_play.ip_ptr) {

				/* shift dock to item docked */
				/* with if its an object  */
				if (pcrft->cr_dock.ip_ptr->uv_type == 'O')
				    pcrft2->cr_dock.ip_ptr =
				    pcrft->cr_dock.ip_ptr;
				else
				    pcrft2->cr_dock.ip_ptr = NULL;
				pcrft2->cr_scrn[7][15] = '?';
			    }

			    break;
			case 'T':
			    ptorp = puniv->uv_ptr.uv_torp;
			    if (ptorp->tp_aim.ip_ptr ==
			    plogin->ln_play.ip_ptr &&
			    pcrft->cr_dock.ip_ptr->uv_type == 'O' &&
			    ptorp->tp_fby.ip_ptr &&
			    ptorp->tp_fby.ip_ptr->uv_type == 'P') {
				pcrft2 = ptorp->tp_fby.ip_ptr->uv_ptr.uv_crft;
				output(pcrft2->cr_lgn,'B',0,0);
				setrpt(pcrft2);
				rpt(pcrft2,"Torpedo missed - target docked");
				fnshrpt(pcrft2,1);
				remove(ptorp->tp_univ);
			    }
			    break;
			case 'A':
			    paln = puniv->uv_ptr.uv_aln;
			    if (paln->al_atck.ip_ptr == plogin->ln_play.ip_ptr
			    && (pcrft->cr_dock.ip_ptr->uv_type == 'O' ||
			    pcrft->cr_dock.ip_ptr == puniv)) {
				paln->al_atck.ip_ptr = NULL;
			    }
			    break;
		    }
		}
		break;

	    /*********/
	    /* radio */
	    /*********/
	    case 18:
		if (sscanf(plogin->ln_input,"%*s %[^\n]",buf) != 1)
		    goto badinp;

		output(pcrft->cr_lgn,0,0,0);

		/* similar to autopilot */
		ftmp = MUL(10.,DEGTORAD);
		if (SUB(pcrft->cr_vang,ftmp) < 0.)
		    ftmp = pcrft->cr_vang;
		ftmp = DIV(ftmp,2.);
		for (puniv=univlst+MAXUNIVERSE;puniv-- > univlst;) {
		    if (puniv->uv_type != 'P') continue;
		    if (puniv->uv_ptr.uv_crft == pcrft) continue;
		    /*vdiff(puniv->uv_pstn,pcrft->cr_pstn,tmpvec);*/
		    tmpdspl = vdisp(puniv,pcrft->cr_univ.ip_ptr,'v');
		    vecmul(/*tmpvec*/tmpdspl.vec,pcrft->cr_rmat,tmpvec);
		    rttosp(tmpvec,tmpvec);
		    if (SUB(tmpvec[2],ftmp) <= 0.) {
			char buf2[40+1];
			plgn = puniv->uv_ptr.uv_crft->cr_lgn;
			output(plgn,'B',0,0);
			output(plgn,'H',FLD_RADIO,buf);
			/*vdiff(pcrft->cr_pstn,puniv->uv_pstn,tmpvec);*/
			tmpdspl = vdisp(pcrft->cr_univ.ip_ptr,puniv,'v');
			rttosp(/*tmpvec*/tmpdspl.vec,tmpvec);
			setrpt(puniv->uv_ptr.uv_crft);
			sprintf(buf2,"Radio @%ld %.1f %.1f",INT(tmpvec[0]),
			DIV(tmpvec[1],DEGTORAD),DIV(tmpvec[2],DEGTORAD));
			rpt(puniv->uv_ptr.uv_crft,buf2);
			fnshrpt(puniv->uv_ptr.uv_crft,1);
		    }
		}
		break;

	    /*******/
	    /* who */
	    /*******/
	    case 19:
		if (sscanf(plogin->ln_input,"%*s%c",&i) != 1) /* null */
		    i = 0;
		else if (sscanf(plogin->ln_input,"%*s %d",&i) != 1 || i < 0)
		    goto badinp;
		i *= flds[FLD_REPORT].f_maxg; /* skip display group count */

		setrpt(pcrft);
		*buf = NULL;
		for (plgn=loginlst+MAXLOGIN;plgn-- > loginlst;) {
		    if (!plgn->ln_play.ip_ptr) continue;
		    if (i-- > 0) continue;
		    if (strlen(buf) + strlen(plgn->ln_name) +
		    strlen(plgn->ln_crft) + 5 > flds[FLD_REPORT].f_len) {
			rpt(pcrft,buf);
			*buf = NULL;
		    }
		    sprintf(buf+strlen(buf),"%s/%s(%c) ",plgn->ln_name,
		    plgn->ln_crft,plgn->ln_play.ip_ptr->uv_pctr);
		}
		rpt(pcrft,buf);
		fnshrpt(pcrft,0);
		break;

	    /**********/
	    /* report */
	    /**********/
	    case 20:
		if (sscanf(plogin->ln_input,"%*s%c",&i) != 1) /* null */
		    i = 0;
		else if (sscanf(plogin->ln_input,"%*s %d",&i) != 1 || i < 0)
		    goto badinp;
		i *= flds[FLD_REPORT].f_maxg; /* skip display group count */

		setrpt(pcrft);

		/* almost identical to view() */
		for (puniv=univlst+MAXUNIVERSE;puniv-- > univlst;) {
		    if (!puniv->uv_type) continue;
		    if (puniv->uv_ptr.uv_crft == pcrft) continue;
		    /*vdiff(puniv->uv_pstn,pcrft->cr_pstn,tmpvec);*/
		    tmpdspl = vdisp(puniv,pcrft->cr_univ.ip_ptr,'v');
		    vecmul(/*tmpvec*/tmpdspl.vec,pcrft->cr_rmat,tmpvec);
		    rttosp(tmpvec,tmpvec);
		    ftmp = DIV(pcrft->cr_vang,2.);
		    if (SUB(ftmp,tmpvec[2]) >= 0. &&
		    (l=INT(tmpvec[0])) <= pcrft->cr_vdst) {
			if (i-- > 0) continue;
			tmpvec[0] = DIV(tmpvec[2],ftmp);
			tmpvec[2] = DIV(PI,2.);
			sptort(tmpvec,tmpvec);

			/* common characteristics */
			sprintf(buf,"%ld,%ld(%c)@%ld",-INT(MUL(tmpvec[1],7.)),
			INT(MUL(tmpvec[0],15.)),puniv->uv_pctr,l);

			/* individual characteristics */
			switch(puniv->uv_type) {
			    case 'O':
				sprintf(buf+strlen(buf)," r=%d m=%ld",
				puniv->uv_rad,puniv->uv_mass);
				break;
			    case 'P':
				plgn = puniv->uv_ptr.uv_crft->cr_lgn;
				sprintf(buf+strlen(buf)," %s/%s h%ld s%d",
				plgn->ln_name,plgn->ln_crft,puniv->uv_mass,
				puniv->uv_ptr.uv_crft->cr_sys[SHIELDS].s_lvl);
				break;
			    case 'T':
				sprintf(buf+strlen(buf)," h%ld",puniv->uv_mass);
				break;
			    case 'A':
				sprintf(buf+strlen(buf)," h%ld s%d",
				puniv->uv_mass,
				puniv->uv_ptr.uv_aln->al_sys[SHIELDS].s_lvl);
				break;
			}

			/* indicate auto'd, dock'd, or home'd */
			l = 0;
			if (pcrft->cr_auto.ip_ptr == puniv) {
			    if (!l) strcat(buf," (");
			    l = 1;
			    strcat(buf,"A");
			}
			if (pcrft->cr_dock.ip_ptr == puniv) {
			    if (!l) strcat(buf," (");
			    l = 1;
			    strcat(buf,"D");
			}
			for (j=0;j<MHOM;++j)
			    if (pcrft->cr_hom[j].ip_ptr == puniv) {
				if (!l) strcat(buf," (");
				l = 1;
				sprintf(buf+strlen(buf),"%d",j+1);
			    }
			if (l) strcat(buf,")");

			rpt(pcrft,buf);
		    }
		}
		fnshrpt(pcrft,0);
		break;

	    /***********/
	    /* phasers */
	    /***********/
	    case 21:
		if (!pcrft->cr_sys[PHASERS].s_cap) {
		    sprintf(buf,"%.27s - no phasers",plogin->ln_input);
		    goto badinp2;
		}
		if (sscanf(plogin->ln_input,"%*s %d",&i) != 1)
		    goto badinp;
		
		/* 1<=arg<=1000 */
		if (i < 1 || i > 1000)
		    goto badinp;

		/* check if conditions allow phasers */
		if (pcrft->cr_dock.ip_ptr) {
		    sprintf(buf,"%.21s - not while docked",plogin->ln_input);
		    goto badinp2;
		}
		puniv = pcrft->cr_auto.ip_ptr;
		if (!puniv || puniv->uv_type == 'O') {
		    sprintf(buf,"%.19s - nothing to fire at",plogin->ln_input);
		    goto badinp2;
		}
		if (inuc) {
		    sprintf(buf,"%.24s - not in '-cmd'",plogin->ln_input);
		    goto badinp2;
		}

		/* must have enough energy for phaser blast */
		if (!pcrft->cr_sys[DILITH].s_cap ||
		pcrft->cr_sys[DILITH].s_lvl < i) {
		    sprintf(buf,"%.20s - not enough energy",plogin->ln_input);
		    goto badinp2;
		}

		/* phasers must not be damaged too much */
		if (pcrft->cr_sys[PHASERS].s_dmg >= 60) {
		    sprintf(buf,"%.22s - too much damage",plogin->ln_input);
		    goto badinp2;
		}

		/* use the requested energy */
		pcrft->cr_sys[DILITH].s_lvl -= i;
		biton(pcrft->cr_chng,BIT_SLEVEL+DILITH*flds[FLD_SLEVEL].f_grpw);

		/* compute damage */
		l = i;
		l *= pcrft->cr_sys[PHASERS].s_pct;
		l /= 100;
		l *= (100-pcrft->cr_sys[PHASERS].s_dmg);
		l /= 100;
		l *= pcrft->cr_sys[PHASERS].s_cap;
		l /= 100;

		/* finally, do it */
		output(pcrft->cr_lgn,0,0,0); /* flush buffering */
		damage(plogin->ln_play.ip_ptr,puniv,1.,FLOAT(l),
		"Phaser attack");

		break;

	    /***********/
	    /* torpedo */
	    /***********/
	    case 22:
		if (!pcrft->cr_sys[TORPS].s_cap ||
		pcrft->cr_sys[TORPS].s_lvl < 1) {
		    sprintf(buf,"%.25s - no torpedoes",plogin->ln_input);
		    goto badinp2;
		}

		/* check if conditions allow torpedoes */
		if (pcrft->cr_dock.ip_ptr) {
		    sprintf(buf,"%.21s - not while docked",plogin->ln_input);
		    goto badinp2;
		}
		puniv = pcrft->cr_auto.ip_ptr;
		if (!puniv || puniv->uv_type == 'O') {
		    sprintf(buf,"%.19s - nothing to fire at",plogin->ln_input);
		    goto badinp2;
		}
		if (inuc) {
		    sprintf(buf,"%.24s - not in '-cmd'",plogin->ln_input);
		    goto badinp2;
		}

		/* torpedo tubes must not be damaged too much */
		if (pcrft->cr_sys[TORPS].s_dmg >= 60) {
		    sprintf(buf,"%.22s - too much damage",plogin->ln_input);
		    goto badinp2;
		}

		/* search for an open slot */
		for (ptorp=torplst+MAXTORP;ptorp-- > torplst;)
		    if (!ptorp->tp_aim.ip_ptr)
			break;
		if (ptorp < torplst) {
		    sprintf(buf,"%.17s - sorry, universe full",plogin->ln_input);
		    goto badinp2;
		}
		for (puniv=univlst+MAXUNIVERSE;puniv-- > univlst;)
		    if (!puniv->uv_type)
			break;
		if (puniv < univlst) {
		    perror("play: ptorp but no puniv");
		    goto badinp;
		}

		/* reduce torpedo count */
		pcrft->cr_sys[TORPS].s_lvl -= 1;
		biton(pcrft->cr_chng,BIT_SLEVEL+TORPS*flds[FLD_SLEVEL].f_grpw);

		/* put torpedo into the universe */
		pcrft->cr_dir[0] = 1.0;
		sptort(pcrft->cr_dir,tmpvec);
		for (i=0;i<3;++i) {
		    ptorp->tp_pstn[i] = ADD(pcrft->cr_pstn[i],
		    ptorp->tp_thr[i]=MUL(tmpvec[i],50.));
		    ptorp->tp_vel[i] = ADD(pcrft->cr_vel[i],MUL(tmpvec[i],10.));
		}
		ptorp->tp_aim.ip_ptr = pcrft->cr_auto.ip_ptr;
		ptorp->tp_fby.ip_ptr = plogin->ln_play.ip_ptr;
		ptorp->tp_dist = INT(vdist(ptorp->tp_pstn,
		ptorp->tp_aim.ip_ptr->uv_pstn));
		ptorp->tp_dmg = 0;
		ptorp->tp_lhit.ip_ptr = NULL;
		puniv->uv_type = 'T';
		puniv->uv_pctr = '+';
		puniv->uv_pstn = ptorp->tp_pstn;
		puniv->uv_mass = ptorp->tp_dmg;
		puniv->uv_rad = 1;
		puniv->uv_ptr.uv_torp = ptorp;
		ptorp->tp_univ.ip_ptr = puniv;

		break;

	    /*******/
	    /* fix */
	    /*******/
	    case 23:
		if (!pcrft->cr_sys[DMGCON].s_cap) {
		    sprintf(buf,"%.20s - no damage control",plogin->ln_input);
		    goto badinp2;
		}
		if (sscanf(plogin->ln_input,"%*s%c",&i) != 1) /* null */
		    i = 0;
		else if (sscanf(plogin->ln_input,"%*s %d",&i) != 1)
		    goto badinp;

		/* 0<=arg<=MSYS */
		if (i < 0 || i > MSYS)
		    goto badinp;

		/* subsystem must be present */
		if (i > 0 && !pcrft->cr_sys[i-1].s_cap) {
		    sprintf(buf,"%.20s - no such subsystem",plogin->ln_input);
		    goto badinp2;
		}

		pcrft->cr_sys[DMGCON].s_lvl = i;
		biton(pcrft->cr_chng,BIT_SLEVEL+DMGCON*flds[FLD_SLEVEL].f_grpw);
		break;

	    /***************/
	    /* - (ucmd)    */
	    /***************/
	    case 24:
		if (inuc) {
		    sprintf(buf,"%.24s - nested '-cmd'",plogin->ln_input);
		    goto badinp2;
		}
		for (i=0;i<9;ucargs[i++][0] = NULL) ;
		if ((i=sscanf(plogin->ln_input,
		"-%s %20s %20s %20s %20s %20s %20s %20s %20s %20s%c",
		buf,ucargs[0],ucargs[1],ucargs[2],ucargs[3],ucargs[4],
		ucargs[5],ucargs[6],ucargs[7],ucargs[8],&j)) < 1 || i > 10)
		    goto badinp;
		buf[sizeof(getuckey.uc_name)-1] = NULL;

		/* set up data structure to get first line of ucmd */
		binit((char *)&getuckey,sizeof(getuckey));
		getuckey.uc_ucmdkey = UCMD;
		strcpy(getuckey.uc_plyr,plogin->ln_name);
		strcpy(getuckey.uc_name,buf);
		getuckey.uc_ucmd = 1;

		/* verify that ucmd exists */
		dbmkey.dptr = (char *)&getuckey;
		dbmkey.dsize = sizeof(getuckey);
		dbmdata = fetch(dbmkey);
		if (!dbmdata.dptr) {
		    sprintf(buf,"%.23s - no such '-cmd'",plogin->ln_input);
		    goto badinp2;
		}
		inuc = 1;
		break;

	    /***********/
	    /* sensors */
	    /***********/
	    case 25:
		if (sscanf(plogin->ln_input,"%*s%c",&i) != 1) /* null */
		    i = 0;
		else if (sscanf(plogin->ln_input,"%*s %d",&i) != 1 || i < 0)
		    goto badinp;
		i *= flds[FLD_REPORT].f_maxg; /* skip display group count */

		setrpt(pcrft);

		/* stolen from report and modified */
		for (puniv=univlst+MAXUNIVERSE;puniv-- > univlst;) {
		    if (!puniv->uv_type) continue;
		    if (puniv->uv_ptr.uv_crft == pcrft) continue;
		    /*vdiff(puniv->uv_pstn,pcrft->cr_pstn,tmpvec);*/
		    tmpdspl = vdisp(puniv,pcrft->cr_univ.ip_ptr,'v');
		    vcopy(tmpvec,tmpdspl.vec);
		    vcopy(tmpvec2,tmpvec);
		    rttosp(tmpvec,tmpvec);
		    l = INT(tmpvec[0]);
		    if (l <= 5000L) {
			if (i-- > 0) continue;

			/* is it on viewscreen? */
			vecmul(tmpvec2,pcrft->cr_rmat,tmpvec2);
			rttosp(tmpvec2,tmpvec2);
			ftmp = DIV(pcrft->cr_vang,2.);
			*buf = NULL;
			if (SUB(ftmp,tmpvec2[2]) >= 0.) {
			    tmpvec2[0] = DIV(tmpvec2[2],ftmp);
			    tmpvec2[2] = DIV(PI,2.);
			    sptort(tmpvec2,tmpvec2);
			    sprintf(buf,"%ld,%ld",-INT(MUL(tmpvec2[1],7.)),
			    INT(MUL(tmpvec2[0],15.)));
			}


			/* common characteristics */
			sprintf(buf+strlen(buf),"(%c)@%ld %.1f %.1f",
			puniv->uv_pctr,l,DIV(tmpvec[1],DEGTORAD),
			DIV(tmpvec[2],DEGTORAD));

			/* indicate auto'd, dock'd, or home'd */
			l = 0;
			if (pcrft->cr_auto.ip_ptr == puniv) {
			    if (!l) strcat(buf," (");
			    l = 1;
			    strcat(buf,"A");
			}
			if (pcrft->cr_dock.ip_ptr == puniv) {
			    if (!l) strcat(buf," (");
			    l = 1;
			    strcat(buf,"D");
			}
			for (j=0;j<MHOM;++j)
			    if (pcrft->cr_hom[j].ip_ptr == puniv) {
				if (!l) strcat(buf," (");
				l = 1;
				sprintf(buf+strlen(buf),"%d",j+1);
			    }
			if (l) strcat(buf,")");

			rpt(pcrft,buf);
		    }
		}
		fnshrpt(pcrft,0);
		break;

	    /**********/
	    /* lockon */
	    /**********/
	    case 26:
		if (!pcrft->cr_lhit.ip_ptr) {
		    sprintf(buf,"%.26s - no attacker",plogin->ln_input);
		    goto badinp2;
		}

		/* undo FFWD/AUTO; point in direction of last hit position */
		fixaf(pcrft);
		vdiff(pcrft->cr_lhpstn,pcrft->cr_pstn,pcrft->cr_dir);
		rttosp(pcrft->cr_dir,pcrft->cr_dir);
		biton(pcrft->cr_chng,BIT_DIR1);
		biton(pcrft->cr_chng,BIT_DIR2);
		fixdir(pcrft);

		/* try autopiloting (gross!) */
		goto doauto;

	    /**********/
	    /* redraw */
	    /**********/
	    case 27:
		goto redraw;

	    default:
		perror("play: impossible cmd");
		goto badinp;
	}

	/* display last command */
	output(plogin,'H',(pcrft->cr_lcmd<<8)|FLD_COMMAND,plogin->ln_input);

	/* process ucmd */
done:	if (inuc) {
	    char *ip,*op;

	    /* get ucmd text; setup for next one */
	    dbmdata = fetch(dbmkey);
	    if (!dbmdata.dptr) goto done2;
	    getuckey.uc_ucmd += 1;

	    /* substitute arguments */
	    for (ip=dbmdata.dptr,op=plogin->ln_input;
	    *ip && op < plogin->ln_input + sizeof(plogin->ln_input)-1;) {
		if (*ip == '$' && ip[1] >= '1' && ip[1] <= '9') {
		    char *ap=ucargs[ip[1]-'1'];
		    while (*ap &&
		    op < plogin->ln_input + sizeof(plogin->ln_input)-1)
			*op++ = *ap++;
		    ip += 2;
		} else
		    *op++ = *ip++;
	    }
	    *op = NULL;

	    goto douc;
	}

	/* update screen, reposition cursor, and clear command line if present */
done2:	nums(pcrft);
	view(pcrft);
	if (plogin->ln_input[0]) {
	    output(plogin,'D',0,0);
	    plogin->ln_input[0] = NULL;
	}
	output(plogin,0,0,0);

#ifdef DEBUG
	VDBG("play return\n");
#endif
	return;
}

static int okdir(plogin,pcrft,pftmp)
register struct login *plogin;
register struct crft *pcrft;
double *pftmp;
{
	char c;

	/* no argument, use half viewing angle */
	if (sscanf(plogin->ln_input,"%*s%c",&c) != 1) {
	    *pftmp = DIV(pcrft->cr_vang,2.);
	    return(1);
	}

	/* 0<arg<360 */
	if (sscanf(plogin->ln_input,"%*s %lf",pftmp) != 1)
	    return(0);
	if (SUB(*pftmp,360.) >= 0. || *pftmp <= 0.)
	    return(0);
	*pftmp = MUL(*pftmp,DEGTORAD);
	return(1);
}

static VOID fixaf(pcrft)
register struct crft *pcrft;
{
	if (pcrft->cr_ffwd) {
	    pcrft->cr_ffwd = NULL;
	    biton(pcrft->cr_chng,BIT_AUTOFFWD);
	} else if (pcrft->cr_auto.ip_ptr) {
	    pcrft->cr_auto.ip_ptr = NULL;
	    biton(pcrft->cr_chng,BIT_AUTOFFWD);
	}
}
