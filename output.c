/*
 * Spacewar - do output to a terminal
 *	      output is built with control/leadin characters
 *		as needed for terminal type and is buffered
 *	      modes are: Delete Erase Converse High Low Beep and (null)
 *
 * Copyright 1984 obo Systems, Inc.
 * Copyright 1984 Dan Rosenblatt
 */

#include <errno.h>
#include "spacewar.h"
#include "universe.h"
#include "login.h"
#include "sys.h"
#define FLDS
#include "flds.h"
#ifdef VMS
#	include <iodef.h>
#	include <ssdef.h>
#endif /* VMS */

static char outbuf[80*24*2]="";
static VOID doout(),dowrite();
extern int errno;

/*VARARGS1*/
VOID output(plogin,mode,fld,str,val2,val3,val4)
register struct login *plogin;
char mode;
int fld;
char *str;
int val2,val3,val4;
{
	char *s,buf[256],fmt[16],*so="",*se="",*tgoto();
	int grp;
#ifdef VMS
	int i;
#endif /* VMS */

	switch(mode) {
	    case 'B':
		doout(plogin->ln_tty,"\007");
		break;
	    case 'C':
		doout(plogin->ln_tty,str);
		break;
	    case 'D':
		s = tgoto(plogin->ln_tcm,0,23);
		doout(plogin->ln_tty,s);
		doout(plogin->ln_tty,plogin->ln_tce);
		break;
	    case 'E':
		if (s=plogin->ln_tcl)
		    doout(plogin->ln_tty,s);
		break;
	    case 'H':
	    case 'L':
		if ((plogin->ln_rvslh && mode == 'L') ||
		(!plogin->ln_rvslh && mode == 'H')) {
		    so = plogin->ln_tso;
		    se = plogin->ln_tse;
		}
		grp = (fld>>8)&0xff;
		fld &= 0xff;
		sprintf(fmt,"%%s%%s%s",flds[fld].f_fmt); /* no overflow check */
		sprintf(buf,fmt,
		tgoto(plogin->ln_tcm,flds[fld].f_col,flds[fld].f_row+grp),
		so,str,val2,val3,val4);
		strcat(buf,se);
		doout(plogin->ln_tty,buf);
		break;
	    case 0:
		if (plogin->ln_play.ip_ptr) {
		    s = tgoto(plogin->ln_tcm,strlen(plogin->ln_input),23);
		    doout(plogin->ln_tty,s);
		}
		if (outbuf[0]) {
#ifdef VMS
		    if ((i=sys$qiow(0,plogin->ln_tty,IO$_WRITEVBLK+IO$M_NOW,
		    0,0,0,outbuf,strlen(outbuf)+1,0,0,0,0)) != SS$_NORMAL) {
			perror("output qiow 1");
#ifdef DEBUG
			VDBG("output qiow(WRITEVBLK)=%d, errno=%d\n",i,errno);
#endif
		    }
#else /* BSD SYSIII SYSV */
		    dowrite(plogin->ln_tty,outbuf,strlen(outbuf));
#endif /* VMS BSD SYSIII SYSV */
		    outbuf[0] = NULL;
		}
		break;
	    default:
		write(2,"output: unknown mode\n",21);
		break;
	}
}

static VOID doout(fd,s)
short fd;
char *s;
{
#ifdef VMS
	int i;
#endif /* VMS */

#if 1 /* with buffering */
	if (strlen(outbuf)+strlen(s)+1 >= sizeof(outbuf)) {
#ifdef VMS
	    if ((i=sys$qiow(0,fd,IO$_WRITEVBLK+IO$M_NOW,0,0,0,
	    outbuf,strlen(outbuf)+1,0,0,0,0)) != SS$_NORMAL) {
		perror("output qiow 2");
#ifdef DEBUG
		VDBG("output qiow(WRITEVBLK)=%d, errno=%d\n",i,errno);
#endif
	    }
#else /* BSD SYSIII SYSV */
	    dowrite(fd,outbuf,strlen(outbuf));
#endif /* VMS BSD SYSIII SYSV */
	    outbuf[0] = NULL;
	}
	strcat(outbuf,s);

#else /* without buffering */
#ifdef VMS
	if ((i=sys$qiow(0,fd,IO$_WRITEVBLK+IO$M_NOW,0,0,0,
	s,strlen(s)+1,0,0,0,0)) != SS$_NORMAL) {
	    perror("output qiow 3");
#ifdef DEBUG
	    VDBG("output qiow(WRITEVBLK)=%d, errno=%d\n",i,errno);
#endif
	}
#else /* BSD SYSIII SYSV */
	dowrite(fd,s,strlen(s));
#endif /* VMS BSD SYSIII SYSV */
#endif /* with/without buffering */
}

#ifndef VMS
#include <signal.h>
static VOID dowrite(fd,s,ls)
int fd,ls;
char *s;
{
	int oldmask;
#ifdef BSD
	oldmask = sigblock(1<<(SIGALRM-1));
#endif
	while (write(fd,s,ls) == -1 && errno == EINTR);
#ifdef BSD
	sigsetmask(oldmask);
#endif
}
#endif
