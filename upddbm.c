/*
 * Spacewar - update dbm(3) records
 *	      for deleting old players and all associated records
 *	      and for creating super and priviledged crafts
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
#include "mlbx.h"
#include "plyr.h"
#include "ucmd.h"

main()
{
	datum srchkey,dbmkey,dbmdata;
	struct crftkey crk;
	static struct crftkey zcrk;
	struct crft crd;
	struct mlbxkey mbk;
	struct plyrkey plk;
	struct plyr pld;
	struct ucmdkey uck;
	struct syskey sk;
	struct sys s;
	char *asctime(),*ctime(),*gets(),buf[32],dnam[100][8+1],*pnam;
	int i,nd=0,atoi();
	long atol();

	if (dbminit(SWDATABASE)) {
		perror(SWDATABASE);
		exit(1);
	}

	/* get all players */
	for (srchkey=firstkey();srchkey.dptr;srchkey=nextkey(srchkey)) {
	    if (srchkey.dptr[0] != PLYR)
		continue;

	    /* show player name, last login time, and # of logins */
	    bytecopy((char *)&plk,srchkey.dptr,sizeof(plk));
	    printf("'%s' ",plk.pl_name);
	    dbmdata = fetch(srchkey);
	    if (!dbmdata.dptr) {
		printf("- can't fetch\n");
		continue;
	    }
	    bytecopy((char *)&pld,dbmdata.dptr,sizeof(pld));
	    printf("{%.24s} %d ",
#ifdef VMS
	    ctime(&pld.pl_lstlgn),
#else /* BSD SYSIII SYSV */
	    asctime(localtime(&pld.pl_lstlgn)),
#endif /* VMS BSD SYSIII SYSV */
	    pld.pl_numlgn);

	    /* prompt for delete or craft name to endow */
getcrnam:   printf("(d/craftname)>");
	    gets(buf);

	    /* save player name to delete */
	    if (!strcmp(buf,"d"))
		strcpy(dnam[nd++],plk.pl_name);

	    /* try to get craft */
	    else if (strlen(buf) > 1) {
		crk = zcrk;
		crk.cr_crftkey = CRAFT;
		strcpy(crk.cr_plyr,plk.pl_name);
		strcpy(crk.cr_name,buf);
		dbmkey.dptr = (char *)&crk;
		dbmkey.dsize = sizeof(crk);
		dbmdata = fetch(dbmkey);
		if (!dbmdata.dptr) {
		    printf("No such craft\n");
		    goto getcrnam;	/* horrendous */
		}

		bytecopy((char *)&crd,dbmdata.dptr,dbmdata.dsize);
		printf("[htyp=%d] flsp=%ld crew=%ld plvl=%d\n",
		crd.cr_htyp,crd.cr_flsp,crd.cr_crew,crd.cr_plvl);
		printf("flsp>");
		if (strlen(gets(buf)) > 0) crd.cr_flsp = atol(buf);
		printf("crew>");
		if (strlen(gets(buf)) > 0) crd.cr_crew = atol(buf);
		printf("plvl>");
		if (strlen(gets(buf)) > 0) crd.cr_plvl = atoi(buf);
		dbmkey.dptr = (char *)&crk;
		dbmdata.dptr = (char *)&crd;
		if (store(dbmkey,dbmdata))
		    printf("Couldn't update craft\n");
	    }
	}

	printf("\nStarting delete loop\n\n");

	/* go through all records requesting ok to delete those matching dnam */
	for (srchkey=firstkey();srchkey.dptr;srchkey=nextkey(srchkey)) {

	    switch(srchkey.dptr[0]) {
		case CRAFT:
		    bytecopy((char *)&crk,srchkey.dptr,sizeof(crk));
		    pnam = crk.cr_plyr;
		    srchkey.dptr = (char *)&crk;
		    srchkey.dsize = sizeof(crk);
		    break;
		case MLBX:
		    bytecopy((char *)&mbk,srchkey.dptr,sizeof(mbk));
		    pnam = mbk.mb_plyr;
		    srchkey.dptr = (char *)&mbk;
		    srchkey.dsize = sizeof(mbk);
		    break;
		case PLYR:
		    bytecopy((char *)&plk,srchkey.dptr,sizeof(plk));
		    pnam = plk.pl_name;
		    srchkey.dptr = (char *)&plk;
		    srchkey.dsize = sizeof(plk);
		    break;
		case SUBSYS:
		    bytecopy((char *)&sk,srchkey.dptr,sizeof(sk));
		    pnam = sk.s_plyr;
		    srchkey.dptr = (char *)&sk;
		    srchkey.dsize = sizeof(sk);
		    break;
		case UCMD:
		    bytecopy((char *)&uck,srchkey.dptr,sizeof(uck));
		    pnam = uck.uc_plyr;
		    srchkey.dptr = (char *)&uck;
		    srchkey.dsize = sizeof(uck);
		    break;
		default:
		    printf("Unknown key: '%c'\n",srchkey.dptr[0]);
		    if (delete(srchkey))
			printf("Can't delete\n");
		    continue;
	    }

	    /* only if the name matches one in the list */
	    for (i=0;i < nd;++i)
		if (!strcmp(pnam,dnam[i]))
		    break;
	    if (i >= nd) continue;

	    switch(srchkey.dptr[0]) {
		case CRAFT:
		    printf("craft '%s' '%s'",crk.cr_plyr,crk.cr_name);
		    break;
		case MLBX:
		    printf("mlbx '%s' %d",mbk.mb_plyr,mbk.mb_mlbx);
		    break;
		case PLYR:
		    printf("plyr '%s'",plk.pl_name);
		    break;
		case SUBSYS:
		    printf("sys '%s' '%s' %d",sk.s_plyr,sk.s_crft,
		    sk.s_type);
		    break;
		case UCMD:
		    printf("ucmd '%s' '%s' %d",uck.uc_plyr,uck.uc_name,
		    uck.uc_ucmd);
		    break;
	    }
	    printf(" (d/)>");
	    if (!strcmp(gets(buf),"d") && delete(srchkey))
		printf("Can't delete\n");
	}
}
