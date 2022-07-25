/*
 * Spacewar - update crafts to database
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

VOID crftupdate(plogin)
register struct login *plogin;
{
	struct login *plow,*phigh;
	struct crftkey getcrkey;
	struct crft getcrdat;
	struct syskey getskey;
	datum dbmkey,dbmdata;
	register struct sys *psys;
	register int i;


	/* do a specific one or all */
	if (plogin) {
#ifdef DEBUG
	    DBG("crftupdate(#%d/%s)\n",plogin-loginlst,plogin->ln_name);
#endif
	    plow = phigh = plogin;
	} else {
#ifdef DEBUG
	    DBG("crftupdate(#ALL)\n");
#endif
	    plow = loginlst;
	    phigh = loginlst + MAXLOGIN - 1;
	}

	/* update associated crafts and subsystems to the database */
	for (plogin=plow;plogin <= phigh;++plogin) {

	    /* skip if not a player or not playing */
	    if (!plogin->ln_tty || !plogin->ln_play.ip_ptr)
		continue;

	    /* update craft */
	    binit((char *)&getcrkey,sizeof(getcrkey));
	    getcrkey.cr_crftkey = CRAFT;
	    strcpy(getcrkey.cr_plyr,plogin->ln_name);
	    strcpy(getcrkey.cr_name,plogin->ln_crft);
	    dbmkey.dptr = (char *)&getcrkey;
	    dbmkey.dsize = sizeof(getcrkey);

	    /* normalize idxptr's */
	    getcrdat = *plogin->ln_play.ip_ptr->uv_ptr.uv_crft;
	    if (getcrdat.cr_dock.ip_ptr)
		getcrdat.cr_dock.ip_ofst = getcrdat.cr_dock.ip_ptr - univlst;
	    else
		getcrdat.cr_dock.ip_ofst = 0;
	    if (getcrdat.cr_auto.ip_ptr)
		getcrdat.cr_auto.ip_ofst = getcrdat.cr_auto.ip_ptr - univlst;
	    else
		getcrdat.cr_auto.ip_ofst = 0;
	    for (i=0;i < MHOM;++i)
		if (getcrdat.cr_hom[i].ip_ptr)
		    getcrdat.cr_hom[i].ip_ofst = getcrdat.cr_hom[i].ip_ptr -
		    univlst;
		else
		    getcrdat.cr_hom[i].ip_ofst = 0;

	    dbmdata.dptr = (char *)&getcrdat;
	    dbmdata.dsize = CRDATSIZ;
	    if (store(dbmkey,dbmdata))
		perror("crftupdate: can't update crft");

	    /* update craft subsystems */
	    binit((char *)&getskey,sizeof(getskey));
	    getskey.s_syskey = SUBSYS;
	    strcpy(getskey.s_plyr,plogin->ln_name);
	    strcpy(getskey.s_crft,plogin->ln_crft);
	    dbmkey.dptr = (char *)&getskey;
	    dbmkey.dsize = sizeof(getskey);
	    dbmdata.dsize = sizeof(struct sys);
	    for (psys=getcrdat.cr_sys,i=0;i < MSYS;++psys,++i) {
		if (!psys->s_cap) continue;  /* not in this craft */
		getskey.s_type = i;
		dbmdata.dptr = (char *)psys;
		if (store(dbmkey,dbmdata))
		    perror("crftupdate: can't update sys");
	    }
	}

#ifdef DEBUG
	VDBG("crftupdate return\n");
#endif
}
