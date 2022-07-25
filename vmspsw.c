/*
 * Spacewar - program that a user runs to play spacewar - VMS ONLY!
 *	      show newsfile
 *	      start up the game if its not running including mailbox creation
 *	      notify spacewar of a new player
 *	      assign screen mailbox
 *	      get plogin value for rsw
 *	      spawn rsw
 *	      set 'raw' mode
 *	      read screen mailbox continually looking for shutdown msg
 *
 * Copyright 1985 obo Systems, Inc.
 * Copyright 1985 Dan Rosenblatt
 */

#include "spacewar.h"
#include "uio.h"
#include <signal.h>
#include <stdio.h>

#include <descrip.h>
#include <ssdef.h>
#include <iodef.h>
#include <psldef.h>
#include <ttdef.h>
#include <prvdef.h>

main()
{
	int i,j,rswpid;
	static $DESCRIPTOR(swmlbx,SWCOMFILE);
	short swchan,chan;
	static char waitmsg[]="\nJust a moment while the game cranks up ...\n";
	static $DESCRIPTOR(swgame,SWGAME);
	static $DESCRIPTOR(swerr,SWERR);
	static $DESCRIPTOR(swname,"sw");
	struct dsc$descriptor_d mlbx;
	int prv[2];
	struct uio uio;
	char buf[128];
	extern int errno;
	static $DESCRIPTOR(tty,"TT");
	short tchan;
	unsigned short speed,parity,fill;
	struct {
		unsigned char class,type;
		unsigned short width;
		unsigned mode : 24;
		unsigned char length;
	} swtty,savtty;


	/* if newsfile readable, show it */
	if ((i=open(SWNEWS,0)) >= 0) {
		char buf[512];
		int j;
		while ((j=read(i,buf,sizeof(buf))) > 0)
			write(2,buf,j);
		close(i);
	}

	/* if spacewar not running, run it */
	if (sys$assign(&swmlbx,&swchan,PSL$C_USER,0) != SS$_NORMAL) {
		write(2,waitmsg,strlen(waitmsg));
		if ((i=sys$crembx(1,&swchan,0,0,0,PSL$C_USER,&swmlbx)) !=
		SS$_NORMAL) {
			perror("crembx 1");
			sprintf(buf,"vmspsw crembx()=%d, errno=%d\n",i,errno);
			write(2,buf,strlen(buf));
			exit(SS$_ABORT);
		}
		prv[0] = 1<<PRV$V_PRMMBX | 1<<PRV$V_SYSNAM;
		prv[1] = 0;
		if ((i=sys$creprc(0,&swgame,0,&swerr,&swerr,prv,0,&swname,15,
		0,0,512/*PRC$M_DETACH*/)) != SS$_NORMAL) {
			perror(SWGAME);
			sprintf(buf,"vmspsw creprc()=%d, errno=%d\n",i,errno);
			write(2,buf,strlen(buf));
			if ((i=sys$delmbx(swchan)) != SS$_NORMAL) {
				perror("delete mbx");
				sprintf(buf,"vmspsw delmbx()=%d, errno=%d\n",
				i,errno);
				write(2,buf,strlen(buf));
			}
			exit(SS$_ABORT);
		}
	}

	/* notify spacewar of a new player */
	sprintf(uio.uio_chrs,"sw%x",getpid());
	uio.uio_lgn = 0;
	if ((i=sys$qiow(0,swchan,IO$_WRITEVBLK,0,0,0,&uio,sizeof(uio),0,0,0,0))
	!= SS$_NORMAL) {
		sprintf(buf,"vmspsw qiow(WRITEVBLK)=%d, errno=%d\n",i,errno);
		write(2,buf,strlen(buf));
		exit(SS$_ABORT);
	}

	/* assign the screen mailbox */
	mlbx.dsc$w_length = strlen(uio.uio_chrs);
	mlbx.dsc$b_dtype = DSC$K_DTYPE_T;
	mlbx.dsc$b_class = DSC$K_CLASS_S;
	mlbx.dsc$a_pointer = uio.uio_chrs;
	for (j=10;j-- > 0;)
	    if ((i=sys$assign(&mlbx,&chan,PSL$C_USER,0)) != SS$_NORMAL) {
		if (j) {
		    sleep(1);
		    continue;
		}
		perror("assign mbx");
		sprintf(buf,"vmspsw assignmbx()=%d, errno=%d\n",i,errno);
		write(2,buf,strlen(buf));
		exit(SS$_ABORT);
	    } else
		break;

	/* get plogin value for rsw */
	if ((i=sys$qiow(0,chan,IO$_READVBLK,0,0,0,buf,sizeof(buf),0,0,0,0)) !=
	SS$_NORMAL) {
		sprintf(buf,"vmspsw qiow(READVBLK)=%d, errno=%d\n",i,errno);
		write(2,buf,strlen(buf));
	}

	/* spawn rsw */
	switch(rswpid=vfork()) {
		case -1: /* failed */
			perror("vfork");
			uio.uio_lgn = SIGHUP;
			if ((i=sys$qiow(0,swchan,IO$_WRITEVBLK,0,0,0,&uio,
			sizeof(uio),0,0,0,0)) != SS$_NORMAL) {
			    sprintf(buf,"vmspsw qiow(WRITEVBLK)=%d, errno=%d\n",
			    i,errno);
			    write(2,buf,strlen(buf));
			}
			exit(SS$_ABORT);

		case 0: /* child */
			execl(SWREAD,"vmsrsw",buf,uio.uio_chrs,0);
			perror(SWREAD);
			uio.uio_lgn = SIGHUP;
			if ((i=sys$qiow(0,swchan,IO$_WRITEVBLK,0,0,0,&uio,
			sizeof(uio),0,0,0,0)) != SS$_NORMAL) {
			    sprintf(buf,"vmspsw qiow(WRITEVBLK)=%d, errno=%d\n",
			    i,errno);
			    write(2,buf,strlen(buf));
			}
			exit(SS$_ABORT);
	}

	/* set 'raw' mode */
	if ((i=sys$assign(&tty,&tchan,PSL$C_USER,0)) != SS$_NORMAL) {
		sprintf(buf,"vmspsw assign()=%d, errno=%d\n",i,errno);
		write(2,buf,strlen(buf));
	}
	if ((i=sys$qiow(0,tchan,IO$_SENSEMODE,0,0,0,&savtty,0,&speed,&fill,
	&parity,0)) != SS$_NORMAL) {
		sprintf(buf,"vmspsw qiow(SENSE)=%d, errno=%d\n",i,errno);
		write(2,buf,strlen(buf));
	}
	swtty = savtty;
	swtty.mode |= TT$M_PASSALL+TT$M_NOECHO;
	if ((i=sys$qiow(0,tchan,IO$_SETMODE,0,0,0,&swtty,0,speed,fill,parity,
	0)) != SS$_NORMAL) {
		sprintf(buf,"vmspsw qiow(SET)=%d, errno=%d\n",i,errno);
		write(2,buf,strlen(buf));
	}

	/* read screen mailbox continually looking for shutdown msg */
	for (;;) {
		if ((i=sys$qiow(0,chan,IO$_READVBLK,0,0,0,buf,sizeof(buf),
		0,0,0,0)) != SS$_NORMAL) {
		    sprintf(buf,"vmspsw qiow(READVBLK)=%d, errno=%d\n",i,errno);
		    write(2,buf,strlen(buf));
		}
		if (!strcmp(buf,"ShUtDoWn")) break;
		write(1,buf,strlen(buf));
	}
	kill(rswpid,SIGTERM);

	/* reset terminal */
	if ((i=sys$qiow(0,tchan,IO$_SETMODE,0,0,0,&savtty,0,speed,fill,parity,
	0)) != SS$_NORMAL) {
		sprintf(buf,"vmspsw qiow(SET)=%d, errno=%d\n",i,errno);
		write(2,buf,strlen(buf));
	}

	exit(SS$_NORMAL);
}
