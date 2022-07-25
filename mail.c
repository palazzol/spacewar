/*
 * Spacewar - mail subsystem
 *
 * Copyright 1984 obo Systems, Inc.
 * Copyright 1984 Dan Rosenblatt
 */

#ifndef VMS
#include <sys/types.h>
#include <dbm.h>
#else /* BSD SYSIII SYSV */
#include <types.h>
#include "dbm.h"
#endif /* VMS */
#include <time.h>
#include "spacewar.h"
#include "universe.h"
#include "login.h"
#include "mlbx.h"
#include "plyr.h"

#define MAILPROMPT	"player/O(ld)/N(ew)/D(elete)/.(quit)>"

struct mstat {
	char	ms_stat;	/* D(isplay) or S(end) */
	short	ms_bgn;		/* display begin */
	short	ms_cur;		/* display current */
	short	ms_end;		/* display end */
	char	ms_towho[8+1];	/* send to player */
	struct mlst *ms_frst;	/* send first line */
	struct mlst *ms_lst;	/* send last line */
};
struct mlst {
	struct mlst *ml_nxt;	/* next mail */
	char	*ml_lin;	/* line of mail */
};

static VOID sndmail(),dspmail();
extern char *malloc();

mail(plogin)
register struct login *plogin;
{
	register struct mstat *pmstat;
	struct mlbxkey getmbkey;
	struct plyrkey getplkey;
	struct plyr getpldat;
	datum dbmkey,dbmdata;
	int i;
	char buf[80+1],c;

#ifdef DEBUG
	DBG("mail(#%d/%s)\n",plogin-loginlst,plogin->ln_name);
#endif

	/********************************/
	/* call subtask based on status */
	/********************************/
	if (pmstat = (struct mstat *)plogin->ln_substat)
	    switch(pmstat->ms_stat) {
		case 'D':
		    dspmail(plogin,pmstat);
		    break;
		case 'S':
		    sndmail(plogin,pmstat);
		    break;
		default:
		    perror("mail: unknown ms_stat");
		    plogin->ln_stat = NULL;
		    plogin->ln_substat = NULL;
		    output(plogin,'C',0,PROMPT);
		    output(plogin,0,0,0);
		    break;
	    }

	/*******************************************/
	/* no status. figure based on input buffer */
	/*******************************************/

	/* nothing - send counts & subtask prompt */
	else if (!plogin->ln_input[0]) {
		/* get player */
		binit((char *)&getplkey,sizeof(getplkey));
		getplkey.pl_plyrkey = PLYR;
		strcpy(getplkey.pl_name,plogin->ln_name);
		dbmkey.dptr = (char *)&getplkey;
		dbmkey.dsize = sizeof(getplkey);
		dbmdata = fetch(dbmkey);
		if (!dbmdata.dptr) {	/* not found? */
		    perror("mail: can't find plyr");
		    plogin->ln_stat = NULL;
		    plogin->ln_substat = NULL;
		    output(plogin,'C',0,PROMPT);
#ifdef DEBUG
		    VDBG("mail return\n");
#endif
		    return;
		}
		bcopy((char *)&getpldat,dbmdata.dptr,sizeof(getpldat));

		sprintf(buf,
		"\nYou have %d line(s) of old mail, %d line(s) of new mail.\n",
		getpldat.pl_seenml - getpldat.pl_frstml + 1,
		getpldat.pl_lstml - getpldat.pl_seenml);
		output(plogin,'C',0,buf);
		output(plogin,'C',0,MAILPROMPT);
		output(plogin,0,0,0);

	/* one character - command */
	} else if (!plogin->ln_input[1]) {

	    /* get player */
	    binit((char *)&getplkey,sizeof(getplkey));
	    getplkey.pl_plyrkey = PLYR;
	    strcpy(getplkey.pl_name,plogin->ln_name);
	    dbmkey.dptr = (char *)&getplkey;
	    dbmkey.dsize = sizeof(getplkey);
	    dbmdata = fetch(dbmkey);
	    if (!dbmdata.dptr) {	/* not found? */
		perror("mail: can't find plyr");
		plogin->ln_stat = NULL;
		plogin->ln_substat = NULL;
		output(plogin,'C',0,PROMPT);
#ifdef DEBUG
		VDBG("mail return\n");
#endif
		return;
	    }
	    bcopy((char *)&getpldat,dbmdata.dptr,sizeof(getpldat));

	    switch(c=plogin->ln_input[0]) {

		case 'D':	/* delete all seen mail */
		case 'd':
		    binit((char *)&getmbkey,sizeof(getmbkey));
		    getmbkey.mb_mlbxkey = MLBX;
		    strcpy(getmbkey.mb_plyr,plogin->ln_name);
		    dbmkey.dptr = (char *)&getmbkey;
		    dbmkey.dsize = sizeof(getmbkey);
		    i = 0;
		    while (getpldat.pl_frstml <= getpldat.pl_seenml) {
			getmbkey.mb_mlbx = getpldat.pl_frstml++;
			if (delete(dbmkey))
			    perror("mail: can't delete mlbx");
			else
			    ++i;
		    }
		    sprintf(buf,"\nDeleted %d line(s) of mail\n",i);
		    output(plogin,'C',0,buf);

		    /* update plyr */
		    dbmkey.dptr = (char *)&getplkey;
		    dbmkey.dsize = sizeof(getplkey);
		    dbmdata.dptr = (char *)&getpldat;
		    dbmdata.dsize = sizeof(getpldat);
		    if (store(dbmkey,dbmdata))
			perror("mail: can't update plyr");

		    /* back to command prompt */
		case '.':	/* quit mail */
		    plogin->ln_stat = NULL;
		    output(plogin,'C',0,PROMPT);
		    output(plogin,0,0,0);
		    break;

		case 'O':	/* display old mail */
		case 'o':
		case 'N':	/* display new mail */
		case 'n':
		    /* allocate subtask status structure */
		    if (!(pmstat = (struct mstat *)
		    malloc(sizeof(struct mstat)))) {
			perror("mail: out of memory for mstat");
			plogin->ln_stat = NULL;
			output(plogin,'C',0,PROMPT);
			output(plogin,0,0,0);
			break;
		    }

		    /* initialize subtask status structure */
		    pmstat->ms_stat = 'D';
		    if (c == 'O' || c == 'o') {
			pmstat->ms_bgn = getpldat.pl_frstml;
			pmstat->ms_end = getpldat.pl_seenml;
		    } else {
			pmstat->ms_bgn = getpldat.pl_seenml + 1;
			pmstat->ms_end = getpldat.pl_lstml;
		    }
		    pmstat->ms_cur = pmstat->ms_bgn - 1;

		    /* save subtask status structure and start subtask */
		    plogin->ln_substat = (char *) pmstat;
		    dspmail(plogin,pmstat);
		    break;

		default:
		    output(plogin,'C',0,MAILPROMPT);
		    output(plogin,0,0,0);
		    break;
	    }

	/* send to player */
	} else {
		/* allocate/initialize subtask status structure */
		if (!(pmstat = (struct mstat *) malloc(sizeof(struct mstat)))) {
			perror("mail: out of memory for mstat");
			plogin->ln_stat = NULL;
			output(plogin,'C',0,PROMPT);
			output(plogin,0,0,0);
		} else {
			pmstat->ms_stat = 'S';
			plogin->ln_input[sizeof(pmstat->ms_towho)-1] = NULL;
			strcpy(pmstat->ms_towho,plogin->ln_input);
			pmstat->ms_frst = pmstat->ms_lst = NULL;
			plogin->ln_substat = (char *) pmstat;
			sndmail(plogin,pmstat);
		}
	}

#ifdef DEBUG
	VDBG("mail return\n");
#endif
}

static VOID dspmail(plogin,pmstat)
register struct login *plogin;
register struct mstat *pmstat;
{
	struct mlbxkey getmbkey;
	struct plyrkey getplkey;
	struct plyr getpldat;
	datum dbmkey,dbmdata;
	char buf[80+1];
	int nlines=6;

#ifdef DEBUG
	DBG("dspmail(#%d/%s,{%d,%d,%d})\n",plogin-loginlst,plogin->ln_name,
	pmstat->ms_bgn,pmstat->ms_cur,pmstat->ms_end);
#endif

	/* first time */
	if (pmstat->ms_cur < pmstat->ms_bgn) {

		sprintf(buf,"\nThere are %d line(s) of mail\n",
		pmstat->ms_end - pmstat->ms_bgn + 1);
		output(plogin,'C',0,buf);

		if (pmstat->ms_end >= pmstat->ms_bgn) {
		    sprintf(buf,"Output is in groups of %d lines. ",nlines);
		    output(plogin,'C',0,buf);
		    output(plogin,'C',0,"Hit return when ready for more.\n");
		    output(plogin,'C',0,
		    "A single dot (.) on a line by itself terminates Mail.\n\n");
		    plogin->ln_iomode = 'm';
		}
	}

	/* display a group of lines */
	binit((char *)&getmbkey,sizeof(getmbkey));
	getmbkey.mb_mlbxkey = MLBX;
	strcpy(getmbkey.mb_plyr,plogin->ln_name);
	dbmkey.dptr = (char *)&getmbkey;
	dbmkey.dsize = sizeof(getmbkey);
	while (nlines-- > 0) {

		/* terminate or no more - update plyr */
		/* and go back to command level */
		if (!strcmp(plogin->ln_input,".") ||
		++pmstat->ms_cur > pmstat->ms_end) {
			binit((char *)&getplkey,sizeof(getplkey));
			getplkey.pl_plyrkey = PLYR;
			strcpy(getplkey.pl_name,plogin->ln_name);
			dbmkey.dptr = (char *)&getplkey;
			dbmkey.dsize = sizeof(getplkey);
			dbmdata = fetch(dbmkey);
			if (!dbmdata.dptr)	/* not found? */
				perror("dspmail: can't find plyr");
			else {
				bcopy((char *)&getpldat,dbmdata.dptr,
				sizeof(getpldat));
				getpldat.pl_seenml =
				    (pmstat->ms_cur < pmstat->ms_end) ?
				    pmstat->ms_cur : pmstat->ms_end;
				dbmdata.dptr = (char *)&getpldat;
				dbmdata.dsize = sizeof(getpldat);
				if (store(dbmkey,dbmdata))
					perror("dspmail: can't update plyr");
			}

			free((char *)pmstat);
			plogin->ln_iomode = NULL;
			plogin->ln_stat = NULL;
			plogin->ln_substat = NULL;
			output(plogin,'C',0,PROMPT);
			break;

		/* get and put out the line of mail */
		} else {
			getmbkey.mb_mlbx = pmstat->ms_cur;
			dbmdata = fetch(dbmkey);
			if (!dbmdata.dptr)	/* not found? */
				perror("dspmail: can't find mlbx");
			else {
				output(plogin,'C',0,dbmdata.dptr);
				output(plogin,'C',0,"\n");
			}
		}
	}

	output(plogin,0,0,0);
#ifdef DEBUG
	VDBG("dspmail return\n");
#endif
}

static VOID sndmail(plogin,pmstat)
register struct login *plogin;
register struct mstat *pmstat;
{
	time_t clock;
	struct mlst *pmlst,*nxtmlst;
	struct plyrkey getplkey;
	struct plyr getpldat;
	struct mlbxkey getmbkey;
	datum dbmkey,dbmdata;
	int mlost=0,nlines;
	struct tm *localtime();
	char *asctime(),*ctime(),buf[80+1];

#ifdef DEBUG
	DBG("sndmail(#%d/%s,%s)\n",plogin-loginlst,plogin->ln_name,
	pmstat->ms_towho);
#endif

	/* first time or terminate */
	if (!pmstat->ms_frst || !strcmp(plogin->ln_input,".")) {

	    /* verify existence of recipient */
	    binit((char *)&getplkey,sizeof(getplkey));
	    getplkey.pl_plyrkey = PLYR;
	    strcpy(getplkey.pl_name,pmstat->ms_towho);
	    dbmkey.dptr = (char *)&getplkey;
	    dbmkey.dsize = sizeof(getplkey);
	    dbmdata = fetch(dbmkey);
	    if (!dbmdata.dptr) {	/* doesn't exist */
		output(plogin,'C',0,pmstat->ms_towho);
		output(plogin,'C',0," - no such player\n");
		goto terminate;	/* horrendous */

	    } else if (!pmstat->ms_frst) {	/* first time */
		time(&clock);
		sprintf(plogin->ln_input,"From '%s' on %.24s",plogin->ln_name,
#ifdef VMS
		ctime(&clock));
#else /* BSD SYSIII SYSV */
		asctime(localtime(&clock)));
#endif /* VMS BSD SYSIII SYSV */
		output(plogin,'C',0,
		"Terminate your mail with a single dot (.) on a line by itself.\n\n");
		output(plogin,0,0,0);

	    } else {	/* terminate */

		/* insert mlbx lines */
		bcopy((char *)&getpldat,dbmdata.dptr,sizeof(getpldat));
		binit((char *)&getmbkey,sizeof(getmbkey));
		getmbkey.mb_mlbxkey = MLBX;
		strcpy(getmbkey.mb_plyr,pmstat->ms_towho);
		dbmkey.dptr = (char *)&getmbkey;
		dbmkey.dsize = sizeof(getmbkey);
		for (nlines=(-1),nxtmlst=pmlst=pmstat->ms_frst;pmlst;
		++nlines,pmlst=nxtmlst) {
		    nxtmlst = nxtmlst->ml_nxt;
		    getmbkey.mb_mlbx = ++getpldat.pl_lstml;
		    dbmdata.dsize = strlen(dbmdata.dptr=pmlst->ml_lin) + 1;
		    if (!mlost && store(dbmkey,dbmdata)) {
			++mlost;
			output(plogin,'C',0,
			"(sigh) database collision, your mail was lost\n");
		    }
		    free(pmlst->ml_lin);
		    free((char *)pmlst);
		}

		/* insert trailer, update recipient */
		if (!mlost) {
		    getmbkey.mb_mlbx = ++getpldat.pl_lstml;
		    dbmdata.dsize = strlen(dbmdata.dptr=".") + 1;
		    if (store(dbmkey,dbmdata)) --getpldat.pl_lstml;

		    dbmkey.dptr = (char *)&getplkey;
		    dbmkey.dsize = sizeof(getplkey);
		    dbmdata.dptr = (char *)&getpldat;
		    dbmdata.dsize = sizeof(getpldat);
		    if (store(dbmkey,dbmdata)) {
			perror("sndmail: can't update plyr");
			output(plogin,'C',0,
			"(sigh) database error, your mail was lost\n");
		    } else {
			sprintf(buf,"Sent %d+2 line(s) of mail\n",nlines);
			output(plogin,'C',0,buf);
		    }
		}

		/* back to command prompt */
terminate:
		free((char *)pmstat);
		plogin->ln_stat = NULL;
		plogin->ln_substat = NULL;
		output(plogin,'C',0,PROMPT);
		output(plogin,0,0,0);
#ifdef DEBUG
		VDBG("sndmail return\n");
#endif
		return;
	    }
	}

	/*************************/
	/* save the line of mail */
	/*************************/

	/* terminate early if no memory */
	if (!(pmlst = (struct mlst *) malloc((unsigned)sizeof(struct mlst))))
		perror("sndmail: out of memory for mlst");
	if (!pmlst || !(pmlst->ml_lin = malloc(strlen(plogin->ln_input)+1))) {
		if (pmlst) perror("sndmail: out of memory for ml_lin");
		strcpy(plogin->ln_input,".");
		sndmail(plogin,pmstat);

	/* save text and link in to list */
	} else {
		pmlst->ml_nxt = NULL;
		strcpy(pmlst->ml_lin,plogin->ln_input);
		if (pmstat->ms_frst)
			pmstat->ms_lst->ml_nxt = pmlst;
		else
			pmstat->ms_frst = pmlst;
		pmstat->ms_lst = pmlst;
		output(plogin,'C',0,">");
		output(plogin,0,0,0);
	}

#ifdef DEBUG
	VDBG("sndmail return\n");
#endif
}
