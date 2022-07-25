/*
 * Spacewar - get and process player commands doing character/line editing
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
#include <errno.h>
#include "spacewar.h"
#include "universe.h"
#include "login.h"
#include "uio.h"
#include "plyr.h"
#include "mlbx.h"
#ifdef VMS
#	include <iodef.h>
#	include <ssdef.h>
	short inmlbx;
#endif /* VMS */

int doproctrap,doupdate;
static struct login *getinp();
static VOID cmd2();
extern VOID prvcmd();

VOID cmd()
{
	register struct login *plogin;

#ifdef DEBUG
	DBG("cmd()\n");
#endif

	/* get and process a command */
	if (!(plogin = getinp())) {
#ifdef DEBUG
		VDBG("cmd return\n");
#endif
		return;
	}

	/* based on current major command */
#ifdef DEBUG
	VDBG("cmd: stat #%d '%c'\n",plogin-loginlst,plogin->ln_stat);
#endif
	switch(plogin->ln_stat) {

		case 'M':	/* Mail (Complain) */
			doupdate = 0;
			mail(plogin);
			if (doupdate == 0) doupdate = 1;
			break;

		case 'B':	/* reBuild */
			doupdate = 0;
			build(plogin);
			if (doupdate == 0) doupdate = 1;
			break;

		case 'S':	/* See */
			doupdate = 0;
			see(plogin);
			if (doupdate == 0) doupdate = 1;
			break;

		case 'U':	/* Usercmd */
			doupdate = 0;
			usrcmd(plogin);
			if (doupdate == 0) doupdate = 1;
			break;

		case 'W':	/* Who */
			doupdate = 0;
			who(plogin);
			if (doupdate == 0) doupdate = 1;
			break;

		case 'P':	/* Play */
			doupdate = 0;
			play(plogin);
			if (doupdate == 0) doupdate = 1;
			break;

		default:	/* not doing anything yet */
			doupdate = 0;
			cmd2(plogin);
			if (doupdate == 0) doupdate = 1;
			break;
	}

	plogin->ln_input[0] = NULL;
#ifdef DEBUG
	VDBG("cmd return\n");
#endif
}

static VOID cmd2(plogin)
register struct login *plogin;
{
	int inplen=strlen(plogin->ln_input);
	struct plyrkey getplkey;
	struct plyr getpldat;
	datum dbmkey,dbmdata;

#ifdef DEBUG
	DBG("cmd2()\n");
#endif

	/********************/
	/* response is name */
	/********************/
	if (!plogin->ln_name[0]) { /* no name */

#ifdef DEBUG
		VDBG("cmd2: #%d response is name\n",plogin-loginlst);
#endif

		/* do again if too short or long */
		if (inplen < 2 || inplen >= sizeof(plogin->ln_name)) {
			output(plogin,'C',0,
			"Name must be 2 to 8 characters\n\nWhat is your name?");
			output(plogin,0,0,0);
#ifdef DEBUG
			VDBG("cmd2 return\n");
#endif
			return;
		}

		/* insure no players playing more than once */
		{
			int i;
			struct login *plgn;

#ifdef DEBUG
			VDBG("cmd2: #%d checking for dup login (%s)\n",
			plogin-loginlst,plogin->ln_input);
#endif
			for (i=MAXLOGIN,plgn=loginlst;i-- > 0;++plgn) {
				if (!strcmp(plogin->ln_input,plgn->ln_name)) {
				    output(plogin,'C',0,
				    "Someone is already playing that name.\n\n\
How about another name?");
				    output(plogin,0,0,0);
#ifdef DEBUG
				    VDBG("cmd2 return\n");
#endif
				    return;
				}
			}
		}

		/* store name and prompt for password */
		strcpy(plogin->ln_name,plogin->ln_input);
		output(plogin,'C',0,"Password:");
		output(plogin,0,0,0);

		/* set up password state */
		plogin->ln_stat = 'p';
		plogin->ln_iomode = 'p'; /* no echo */

	/************************/
	/* response is password */
	/************************/
	} else if (plogin->ln_stat == 'p') { /* password state */

#ifdef DEBUG
		VDBG("cmd2: #%d (%s) response is password\n",
		plogin-loginlst,plogin->ln_name);
#endif

		/* get player */
		binit((char *)&getplkey,sizeof(getplkey));
		getplkey.pl_plyrkey = PLYR;
		strcpy(getplkey.pl_name,plogin->ln_name);
		dbmkey.dptr = (char *)&getplkey;
		dbmkey.dsize = sizeof(getplkey);

		/* found; check password */
		dbmdata = fetch(dbmkey);
		if (dbmdata.dptr) {

#ifdef DEBUG
			VDBG("cmd2: #%d (%s) found, checking password\n",
			plogin-loginlst,plogin->ln_name);
#endif

			bcopy((char *)&getpldat,dbmdata.dptr,sizeof(getpldat));

			/* bad password */
			if (strcmp(plogin->ln_input,getpldat.pl_passwd)) {
				output(plogin,'C',0,"Incorrect.\n\nPassword:");
				output(plogin,0,0,0);

			/* good password; check for mail and give prompt */
			} else {
				output(plogin,'C',0,"\n\n");
				if (getpldat.pl_seenml < getpldat.pl_lstml)
				output(plogin,'C',0,"You have new mail\n");

				output(plogin,'C',0,PROMPT);
				output(plogin,0,0,0);
				plogin->ln_stat = NULL;
				plogin->ln_iomode = NULL;

				/* update login info */
				++getpldat.pl_numlgn;
				time(&getpldat.pl_lstlgn);
				dbmdata.dptr = (char *)&getpldat;
				dbmdata.dsize = sizeof(getpldat);
				if (store(dbmkey,dbmdata))
					perror("cmd2: can't update plyr");
			}

		/* not found; insert new player */
		} else {

#ifdef DEBUG
			VDBG("cmd2: #%d (%s) not found, inserting\n",
			plogin-loginlst,plogin->ln_name);
#endif

			/* do again if too short or long */
			if (inplen < 2 || inplen >= sizeof(getpldat.pl_passwd)) {
			    output(plogin,'C',0,
			    "Password must be 2 to 8 characters\n\nPassword:");
			    output(plogin,0,0,0);
#ifdef DEBUG
			    VDBG("cmd2 return\n");
#endif
			    return;
			}

			/* initialize new player login */
			binit((char *)&getpldat,sizeof(getpldat));
			strcpy(getpldat.pl_passwd,plogin->ln_input);
			getpldat.pl_numlgn = 1;
			time(&getpldat.pl_lstlgn);
			getpldat.pl_frstml = 1;

			/* insert new player */
			dbmdata.dptr = (char *)&getpldat;
			dbmdata.dsize = sizeof(getpldat);
			if (store(dbmkey,dbmdata)) {
				output(plogin,'C',0,
				"(sigh) database collision - try another name\
\n\nWhat is your name?");
				output(plogin,0,0,0);
				plogin->ln_name[0] = NULL;

			/* give prompt */
			} else {
				output(plogin,'C',0,"\n\n");
				output(plogin,'C',0,PROMPT);
				output(plogin,0,0,0);
			}
			plogin->ln_stat = NULL;
			plogin->ln_iomode = NULL;
		}

	/***********************/
	/* response is command */
	/***********************/
	} else switch(plogin->ln_input[0]) {

		case 'L': case 'l':	/* Logoff */
			logoff(plogin);
			break;

		case 'B': case 'b':	/* reBuild */
			plogin->ln_stat = 'B';
			plogin->ln_crft[0] = NULL;
			plogin->ln_substat = NULL;
			plogin->ln_input[0] = NULL;
			build(plogin);
			break;

		case 'I': case 'i':	/* Information */
			output(plogin,'C',0,
			"\nGet hold of and read the spacewar documentation\n");
			output(plogin,'C',0,PROMPT);
			output(plogin,0,0,0);
			break;

		case 'C': case 'c':	/* Complain -> Mail to Dan */
			plogin->ln_stat = 'M';
			plogin->ln_substat = NULL;
			strcpy(plogin->ln_input,SWMASTER);
			mail(plogin);
			break;

		case 'M': case 'm':	/* Mail */
			plogin->ln_stat = 'M';
			plogin->ln_substat = NULL;
			plogin->ln_input[0] = NULL;
			mail(plogin);
			break;

		case 'U': case 'u':	/* Usercmd */
			plogin->ln_stat = 'U';
			plogin->ln_substat = NULL;
			plogin->ln_input[0] = NULL;
			usrcmd(plogin);
			break;

		case 'S': case 's':	/* See */
			plogin->ln_stat = 'S';
			plogin->ln_substat = NULL;
			plogin->ln_input[0] = NULL;
			see(plogin);
			break;

		case 'W': case 'w':	/* Who */
			plogin->ln_stat = 'W';
			plogin->ln_substat = NULL;
			plogin->ln_input[0] = NULL;
			who(plogin);
			break;

		case 'P': case 'p':	/* Play */
			plogin->ln_stat = 'P';
			plogin->ln_crft[0] = NULL;
			plogin->ln_substat = NULL;
			plogin->ln_input[0] = NULL;
			play(plogin);
			break;

		default:
			prvcmd(plogin);
			output(plogin,'C',0,PROMPT);
			output(plogin,0,0,0);
			break;
	}

#ifdef DEBUG
	VDBG("cmd2 return\n");
#endif
}

/* get input; return a ptr to where a full command is */
static struct login *getinp()
{
	struct uio inp;
	int i;
	register char *p;
	register char *input;
	extern int errno;
#ifndef BSD
#ifndef VMS
#	include "uio2.h"
	struct uio2 inp2;
#endif /* VMS */
#endif /* BSD */

#ifdef DEBUG
	DBG("getinp()\n");
#endif /* DEBUG */

	/* get player input allowing asynchronous  */
	/* trap processing; do echo and edit magic */
	/* do all this until there is a full cmd   */
	for (;;) {

		/* get the uio header allowing asynch trap processing */
		if (doproctrap < 0 || doupdate < 0) {
#ifdef DEBUG
			VDBG("getinp return\n");
#endif
			return(NULL);
		}
		doproctrap = 1;
#ifdef VMS
		inp.uio_lgn = (struct login *)-1;
		if ((i=sys$qiow(0,inmlbx,IO$_READVBLK,0,0,0,&inp,sizeof(inp),
		0,0,0,0)) != SS$_NORMAL)  {
#ifdef DEBUG
		    VDBG("getinp qiow(READVBLK)=%d errno=%d\n",i,errno);
#endif
		    if (errno != EINTR)
			perror("read mlbx uio");
		    continue;
		}
		if (inp.uio_lgn == (struct login *)-1) continue;
#else /* BSD SYSIII SYSV */
		if (read(0,&inp,sizeof(inp)) != sizeof(inp)) {
			if (errno != EINTR)
				perror("read uio");
			continue;
		}
#endif /* VMS BSD SYSIII SYSV */
		doproctrap = 0;
#ifdef DEBUG
#ifndef BSD
		if (((int)inp.uio_lgn) >= 0 &&((int)inp.uio_lgn) <= 20) {
#ifdef VMS
			VDBG("getinp: uio sig %d %s\n",(int)inp.uio_lgn,
			inp.uio_chrs);
#else /* SYSIII SYSV */
			bcopy((char *)&inp2,(char *)&inp,sizeof(inp2));
			VDBG("getinp: uio sig %d %d %s\n",inp2.uio2sig,
			inp2.uio2pid,inp2.uio2tty);
#endif /* VMS SYSIII SYSV */
		} else
#endif /* BSD */
		{
			VDBG("getinp: uio #%d '",inp.uio_lgn-loginlst);
			for (p=inp.uio_chrs;*p;++p)
			    VDBG((*p < ' ' || *p > '~') ? "\\%03o" : "%c",*p);
			VDBG("'\n");
		}
#endif /* DEBUG */

		/* validate login pointer */
#ifndef BSD
		if (((int)inp.uio_lgn) >= 0 &&((int)inp.uio_lgn) <= 20) {
#ifdef VMS
			proctrap(inp);
#else /* SYSIII SYSV */
			bcopy((char *)&inp2,(char *)&inp,sizeof(inp2));
			proctrap(inp2);
#endif /* VMS SYSIII SYSV */
			continue;
		}
#endif /* BSD */
		if (inp.uio_lgn < loginlst ||
		inp.uio_lgn >= loginlst+MAXLOGIN) {
			perror("uio_lgn out of range");
			continue;
		}

		/* echo (based on iomode) and edit magic */
		input = inp.uio_lgn->ln_input;
		input += strlen(input);
		for (p=inp.uio_chrs;*p;++p) {
		    switch(*p) {
			case '\n':
			case '\r':
			    *input = NULL;

			    /* strip trailing blanks */
			    for (p=inp.uio_lgn->ln_input;--input >= p &&
			    *input == ' ';)
				*input = NULL;

			    if (inp.uio_lgn->ln_iomode == 'm' ||
			    inp.uio_lgn->ln_iomode == 's')
				output(inp.uio_lgn,'C',0,"\r");
			    else
				output(inp.uio_lgn,'C',0,"\n");
			    output(inp.uio_lgn,0,0,0);
#ifdef DEBUG
			    VDBG("getinp return\n");
#endif /* DEBUG */
			    return(inp.uio_lgn);

			case '\025':	/* control-U */
			    if (inp.uio_lgn->ln_iomode == 's') {
				output(inp.uio_lgn,'D',0,0);
				input = inp.uio_lgn->ln_input;
			    } else {
				for (i=strlen(inp.uio_lgn->ln_input);i-- > 0;) {
				    --input;
				    output(inp.uio_lgn,'C',0,"\b \b");
				}
			    }
			    break;

			case '\b':
			    if (input > inp.uio_lgn->ln_input) {
				--input;
				output(inp.uio_lgn,'C',0,"\b \b");
			    }
			    break;
		    }

		    /* ignore all non-printing chars */
		    if (*p < ' ' || *p > '~') continue;

		    /* make sure buffer can't overflow */
		    if (input >= inp.uio_lgn->ln_input +
		    sizeof(inp.uio_lgn->ln_input) - 1) {
			*input = NULL;
			if (inp.uio_lgn->ln_iomode == 'm' ||
			inp.uio_lgn->ln_iomode == 's')
			    output(inp.uio_lgn,'C',0,"\r");
			else
			    output(inp.uio_lgn,'C',0,"\n");
			output(inp.uio_lgn,0,0,0);
#ifdef DEBUG
			VDBG("getinp return\n");
#endif /* DEBUG */
			return(inp.uio_lgn);
		    }

		    input[1] = NULL;
		    *input = *p;
		    output(inp.uio_lgn,'C',0,(inp.uio_lgn->ln_iomode == 'p') ?
		    " " : input); /* space if password state, char otherwise */
		    ++input;
		}
		*input = NULL;
		output(inp.uio_lgn,0,0,0);
	}
}
