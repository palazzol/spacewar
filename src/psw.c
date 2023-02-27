/*
 * Spacewar - program that a user runs to play spacewar
 *	      start up the game if its not running
 *	      set tty permissions so that spacewar can communicate
 *	      notify spacewar of a new player
 *	      pause in order to tie up the shell
 *
 * Copyright 1984 obo Systems, Inc.
 * Copyright 1984 Dan Rosenblatt
 */

#include <fcntl.h>
#include <signal.h>
#include "spacewar.h"
#include "uio2.h"

// add missing headers
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

struct uio2 uio;
int swpid,swlgnfd;
char *thistty;
static void fixttyexit(),catchsig();

int main()
{
	int i;
	char *ttyname();
	static char waitmsg[]="\nJust a moment while the game cranks up ...\n";
	static char backmsg[]="Doesn't make sense to run in background\n";
	extern char **environ;

	/* if newsfile readable, show it */
	if ((i=open(SWNEWS,0)) >= 0) {
		char buf[1024];
		int j;
		while ((j=read(i,buf,sizeof(buf))) > 0)
			write(2,buf,j);
		close(i);
	}
	uio.uio2pid = getpid();

	/* if spacewar not running, run it */
	if (access(SWCOMFILE,0)) {
tryagain:
		write(2,waitmsg,strlen(waitmsg));
		switch(fork()) {
			case -1: /* fork failed */
				perror("fork");
				exit(1);
			case 0: /* child */
				for (i=0;i < 20;fcntl(i++,F_SETFD,1));
				fcntl(2,F_SETFD,0);
				close(2);
				lseek(open(SWERR,1),0L,2);
				environ = NULL;
				execle(SWGAME,"sw",(char *)0,(char *)0);
				perror(SWGAME);
				kill(uio.uio2pid,SIGKILL); /* inform parent */
				exit(1);
			default: /* parent; wait for the game to get going */
				while (access(SWCOMFILE,0))
					sleep(2);
				break;
		}
	}

	/* trap user signals, termination signal */
	/* (insure not running in background)    */
	if (signal(SIGINT,catchsig) == SIG_IGN) {
		write(2,backmsg,strlen(backmsg));
		exit(1);
	}
	signal(SIGHUP,catchsig);
	signal(SIGQUIT,catchsig);
	signal(SIGTERM,fixttyexit);

	/* open communication file */
	if ((swlgnfd=open(SWCOMFILE,O_WRONLY|O_NDELAY)) < 0) {
		perror("notify spacewar");
		write(2,"Restarting!\n",12);
		unlink(SWCOMFILE);
		goto tryagain;
		/*perror(SWCOMFILE);
		 *exit(1);
		 */
	}

	/* get tty name and set permissions so */
	/* that the spacewar game can communicate */
	if (!(thistty = ttyname(0)) && !(thistty = ttyname(1))) {
		perror("ttyname");
		exit(1);
	}
	strcpy(uio.uio2tty,thistty);

	if (write(swlgnfd,&uio,sizeof(uio)) != sizeof(uio)) {
		perror(SWCOMFILE);
		fixttyexit(1);
	}

	/* wait for notification so that shell is tied up */
	uio.uio2tty[0] = '\0';
	for (;;) pause();
}

static void catchsig(sig)
int sig;
{
	signal(sig,catchsig);
	uio.uio2sig = sig;

	if (write(swlgnfd,&uio,sizeof(uio)) != sizeof(uio)) {
		perror(SWCOMFILE);
		fixttyexit(1);
	}
}

static void fixttyexit(n)
int n;
{
	exit(n);
}
