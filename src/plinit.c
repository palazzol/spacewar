/*
 * Spacewar - routine to initialize a player into the universe
 *	      - terminal type
 *	      - craft name
 *	      - craft structure and universe entry
 *
 * Copyright 1985 obo Systems, Inc.
 * Copyright 1985 Dan Rosenblatt
 */

#include <sys/types.h>
#include <dbm.h>
#include "spacewar.h"
#include "universe.h"
#include "login.h"
#include "sys.h"
#include "crft.h"
#include "obj.h"
#include "aln.h"
#include "torp.h"
#include "build.h"

// add missing headers
#include <stdlib.h>
#include <string.h>

int plinit(plogin)
struct login *plogin;
{
	char trmbuf[2048],trmcap[128],*ptrmcap,*pcm,*pcl,*pce,*pso,*pse;
	char buf[80+1+18],*s,*so,*se;
	struct universe *puniv;
	struct crft *pcrft;
	struct crftkey getcrkey;
	struct crft getcrdat;
	struct syskey getskey;
	datum dbmkey,dbmdata;
	int i;

	DBG("plinit(#%d/%s)\n",plogin-loginlst,plogin->ln_name);

	if (!strcmp(plogin->ln_input,".")) goto noplay;

	/********************/
	/* no terminal type */
	/********************/
	if (!plogin->ln_term) {

	    /* get terminal type from user input */
	    if (!plogin->ln_input[0]) {
getterm:	output(plogin,'C',0,"\nWhat (termcap) terminal type>");
		output(plogin,0,0,0);
		VDBG("plinit return\n");
		return(0);
	    }
	    plogin->ln_rvslh = 0;
	    switch(plogin->ln_input[0]) {
		case '.':
		    so = "so"; se = "se";
		    plogin->ln_rvslh = 1;
		    strcpy(plogin->ln_input,plogin->ln_input+1);
		    break;
		case ',':
		    so = "us"; se = "ue";
		    strcpy(plogin->ln_input,plogin->ln_input+1);
		    break;
		case ';':
		    so = "us"; se = "ue";
		    plogin->ln_rvslh = 1;
		    strcpy(plogin->ln_input,plogin->ln_input+1);
		    break;
		default:
		    so = "so"; se = "se";
		    break;
	    }
	    if (tgetent(trmbuf,plogin->ln_input) != 1) goto getterm;

	    /* get necessary capabilities */
	    ptrmcap = trmcap;
	    if (!(pcm=tgetstr("cm")) ||
	    !(pcl=tgetstr("cl")) ||
	    !(pce=tgetstr("ce")) ||
	    !(pso=tgetstr(so)) ||
	    !(pse=tgetstr(se))) {
		output(plogin,'C',0,
		"\nMissing minimum necessary terminal capabilities\n");
		goto noplay;
	    }

	    /* save terminal type and capabilities */
	    if (!(plogin->ln_term = malloc((unsigned)strlen(plogin->ln_input)+1)) ||
	    !(plogin->ln_tcm = malloc((unsigned)strlen(pcm)+1)) ||
	    !(plogin->ln_tcl = malloc((unsigned)strlen(pcl)+1)) ||
	    !(plogin->ln_tce = malloc((unsigned)strlen(pce)+1)) ||
	    !(plogin->ln_tso = malloc((unsigned)strlen(pso)+1)) ||
	    !(plogin->ln_tse = malloc((unsigned)strlen(pse)+1))) {
		perror("plinit: out of memory for termcaps");
		plogin->ln_term = nullptr;
		plogin->ln_tcm = nullptr;
		plogin->ln_tcl = nullptr;
		plogin->ln_tce = nullptr;
		plogin->ln_tso = nullptr;
		plogin->ln_tse = nullptr;
		goto noplay;
	    }
	    strcpy(plogin->ln_term,plogin->ln_input);
	    plogin->ln_input[0] = 0;
	    strcpy(plogin->ln_tcm,pcm);
	    strcpy(plogin->ln_tcl,pcl);
	    strcpy(plogin->ln_tce,pce);
	    strcpy(plogin->ln_tso,pso);
	    strcpy(plogin->ln_tse,pse);
	}

	/*********/
	/* craft */
	/*********/

	/* prompt for ship name */
	if (!plogin->ln_input[0]) {
	    output(plogin,'C',0,"\nWhat ship>");
	    output(plogin,0,0,0);
	    VDBG("plinit return\n");
	    return(0);
	}

	/* get craft */
	plogin->ln_input[sizeof(plogin->ln_crft)-1] = 0;
	binit((char *)&getcrkey,sizeof(getcrkey));
	getcrkey.cr_crftkey = CRAFT;
	strcpy(getcrkey.cr_plyr,plogin->ln_name);
	strcpy(getcrkey.cr_name,plogin->ln_input);
	dbmkey.dptr = (char *)&getcrkey;
	dbmkey.dsize = sizeof(getcrkey);
	dbmdata = fetch(dbmkey);
	if (!dbmdata.dptr) {
	    sprintf(buf,"\n'%s' - no such ship\n",plogin->ln_input);
	    output(plogin,'C',0,buf);
	    goto noplay;
	}
	binit((char *)&getcrdat,sizeof(getcrdat));
	bytecopy((char *)&getcrdat,dbmdata.dptr,dbmdata.dsize);

	/* must have a hull */
	if (!getcrdat.cr_htyp) {
	    sprintf(buf,"\n'%s' - has no hull\n",plogin->ln_input);
	    output(plogin,'C',0,buf);
	    goto noplay;
	}

	/* must be room in the universe */
	for (puniv=univlst+MAXOBJ+MAXALN;puniv < univlst+MAXUNIVERSE;++puniv)
	    if (!puniv->uv_type)
		break;
	if (puniv >= univlst+MAXUNIVERSE) {
	    output(plogin,'C',0,"\nSorry, the universe is temporarily full\n");
	    goto noplay;
	}

	/* must not be locked */
	if (s=lckmsg()) {
	    output(plogin,'C',0,"\n");
	    output(plogin,'C',0,s);
	    output(plogin,'C',0,"\n");
	    goto noplay;
	}


	/**********************************/
	/* put player/craft into universe */
	/**********************************/

	/* fill in craft data */
	pcrft = crftlst + (plogin - loginlst);	/* 1to1 correspondence */
	*pcrft = getcrdat;

	/* if first time playing, pick a pstn near the object */
	if (pcrft->cr_dock.ip_ofst == -1) {
	    pcrft->cr_dock.ip_ptr = nullptr;
	    i = RANDOM(MAXOBJ-1) + 1; /* 1:MAXOBJ-1 */
	    vcopy(pcrft->cr_pstn,univlst[i].uv_pstn);
	    pcrft->cr_pstn[0] =
		ADD(pcrft->cr_pstn[0],FLOAT(RANDOM(2000)+500));
	    pcrft->cr_pstn[1] =
	   	 ADD(pcrft->cr_pstn[1],FLOAT(RANDOM(2000)+500));
	    pcrft->cr_vang = DIV(PI,4.);
	    pcrft->cr_vdst =
		INT(DIV(VANGVDST,SQUARE(pcrft->cr_vang)));
	    
	    /* point so that facing the object */
	    vdiff(univlst[i].uv_pstn,pcrft->cr_pstn,pcrft->cr_dir);
	    rttosp(pcrft->cr_dir,pcrft->cr_dir);

	/* previously docked with a non-object; pstn remains the same */
	} else if (pcrft->cr_dock.ip_ofst >= MAXOBJ ||
	pcrft->cr_dock.ip_ofst <= 0) {
	    pcrft->cr_dock.ip_ptr = nullptr;

	/* prevsiously docked with an object; pstn shifts to object */
	} else if (pcrft->cr_dock.ip_ofst) {
	    pcrft->cr_dock.ip_ptr = univlst + pcrft->cr_dock.ip_ofst;
	    vcopy(pcrft->cr_pstn,pcrft->cr_dock.ip_ptr->uv_pstn);
	}

	/* turn off inappropriate homing/autopilot */
	if (pcrft->cr_auto.ip_ofst >= MAXOBJ || pcrft->cr_auto.ip_ofst <= 0)
	    pcrft->cr_auto.ip_ptr = nullptr;
	else
	    pcrft->cr_auto.ip_ptr = univlst + pcrft->cr_auto.ip_ofst;
	for (i=0;i < MHOM;++i) {
	    if (pcrft->cr_hom[i].ip_ofst >= MAXOBJ || pcrft->cr_hom[i].ip_ofst <= 0)
		pcrft->cr_hom[i].ip_ptr = nullptr;
	    else
		pcrft->cr_hom[i].ip_ptr = univlst + pcrft->cr_hom[i].ip_ofst;
	}

	/* normalize direction and do rotation matrix */
	fixdir(pcrft);

	/* get all subsystems */
	binit((char *)&getskey,sizeof(getskey));
	getskey.s_syskey = SUBSYS;
	strcpy(getskey.s_plyr,plogin->ln_name);
	strcpy(getskey.s_crft,getcrkey.cr_name);
	dbmkey.dptr = (char *)&getskey;
	dbmkey.dsize = sizeof(getskey);
	for (i=0;i < MSYS;++i) {
	    getskey.s_type = i;
	    dbmdata = fetch(dbmkey);
	    if (dbmdata.dptr)
		bytecopy((char *)(pcrft->cr_sys+i),dbmdata.dptr,dbmdata.dsize);
	}

	/* et al */
	pcrft->cr_lgn = plogin;
	pcrft->cr_lhit.ip_ptr = nullptr;
	plogin->ln_iomode = 's';
	strcpy(plogin->ln_crft,getcrkey.cr_name);
	plogin->ln_play.ip_ptr = puniv;
	puniv->uv_type = 'P';
	puniv->uv_pctr = pcrft->cr_htyp + '0';
	puniv->uv_pstn = pcrft->cr_pstn;
	puniv->uv_mass = pcrft->cr_sys[HULL].s_dmg;
	puniv->uv_rad = 1;
	puniv->uv_ptr.uv_crft = pcrft;
	pcrft->cr_univ.ip_ptr = puniv;

	/* first player must start universe update */
	if (!numpling++) firstplyr();

	VDBG("plinit return\n");
	return(1);

noplay: plogin->ln_stat = 0;
	output(plogin,'C',0,PROMPT);
	output(plogin,0,0,0);
	VDBG("plinit return\n");
	return(0);
}
