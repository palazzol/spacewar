/*
 * Spacewar - program that reads a user's terminal and passes
 *	      it back to the spacewar game through a mailbox in
 *	      order to present spacewar with only one I/O channel to
 *	      read for user input eliminating polling
 *
 * Copyright 1985 obo Systems, Inc.
 * Copyright 1985 Dan Rosenblatt
 */

#include "spacewar.h"
#include "universe.h"
#include "login.h"
#include "uio.h"

#include <descrip.h>
#include <iodef.h>
#include <psldef.h>
#include <ssdef.h>
#include <signal.h>
static $DESCRIPTOR(swmlbx,SWCOMFILE);
static $DESCRIPTOR(tty,"TT");
static struct dsc$descriptor_s pmlbx;
static char buf[32];
static short swchan,tchan,pchan;
static struct uio ttyuio;
extern int errno;

main(argc,argv)
int argc;
char *argv[];
{
	struct login *plogin;
	int i,cleanup();

	/* get and save player's login pointer */
	if (argc != 3) exit(1);
	plogin = (struct login *) atoi(argv[1]);

	if ((i=sys$assign(&swmlbx,&swchan,PSL$C_USER,0)) != SS$_NORMAL) {
		sprintf(buf,"vmsrsw assign(swmlbx)=%d, errno=%d\n",i,errno);
		write(2,buf,strlen(buf));
		cleanup();
	}
	if ((i=sys$assign(&tty,&tchan,PSL$C_USER,0)) != SS$_NORMAL) {
		sprintf(buf,"vmsrsw assign(tty)=%d, errno=%d\n",i,errno);
		write(2,buf,strlen(buf));
		cleanup();
	}

	pmlbx.dsc$w_length = strlen(argv[2]);
	pmlbx.dsc$b_dtype = DSC$K_DTYPE_T;
	pmlbx.dsc$b_class = DSC$K_CLASS_S;
	pmlbx.dsc$a_pointer = argv[2];
	if ((i=sys$assign(&pmlbx,&pchan,PSL$C_USER,0)) != SS$_NORMAL) {
		sprintf(buf,"vmsrsw assign(pmlbx)=%d, errno=%d\n",i,errno);
		write(2,buf,strlen(buf));
		cleanup();
	}

	/* read tty and forward it on */
	signal(SIGALRM,cleanup);
	signal(SIGHUP,cleanup);
	for(;;) {
		if ((i=sys$qiow(0,tchan,IO$_READVBLK,0,0,0,ttyuio.uio_chrs,1,
		0,0,0,0)) == SS$_NORMAL) {
			switch(ttyuio.uio_chrs[0]) {
				case '\031':	/* ^Y */
				case '\032':	/* ^Z */
					ttyuio.uio_lgn = (struct login *) SIGINT;
					strcpy(ttyuio.uio_chrs,argv[2]);
					break;
				case '\003':	/* ^C */
					ttyuio.uio_lgn = (struct login *) SIGQUIT;
					strcpy(ttyuio.uio_chrs,argv[2]);
					break;
				default:
					ttyuio.uio_lgn = plogin;
					ttyuio.uio_chrs[1] = '\0';
					break;
			}
			alarm(10);
			if ((i=sys$qiow(0,swchan,IO$_WRITEVBLK,0,0,0,&ttyuio,
			sizeof(ttyuio),0,0,0,0)) != SS$_NORMAL) {
				sprintf(buf,"vmsrsw qiow(WRITE)=%d, errno=%d\n",
				i,errno);
				write(2,buf,strlen(buf));
				cleanup();
			}
			alarm(0);
		}
	}
}

cleanup()
{
	ttyuio.uio_lgn = (struct login *)SIGHUP;
	strcpy(ttyuio.uio_chrs,pmlbx.dsc$a_pointer);
	sys$qiow(0,swchan,IO$_WRITEVBLK+IO$M_NOW,0,0,0,&ttyuio,sizeof(ttyuio),
	0,0,0,0);
	sys$qiow(0,pchan,IO$_WRITEVBLK+IO$M_NOW,0,0,0,"ShUtDoWn",9,0,0,0,0);
	exit(SS$_ABORT);
}
