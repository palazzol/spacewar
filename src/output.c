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

// add missing headers
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static char outbuf[80*24*2]="";
static void doout(),dowrite();
extern int errno;

/*VARARGS1*/
void output(register struct login *plogin, char mode, int fld, char *str, ...)
{
	int val2,val3,val4;
	char *s,buf[256],fmt[16],*so="",*se="",*tgoto();
	int grp;

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
		va_list(argp);
		va_start(argp, str);
		val2 = va_arg(argp, int);
		val3 = va_arg(argp, int);
		val4 = va_arg(argp, int);
		va_end(argp);
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
		    dowrite(plogin->ln_tty,outbuf,strlen(outbuf));
		    outbuf[0] = 0;
		}
		break;
	    default:
		write(2,"output: unknown mode\n",21);
		break;
	}
}

static void doout(fd,s)
short fd;
char *s;
{

#if 1 /* with buffering */
	if (strlen(outbuf)+strlen(s)+1 >= sizeof(outbuf)) {
	    dowrite(fd,outbuf,strlen(outbuf));
	    outbuf[0] = 0;
	}
	strcat(outbuf,s);

#else /* without buffering */
	dowrite(fd,s,strlen(s));
#endif /* with/without buffering */
}

#include <signal.h>
static void dowrite(fd,s,ls)
int fd,ls;
char *s;
{
	while (write(fd,s,ls) == -1 && errno == EINTR);
}

