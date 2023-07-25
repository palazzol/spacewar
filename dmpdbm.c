/*
 * Spacewar - dump all dbm(3) records
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
#include "spacewar.h"
#include "universe.h"
#include "login.h"
#include "sys.h"
#include "crft.h"
#include "mlbx.h"
#include "plyr.h"
#include "ucmd.h"

main(argc,argv)
int argc;
char *argv[];
{
	datum dbmkey,dbmdata;
	struct crftkey crk;
	struct crft crd;
	struct mlbxkey mbk;
	struct plyrkey plk;
	struct plyr pld;
	struct ucmdkey uck;
	struct syskey sk;
	struct sys s;
	int dodtl = (argc > 1);
	char *asctime(),*ctime();

	if (dbminit(SWDATABASE)) {
		perror(SWDATABASE);
		exit(1);
	}

	for (dbmkey=firstkey();dbmkey.dptr;dbmkey=nextkey(dbmkey))
	    switch(dbmkey.dptr[0]) {

		case CRAFT:
		    bytecopy((char *)&crk,dbmkey.dptr,sizeof(crk));
		    printf("craft '%s' '%s': ",crk.cr_plyr,crk.cr_name);
		    dbmdata = fetch(dbmkey);
		    if (!dbmdata.dptr)
			printf("can't fetch\n");
		    else {
			bytecopy((char *)&crd,dbmdata.dptr,sizeof(crd));
			printf("%d %ld %ld %ld %ld %d %d\n",
			crd.cr_htyp,crd.cr_flsp,crd.cr_crew,crd.cr_pnts,
			crd.cr_time,crd.cr_kill,crd.cr_dock.ip_ofst);
			if (dodtl) {
			    printf(
			    "      (%g %g %g) (%g %g %g) (%g %g %g) (%g %g %g)\n",
			    crd.cr_pstn[0],crd.cr_pstn[1],crd.cr_pstn[2],
			    crd.cr_dir[0],crd.cr_dir[1],crd.cr_dir[2],
			    crd.cr_vel[0],crd.cr_vel[1],crd.cr_vel[2],
			    crd.cr_thr[0],crd.cr_thr[1],crd.cr_thr[2]);
			    printf("      %d %d %d %d %d %d %d %d %g %ld %d\n",
			    crd.cr_hom[0].ip_ofst,crd.cr_hom[1].ip_ofst,
			    crd.cr_hom[2].ip_ofst,crd.cr_hom[3].ip_ofst,
			    crd.cr_hom[4].ip_ofst,crd.cr_hom[5].ip_ofst,
			    crd.cr_auto.ip_ofst,crd.cr_ffwd,crd.cr_vang,
			    crd.cr_vdst,crd.cr_plvl);
			}
		    }
		    break;

		case MLBX:
		    bytecopy((char *)&mbk,dbmkey.dptr,sizeof(mbk));
		    printf("mlbx '%s' %d: ",mbk.mb_plyr,mbk.mb_mlbx);
		    dbmdata = fetch(dbmkey);
		    if (!dbmdata.dptr)
			printf("can't fetch\n");
		    else
			printf("'%s'\n",dbmdata.dptr);
		    break;

		case PLYR:
		    bytecopy((char *)&plk,dbmkey.dptr,sizeof(plk));
		    printf("plyr '%s': ",plk.pl_name);
		    dbmdata = fetch(dbmkey);
		    if (!dbmdata.dptr)
			printf("can't fetch\n");
		    else {
			bytecopy((char *)&pld,dbmdata.dptr,sizeof(pld));
			printf("'%s' %d {%.24s} %d %d %d %d %ld %ld %ld\n",
			pld.pl_passwd,pld.pl_numlgn,
#ifdef VMS
			ctime(&pld.pl_lstlgn),
#else /* BSD SYSIII SYSV */
			asctime(localtime(&pld.pl_lstlgn)),
#endif /* VMS BSD SYSIII SYSV */
			pld.pl_frstml,pld.pl_seenml,pld.pl_lstml,
			pld.pl_slst,pld.pl_klst,pld.pl_plst,pld.pl_tlst);
		    }
		    break;

		case UCMD:
		    bytecopy((char *)&uck,dbmkey.dptr,sizeof(uck));
		    printf("ucmd '%s' '%s' %d: ",uck.uc_plyr,
		    uck.uc_name,uck.uc_ucmd);
		    dbmdata = fetch(dbmkey);
		    if (!dbmdata.dptr)
			printf("can't fetch\n");
		    else
			printf("'%s'\n",dbmdata.dptr);
		    break;

		case SUBSYS:
		    bytecopy((char *)&sk,dbmkey.dptr,sizeof(sk));
		    printf("sys '%s' '%s' %d: ",sk.s_plyr,sk.s_crft,
		    sk.s_type);
		    dbmdata = fetch(dbmkey);
		    if (!dbmdata.dptr)
			printf("can't fetch\n");
		    else {
			bytecopy((char *)&s,dbmdata.dptr,sizeof(s));
			printf("%d %d %d %d %d\n",s.s_pct,s.s_edmg,s.s_dmg,
			s.s_lvl,s.s_cap);
		    }
		    break;

		default:
		    printf("%c: unknown\n",dbmkey.dptr[0]);
		    break;
	    }
}
