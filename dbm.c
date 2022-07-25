/*
 * NOTE: VMS only
 */

#include "dbm.h"
#include <rms.h>
#include <stdio.h>

static struct FAB dbmfab;
static struct RAB dbmrab;
static struct XABKEY dbmxab;
static char *dbmfile;
static char reckey[24],recdat[512];

dbminit(file)
char *file;
{
	int e;

	dbmfile = file;

	dbmfab=cc$rms_fab;
	dbmrab=cc$rms_rab;
	dbmxab=cc$rms_xabkey;

	/* open the file */
	dbmfab.fab$b_fac = FAB$M_PUT + FAB$M_GET + FAB$M_DEL + FAB$M_UPD;
	dbmfab.fab$b_shr = FAB$M_SHRPUT + FAB$M_SHRGET + FAB$M_SHRDEL +
	FAB$M_SHRUPD;
	dbmfab.fab$l_fna = file;
	dbmfab.fab$b_fns = strlen(file);
	dbmfab.fab$l_xab = &dbmxab;
	if ((e=sys$open(&dbmfab)) != RMS$_KFF && e != RMS$_NORMAL) {
		fprintf(stderr,"dbminit 1 rms=%x ",e);
		return(-1);
	}

	/* set up for record I/O */
	dbmrab.rab$l_fab = &dbmfab;
	dbmrab.rab$b_krf = 0;
	if ((e=sys$connect(&dbmrab)) != RMS$_NORMAL) {
		fprintf(stderr,"dbminit 2 rms=%x ",e);
		return(-1);
	}

	return(0);
}

dbmclose()
{
	int e;

	if ((e=sys$close(&dbmfab)) != RMS$_NORMAL) {
		fprintf(stderr,"dbmclose rms=%x ",e);
		return(-1);
	}
	return(0);
}

datum fetch(key)
datum key;
{
	datum rec;
	int e;

	rec.dptr = (char *)0;

	dbmrab.rab$b_rac = RAB$C_KEY;
	dbmrab.rab$w_usz = sizeof(recdat);
	dbmrab.rab$l_ubf = recdat;
	dbmrab.rab$l_kbf = key.dptr;
	dbmrab.rab$b_ksz = key.dsize;
	dbmrab.rab$l_rop = RAB$M_NLK;
	if ((e=sys$get(&dbmrab)) != RMS$_NORMAL && e != RMS$_RNF) {
		fprintf(stderr,"fetch rms=%x ",e);
	}
	if (e == RMS$_NORMAL) {
		rec.dptr = dbmrab.rab$l_rbf + sizeof(reckey);
		rec.dsize = dbmrab.rab$w_rsz - sizeof(reckey);
	}
	return(rec);
}

dbmdelete(key)
datum key;
{
	int e;

	dbmrab.rab$b_rac = RAB$C_KEY;
	dbmrab.rab$l_kbf = key.dptr;
	dbmrab.rab$b_ksz = key.dsize;
	dbmrab.rab$l_rop = RAB$M_FDL;
	if ((e=sys$find(&dbmrab)) != RMS$_NORMAL && e != RMS$_RNF) {
		fprintf(stderr,"dbmdelete 1 rms=%x ",e);
	}
	if (e == RMS$_NORMAL) {
		if ((e=sys$delete(&dbmrab)) != RMS$_NORMAL) {
			fprintf(stderr,"dbmdelete 2 rms=%x ",e);
			return(-1);
		}
		return(0);
	} else
		return(-1);
}

store(key, dat)
datum key, dat;
{
	int e;

	dbmrab.rab$b_rac = RAB$C_KEY;
	binit(recdat,sizeof(reckey));
	bcopy(recdat,key.dptr,key.dsize);
	bcopy(recdat+sizeof(reckey),dat.dptr,dat.dsize);
	dbmrab.rab$l_kbf = recdat;
	dbmrab.rab$b_ksz = sizeof(reckey);
	dbmrab.rab$l_rbf = recdat;
	dbmrab.rab$w_rsz = sizeof(reckey) + dat.dsize;
	dbmrab.rab$l_rop = RAB$M_LOA + RAB$M_UIF;
	if ((e=sys$put(&dbmrab)) != RMS$_NORMAL) {
		fprintf(stderr,"store rms=%x ",e);
		return(-1);
	}
	return(0);
}

datum firstkey()
{
	datum key;
	int e;

	key.dptr = (char *)0;

	dbmrab.rab$b_rac = RAB$C_SEQ;
	if ((e=sys$rewind(&dbmrab)) != RMS$_NORMAL) {
		fprintf(stderr,"firstkey 1 rms=%x ",e);
	}
	if (e == RMS$_NORMAL) {
		dbmrab.rab$l_rop = RAB$M_NLK;
		dbmrab.rab$w_usz = sizeof(recdat);
		dbmrab.rab$l_ubf = recdat;
		if ((e=sys$get(&dbmrab)) != RMS$_NORMAL && e != RMS$_EOF) {
			fprintf(stderr,"firstkey 2 rms=%x ",e);
		}
		if (e == RMS$_NORMAL) {
			key.dptr = dbmrab.rab$l_rbf;
			key.dsize = sizeof(reckey);
		}
	}
	return(key);
}

datum nextkey(xkey)
datum xkey;
{
	datum key;
	int e;

	key.dptr = (char *)0;

	dbmrab.rab$b_rac = RAB$C_SEQ;
	dbmrab.rab$l_rop = RAB$M_NLK;
	dbmrab.rab$w_usz = sizeof(recdat);
	dbmrab.rab$l_ubf = recdat;
	if ((e=sys$get(&dbmrab)) != RMS$_NORMAL && e != RMS$_EOF) {
		fprintf(stderr,"nextkey rms=%x ",e);
	}
	if (e == RMS$_NORMAL) {
		key.dptr = dbmrab.rab$l_rbf;
		key.dsize = sizeof(reckey);
	}
	return(key);
}
