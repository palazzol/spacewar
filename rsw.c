/*
 * Spacewar - program that reads a user's terminal and passes
 *	      it back to the spacewar game through a pipe in
 *	      order to present spacewar with only one file to
 *	      read for user input eliminating polling
 *
 * Copyright 1984 obo Systems, Inc.
 * Copyright 1984 Dan Rosenblatt
 */

#include "spacewar.h"
#include "universe.h"
#include "login.h"
#include "uio.h"

main(argc,argv)
int argc;
char *argv[];
{
	struct uio ttyuio;
	register int i;

	/* get, save, and clear player's login pointer */
	if (argc != 2) exit(1);
	ttyuio.uio_lgn = (struct login *) atoi(argv[1]);
	argv[1] = (char *) 0;

	/* read tty and forward it on */
	for(;;) {
		if ((i=read(0,ttyuio.uio_chrs,sizeof(ttyuio.uio_chrs)-1)) > 0) {
			ttyuio.uio_chrs[i] = '\0';
			write(1,&ttyuio,sizeof(ttyuio));
		}
	}
}
