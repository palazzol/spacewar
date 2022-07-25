/*
 * Spacewar - include file that defines a player's login
 *	      (requires prior inclusion of universe.h)
 *
 * Copyright 1984 obo Systems, Inc.
 * Copyright 1984 Dan Rosenblatt
 */

struct login {
	short	ln_tty;		/* login terminal I/O channel */
#ifdef VMS
	int	ln_pid;
#else /* BSD SYSIII SYSV */
	short	ln_playpid;	/* login play process pid */
	short	ln_readpid;	/* login read process pid */
#endif /* VMS BSD SYSIII SYSV */
	char	ln_name[8+1];	/* login name */
	char	*ln_term;	/* login terminal name */
				/* login trm. characteristics */
	char	*ln_tcm,*ln_tcl,*ln_tce,*ln_tso,*ln_tse;
	char	ln_rvslh;	/* login reverse low/high screen display */
	char	ln_iomode;	/* login I/O mode */
	char	ln_crft[12+1];	/* login craft name */
	idxptr	ln_play;	/* login is playing */
	char	ln_stat;	/* login status (major command) */
	char	*ln_substat;	/* login substatus (command dependent) */
	char	ln_input[79+1];	/* login input buffer */
};

#define MAXLOGIN	30	/* same as MAXCRFT */

extern struct login loginlst[];
