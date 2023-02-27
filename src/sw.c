/*
 * Spacewar - main for spacewar game
 *
 * Copyright 1984 obo Systems, Inc.
 * Copyright 1984 Dan Rosenblatt
 */

#include <signal.h>
#include "spacewar.h"

// add missing headers
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <stdio.h>
#include <dbm.h>
#include <stdarg.h>

int numpling;

#include <sys/types.h>
#include <sys/stat.h>

// add missing headers
#include <errno.h>

extern int doproctrap,doupdate;
static int dbglvl = 0;
static void catchtrp(),catchalrm();

int main(argc,argv)
int argc;
char *argv[];
{
	extern void proctrap(),shutdown(),cmd();

	if (argc > 1) dbglvl = atoi(argv[1]);

	/* insure running in background */
	/*if (fork() > 0) exit(0);*/

	/* ignore interrupts, shutdown on terminate */
	/* break connection with controlling tty */
	/* close unneccesary files */
	signal(SIGHUP,SIG_IGN);
	signal(SIGINT,SIG_IGN);
	signal(SIGQUIT,SIG_IGN);
	signal(SIGTERM,shutdown);
#ifdef SIGTSTP
	signal(SIGTSTP,SIG_IGN);
#endif
#ifdef SIGTTIN
	signal(SIGTTIN,SIG_IGN);
#endif
#ifdef SIGTTOU
	signal(SIGTTOU,SIG_IGN);
#endif
	setpgid(getpid(),getpid());
	close(0);
	close(1);

	/* set up objects and aliens */
	srand(time(0));
	objinit();
	alninit();

	/* set up readsw pipe/named pipe/mailbox  */
	if (mknod(SWCOMFILE,0666+S_IFIFO,0) ||
	open(SWCOMFILE,0) != 0 ||
	open(SWCOMFILE,1) != 1) {
		perror(SWCOMFILE);
		exit(1);
	}

	/* open dbm(3) file */
	if (dbminit(SWDATABASE)) {
		perror(SWDATABASE);
		if (unlink(SWCOMFILE)) perror(SWCOMFILE);
		exit(1);
	}

	/* catch asynchronous event notification from playsw */

	/* trap alarm to update universe */
	signal(SIGALRM,catchalrm);

	/*******************/
	/* MAIN PROCESSING */
	/*******************/

	/* get and process commands and interrupts */
	for (;;) {
		cmd();
		if (doupdate < 0) {
			doproctrap = 0;
			update();
			if (doproctrap == 0) doproctrap = 1;
			doupdate = 1;
		}
	}

}

void firstplyr()
{catchalrm();}

static void catchalrm()
{

	VDBG("catchalrm\n");
	signal(SIGALRM,catchalrm);
	if (doproctrap > 0 && doupdate > 0) {
		doproctrap = 0;
		update();
		if (doproctrap == 0) doproctrap = 1;
	} else
		doupdate = -1;
	if (numpling)
		alarm(1);
}

#ifdef DEBUG

#undef nullptr
#include <stdio.h>

/*VARARGS1*/
void DBG(char *fmt, ...)
{
	if (dbglvl > 0) {
		va_list argp;
		va_start(argp, fmt);
		vfprintf(stderr,fmt,argp);
		va_end(argp);
	}
}

/*VARARGS1*/
void VDBG(char *fmt, ...)
{
	if (dbglvl > 1) {
		va_list argp;
		va_start(argp, fmt);
		vfprintf(stderr,fmt,argp);
		va_end(argp);
	}
}

#endif
