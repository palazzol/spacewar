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

#ifdef BSD
#	include <sys/ioctl.h>
	extern long lseek();
#else /* SYSIII SYSV */
#	include <fcntl.h>
#endif /* BSD SYSIII SYSV */

#include <signal.h>
#include "spacewar.h"
#include "uio2.h"

struct uio2 uio;
int swpid,swlgnfd;
char *thistty;
static VOID fixttyexit(),catchsig();

main()
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
#ifdef BSD
	if (access(SWPIDFILE,0)) {
#else /* SYSIII SYSV */
	if (access(SWCOMFILE,0)) {
#endif /* BSD SYSIII SYSV */
tryagain:
		write(2,waitmsg,strlen(waitmsg));
		switch(fork()) {
			case -1: /* fork failed */
				perror("fork");
				exit(1);
			case 0: /* child */
#ifdef BSD
				for (i=0;i < 20;ioctl(i++,FIOCLEX,NULL));
				ioctl(2,FIONCLEX,NULL);
#else /* SYSIII SYSV */
				for (i=0;i < 20;fcntl(i++,F_SETFD,1));
				fcntl(2,F_SETFD,0);
#endif /* BSD SYSIII SYSV */
				close(2);
				lseek(open(SWERR,1),0L,2);
				environ = NULL;
				execle(SWGAME,"sw",(char *)0,(char *)0);
				perror(SWGAME);
				kill(uio.uio2pid,SIGKILL); /* inform parent */
				exit(1);
			default: /* parent; wait for the game to get going */
#ifdef BSD
				while (access(SWPIDFILE,0))
#else /* SYSIII SYSV */
				while (access(SWCOMFILE,0))
#endif /* BSD SYSIII SYSV */
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

#ifdef BSD
	/* get pid of game to notify it of a new player */
	if ((i=open(SWPIDFILE,0)) < 0 ||
	read(i,&swpid,sizeof(swpid)) != sizeof(swpid) ||
	close(i)) {
		perror(SWPIDFILE);
		exit(1);
	}

	/* open notification information file */
	if ((swlgnfd=open(SWLGNFILE,1)) < 0) {
		perror(SWLGNFILE);
		exit(1);
	}
#else /* SYSIII SYSV */
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
#endif /* BSD SYSIII SYSV */

	/* get tty name and set permissions so */
	/* that the spacewar game can communicate */
	if (!(thistty = ttyname(0)) && !(thistty = ttyname(1))) {
		perror("ttyname");
		exit(1);
	}
	strcpy(uio.uio2tty,thistty);

#ifdef BSD
	/* notify spacewar of new player's tty and this program's pid */
	if (lseek(swlgnfd,0L,2) < 0L ||
	write(swlgnfd,&uio,sizeof(uio)) != sizeof(uio)) {
		perror(SWLGNFILE);
		fixttyexit(1);
	}
	if (kill(swpid,SIGTRAP)) {
		perror("notify spacewar");
		write(2,"Restarting!\n",12);
		close(swlgnfd);
		unlink(SWPIDFILE);
		unlink(SWLGNFILE);
		goto tryagain;
	}
#else /* SYSIII SYSV */
	if (write(swlgnfd,&uio,sizeof(uio)) != sizeof(uio)) {
		perror(SWCOMFILE);
		fixttyexit(1);
	}
#endif /* BSD SYSIII SYSV */

	/* wait for notification so that shell is tied up */
	uio.uio2tty[0] = '\0';
	for (;;) pause();
}

static VOID catchsig(sig)
int sig;
{
	signal(sig,catchsig);
	uio.uio2sig = sig;

#ifdef BSD
	if (lseek(swlgnfd,0L,2) < 0L ||
	write(swlgnfd,&uio,sizeof(uio)) != sizeof(uio)) {
		perror(SWLGNFILE);
		fixttyexit(1);
	}
	if (kill(swpid,SIGTRAP)) {
		perror("notify spacewar");
		fixttyexit(1);
	}
#else /* SYSIII SYSV */
	if (write(swlgnfd,&uio,sizeof(uio)) != sizeof(uio)) {
		perror(SWCOMFILE);
		fixttyexit(1);
	}
#endif /* BSD SYSIII SYSV */
}

static VOID fixttyexit(n)
int n;
{
	exit(n);
}
