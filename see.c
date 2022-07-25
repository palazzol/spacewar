/*
 * Spacewar - let users see the standings
 *	      shows crafts in dbm(3) order, then
 *	      shows players and their standings in dbm(3) order
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
#include "plyr.h"

struct sstat {
	char	ss_stat;		/* C(raft) or P(layer) */
	char	ss_savkey[32];		/* for nextkey(dbm); kludge */
	int	ss_savsiz;		/* for nextkey(dbm); kludge */
	struct pst *ss_lst;		/* list of player statistics */
};
struct pst {
	struct pst *ps_nxt;		/* next player */
	char	ps_name[8+1];		/* player's name */
	long	ps_tpnts,ps_mpnts;	/* total,max points */
	time_t	ps_ttm,ps_mtm;		/* total,max time */
	short	ps_tkls,ps_mkls;	/* total,max kills */
	short	ps_opnts,ps_otm,ps_okls;/* ordering for points, time, kills */
	short	ps_ncrfts;		/* number of crafts (for avg) */
};
static struct pst zpst;

char *malloc();


VOID see(plogin)
register struct login *plogin;
{
	struct sstat *psstat;
	register struct pst *ppst;
	struct pst *nxtpst;
	int nlines=6,nc;
	char buf[80+1];
	datum dbmkey,dbmdata;
	struct crftkey getcrkey;
	struct crft getcrdat;
	struct plyrkey getplkey;
	struct plyr getpldat;


#ifdef DEBUG
	DBG("see(#%d/%s)\n",plogin-loginlst,plogin->ln_name);
#endif

	/**************/
	/* first time */
	/**************/
	if (!(psstat = (struct sstat *)plogin->ln_substat)) {

	    /* allocate and init subtask status structure */
	    if (!(psstat = (struct sstat *)malloc(sizeof(struct sstat)))) {
		perror("see: out of memory for sstat");
		goto done;
	    }
	    binit((char *)psstat,sizeof(*psstat));
	    psstat->ss_stat = CRAFT;
	    plogin->ln_substat = (char *)psstat;

	    sprintf(buf,"Output is in groups of %d lines. ",nlines);
	    output(plogin,'C',0,buf);
	    output(plogin,'C',0,"Hit return when ready for more.\n");
	    output(plogin,'C',0,
	    "A single dot (.) on a line by itself terminates See.\n\n");
	    plogin->ln_iomode = 'm';
	    dbmkey = firstkey();

	/****************/
	/* continuation */
	/****************/
	} else {
	    dbmkey.dptr = ((dbmkey.dsize = psstat->ss_savsiz) > 0) ?
	    psstat->ss_savkey : NULL;
	}

#ifdef DEBUG
	VDBG("see: stat '%c'\n",psstat->ss_stat);
#endif

	/* terminate */
	if (!strcmp(plogin->ln_input,".")) goto done;	/* horrendous */

	/*********************************************************/
	/* keep getting and displaying the current_state records */
	/*********************************************************/
again:	while (dbmkey.dptr) {
	    if (dbmkey.dptr[0] == CRAFT && psstat->ss_stat == CRAFT) {

		bcopy((char *)&getcrkey,dbmkey.dptr,sizeof(getcrkey));
		dbmdata = fetch(dbmkey);
		if (!dbmdata.dptr) {
		    perror("see: can't fetch craft");
		    goto done;
		}
		bcopy((char *)&getcrdat,dbmdata.dptr,dbmdata.dsize);

		/* find/create player statistics structure */
		for (ppst=psstat->ss_lst;ppst;ppst=ppst->ps_nxt)
		    if (!strcmp(ppst->ps_name,getcrkey.cr_plyr))
			break;
		if (!ppst) {
		    if (!(ppst = (struct pst *)malloc(sizeof(struct pst)))) {
			perror("see: out of memory for pst");
			goto done;
		    }
		    binit((char *)ppst,sizeof(*ppst));
		    ppst->ps_nxt = psstat->ss_lst;
		    psstat->ss_lst = ppst;
		    strcpy(ppst->ps_name,getcrkey.cr_plyr);
		}

		/* accumulate statistics */
		ppst->ps_tpnts += getcrdat.cr_pnts;
		ppst->ps_ttm += getcrdat.cr_time;
		ppst->ps_tkls += getcrdat.cr_kill;
		if (getcrdat.cr_pnts > ppst->ps_mpnts)
		    ppst->ps_mpnts = getcrdat.cr_pnts;
		if (getcrdat.cr_time > ppst->ps_mtm)
		    ppst->ps_mtm = getcrdat.cr_time;
		if (getcrdat.cr_kill > ppst->ps_mkls)
		    ppst->ps_mkls = getcrdat.cr_kill;
		ppst->ps_ncrfts += 1;

		/* show */
		sprintf(buf,"%-8s %-12s %14ld %14ld %8d\n",
		getcrkey.cr_plyr,getcrkey.cr_name,getcrdat.cr_pnts,
		getcrdat.cr_time,getcrdat.cr_kill);
		output(plogin,'C',0,buf);
		nlines -= 1;

	    } else if (dbmkey.dptr[0] == PLYR && psstat->ss_stat == PLYR) {

		bcopy((char *)&getplkey,dbmkey.dptr,sizeof(getplkey));
		dbmdata = fetch(dbmkey);
		if (!dbmdata.dptr) {
		    perror("see: can't fetch plyr");
		    goto done;
		}
		bcopy((char *)&getpldat,dbmdata.dptr,sizeof(getpldat));

		/* find player statistics structure */
		for (ppst=psstat->ss_lst;ppst;ppst=ppst->ps_nxt)
		    if (!strcmp(ppst->ps_name,getplkey.pl_name))
			break;
		if (!ppst)
		    ppst = &zpst;

		/* show */
		if (!(nc = ppst->ps_ncrfts)) nc = 1;
		sprintf(buf,
		"\n%-8s %12d %5ld/%5ld/%2d %5ld/%5ld/%2d %2d/%2d/%2d\n",
		getplkey.pl_name,ppst->ps_ncrfts,
		ppst->ps_tpnts/nc,ppst->ps_mpnts,ppst->ps_opnts,
		ppst->ps_ttm/nc,ppst->ps_mtm,ppst->ps_otm,
		ppst->ps_tkls/nc,ppst->ps_mkls,ppst->ps_okls);
		output(plogin,'C',0,buf);
		sprintf(buf,"%8s (%10d) (%12ld) (%12ld) (%6d)\n","",
		getpldat.pl_slst,getpldat.pl_plst,getpldat.pl_tlst,
		getpldat.pl_klst);
		output(plogin,'C',0,buf);
		nlines -= 3;
	    }
	    dbmkey = nextkey(dbmkey);

	    /* if output linecount reached, save state */
	    if (nlines <= 0) {
		output(plogin,0,0,0);
		psstat->ss_savsiz = (dbmkey.dptr) ? dbmkey.dsize : 0;
		bcopy(psstat->ss_savkey,dbmkey.dptr,psstat->ss_savsiz);
#ifdef DEBUG
		VDBG("see return\n");
#endif
		return;
	    }
	}

	/*************************************************/
	/* no more dbm records, (possibly) switch states */
	/*************************************************/
	if (psstat->ss_stat == CRAFT) {

	    /* reset state */
	    psstat->ss_stat = PLYR;
	    dbmkey = firstkey();

	    /*************************************************************/
	    /* compute player's ordering for each of points, time, kills */
	    /*************************************************************/
	    nc = 0;
	    do {
		for (nxtpst=NULL,ppst=psstat->ss_lst;ppst;ppst=ppst->ps_nxt) {
		    if (!ppst->ps_opnts)
			if (!nxtpst || ppst->ps_mpnts > nxtpst->ps_mpnts)
			    nxtpst = ppst;
		}
		if (nxtpst)
		    nxtpst->ps_opnts = ++nc;
	    } while (nxtpst);
	    nc = 0;
	    do {
		for (nxtpst=NULL,ppst=psstat->ss_lst;ppst;ppst=ppst->ps_nxt) {
		    if (!ppst->ps_otm)
			if (!nxtpst || ppst->ps_mtm > nxtpst->ps_mtm)
			    nxtpst = ppst;
		}
		if (nxtpst)
		    nxtpst->ps_otm = ++nc;
	    } while (nxtpst);
	    nc = 0;
	    do {
		for (nxtpst=NULL,ppst=psstat->ss_lst;ppst;ppst=ppst->ps_nxt) {
		    if (!ppst->ps_okls)
			if (!nxtpst || ppst->ps_mkls > nxtpst->ps_mkls)
			    nxtpst = ppst;
		}
		if (nxtpst)
		    nxtpst->ps_okls = ++nc;
	    } while (nxtpst);

	    goto again;	/* horrendous */
	}

	/*************************************/
	/* cleanup - free up allocated space */
	/*************************************/
done:	for (nxtpst=ppst=psstat->ss_lst;ppst;ppst=nxtpst) {
	    nxtpst = nxtpst->ps_nxt;
	    free((char *)ppst);
	}
	free((char *)psstat);
	plogin->ln_iomode = NULL;
	plogin->ln_stat = NULL;
	plogin->ln_substat = NULL;
	output(plogin,'C',0,PROMPT);
	output(plogin,0,0,0);

#ifdef DEBUG
	VDBG("see return\n");
#endif
}
