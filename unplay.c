/*
 * Spacewar - remove a player from the universe
 *	      update/destroy the ship
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
#include "plyr.h"
#include "login.h"
#include "sys.h"
#include "crft.h"

VOID unplay(plogin)
register struct login *plogin;
{
	register struct crft *pcrft;
	struct crft *pcrft2;
	struct sys *psys;
	struct crftkey getcrkey;
	struct syskey getskey;
	struct plyrkey getplkey;
	struct plyr getpldat;
	datum dbmkey,dbmdata;
	int i;
	extern int numpling;

#ifdef DEBUG
	DBG("unplay(#%d/%s)\n",plogin-loginlst,plogin->ln_name);
#endif

	/* privileged or docked with an object or nothing bad nearby */
	pcrft = plogin->ln_play.ip_ptr->uv_ptr.uv_crft;
	if (pcrft->cr_plvl || (pcrft->cr_dock.ip_ptr &&
	pcrft->cr_dock.ip_ptr->uv_type == 'O') || pcrft->cr_sens[1] == 0)
	    crftupdate(plogin);

	/* destroy; report and give credit if due to another player */
	else {

	    /* delete craft and subsystems from database */
	    binit((char *)&getcrkey,sizeof(getcrkey));
	    getcrkey.cr_crftkey = CRAFT;
	    strcpy(getcrkey.cr_plyr,plogin->ln_name);
	    strcpy(getcrkey.cr_name,plogin->ln_crft);
	    dbmkey.dptr = (char *)&getcrkey;
	    dbmkey.dsize = sizeof(getcrkey);
	    if (delete(dbmkey))
		perror("unplay: can't delete crft");
	    binit((char *)&getskey,sizeof(getskey));
	    getskey.s_syskey = SUBSYS;
	    strcpy(getskey.s_plyr,plogin->ln_name);
	    strcpy(getskey.s_crft,plogin->ln_crft);
	    dbmkey.dptr = (char *)&getskey;
	    dbmkey.dsize = sizeof(getskey);
	    for (psys=pcrft->cr_sys,i=0;i < MSYS;++psys,++i) {
		if (!psys->s_cap) continue;  /* not in this craft */
		getskey.s_type = i;
		if (delete(dbmkey))
		    perror("unplay: can't delete sys");
	    }

	    /* update player losing craft */
	    binit((char *)&getplkey,sizeof(getplkey));
	    getplkey.pl_plyrkey = PLYR;
	    strcpy(getplkey.pl_name,plogin->ln_name);
	    dbmkey.dptr = (char *)&getplkey;
	    dbmkey.dsize = sizeof(getplkey);
	    dbmdata = fetch(dbmkey);
	    if (dbmdata.dptr) {
		bcopy((char *)&getpldat,dbmdata.dptr,sizeof(getpldat));
		getpldat.pl_slst += 1;
		getpldat.pl_klst += pcrft->cr_kill;
		getpldat.pl_plst += pcrft->cr_pnts;
		getpldat.pl_tlst += pcrft->cr_time;
		dbmdata.dptr = (char *)&getpldat;
		dbmdata.dsize = sizeof(getpldat);
		if (store(dbmkey,dbmdata))
		    perror("unplay: can't update plyr");
	    } else
		perror("unplay: can't fetch plyr");

	    /* report destruction and credit destroyer */
	    if (pcrft->cr_lhit.ip_ptr && pcrft->cr_lhit.ip_ptr->uv_type == 'P') {
		pcrft2 = pcrft->cr_lhit.ip_ptr->uv_ptr.uv_crft;
		pcrft2->cr_kill += 1;
		output(pcrft2->cr_lgn,'B',0,0);
		setrpt(pcrft2);
		rpt(pcrft2,"Ship destroyed by you");
		fnshrpt(pcrft2,1);
	    }
	}

	/* remove craft from universe */
	remove(plogin->ln_play);
	if (--numpling < 0) numpling = 0;

	/* put player back to command mode */
	plogin->ln_iomode = NULL;
	plogin->ln_crft[0] = NULL;
	plogin->ln_play.ip_ptr = NULL;
	plogin->ln_stat = NULL;
	plogin->ln_substat = NULL;

#ifdef DEBUG
	VDBG("unplay return\n");
#endif
}
