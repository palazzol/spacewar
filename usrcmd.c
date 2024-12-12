/*
 * Spacewar - user-command (play) subsystem
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
#include "ucmd.h"

#define UCMDPROMPT	"\ncommand/L(ist)/.(quit)>"

VOID usrcmd(plogin)
register struct login *plogin;
{
	struct ucmdkey *pucmdkey;
	struct ucmdkey getuckey;
	datum dbmkey,dbmdata;
	char buf[40+1];
	extern char *malloc();

#ifdef DEBUG
	DBG("usrcmd(#%d/%s)\n",plogin-loginlst,plogin->ln_name);
#endif

	/**************/
	/* first time */
	/**************/
	if (!strcmp(plogin->ln_input,".")) goto done;
	if (!(pucmdkey = (struct ucmdkey *)plogin->ln_substat)) {
#ifdef DEBUG
	    VDBG("usrcmd: inp '%s'\n",plogin->ln_input);
#endif

	    /* subtask prompt */
	    if (!plogin->ln_input[0]) {
		output(plogin,'C',0,UCMDPROMPT);

	    /* one letter command - list all user-defined command names */
	    } else if (!plogin->ln_input[1]) {

		strcpy(buf,"\n");
		dbmkey = firstkey();
		while (dbmkey.dptr) {
		    bytecopy((char *)&getuckey,dbmkey.dptr,sizeof(getuckey));
		    if (getuckey.uc_ucmdkey == UCMD &&
		    !strcmp(getuckey.uc_plyr,plogin->ln_name) &&
		    getuckey.uc_ucmd == 1) {
			if (strlen(buf) + 1 + strlen(getuckey.uc_name) >=
			sizeof(buf)) {
			    output(plogin,'C',0,buf);
			    strcpy(buf,"\n");
			}
			strcat(buf,getuckey.uc_name);
			strcat(buf," ");
		    }
		    dbmkey = nextkey(dbmkey);
		}
		if (strlen(buf) > 1) {
		    output(plogin,'C',0,buf);
		    output(plogin,'C',0,"\n");
		}
		goto done;

	    /* command name - show existing and set up for re-entry */
	    } else {
		if (!(pucmdkey = (struct ucmdkey *)
		malloc(sizeof(struct ucmdkey)))) {
		    perror("usrcmd: out of memory");
		    goto done;
		}
		plogin->ln_substat = (char *) pucmdkey;

		output(plogin,'C',0,
		"\nThe currently defined command (if any) will be displayed.\n");
		output(plogin,'C',0,
		"A single dot (.) at the first prompt will retain it.\n");

		/* create subtask structure */
		binit((char *)pucmdkey,sizeof(*pucmdkey));
		pucmdkey->uc_ucmdkey = UCMD;
		strcpy(pucmdkey->uc_plyr,plogin->ln_name);
		plogin->ln_input[sizeof(pucmdkey->uc_name)-1] = NULL;
		strcpy(pucmdkey->uc_name,plogin->ln_input);
		pucmdkey->uc_ucmd = 1;

		/* show existing */
		dbmkey.dptr = (char *)pucmdkey;
		dbmkey.dsize = sizeof(*pucmdkey);
		dbmdata = fetch(dbmkey);
		if (dbmdata.dptr) output(plogin,'C',0,"\n");
		while (dbmdata.dptr) {
		    output(plogin,'C',0,dbmdata.dptr);
		    output(plogin,'C',0,"\n");
		    ++pucmdkey->uc_ucmd;
		    dbmdata = fetch(dbmkey);
		}

		/* prompt for new */
		pucmdkey->uc_ucmd = 1;
		output(plogin,'C',0,"\n>");
	    }

	/***************/
	/* new command */
	/***************/
	} else {

	    /* delete old stuff */
	    if (pucmdkey->uc_ucmd == 1) {
		dbmkey.dptr = (char *)pucmdkey;
		dbmkey.dsize = sizeof(*pucmdkey);
		while (!delete(dbmkey))
		    ++pucmdkey->uc_ucmd;
		pucmdkey->uc_ucmd = 1;
	    }

	    /* insert new line and set up for next */
	    if (plogin->ln_input[0]) {
		dbmkey.dptr = (char *)pucmdkey;
		dbmkey.dsize = sizeof(*pucmdkey);
		dbmdata.dptr = plogin->ln_input;
		dbmdata.dsize = strlen(plogin->ln_input) + 1;
		if (store(dbmkey,dbmdata)) {
		    perror("usrcmd: can't store ucmd");
		    goto done;
		}
		pucmdkey->uc_ucmd += 1;
	    }
	    output(plogin,'C',0,">");
	}

	output(plogin,0,0,0);
#ifdef DEBUG
	VDBG("usrcmd return\n");
#endif
	return;

done:	plogin->ln_stat = NULL;
	if (plogin->ln_substat) {
	    free(plogin->ln_substat);
	    plogin->ln_substat = NULL;
	}
	output(plogin,'C',0,PROMPT);
	output(plogin,0,0,0);
#ifdef DEBUG
	VDBG("usrcmd return\n");
#endif
	return;
}
