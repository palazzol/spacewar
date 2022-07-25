/*
 * Spacewar - for deleting mailboxes left around if the game dies - VMS ONLY!
 *
 * Copyright 1985 obo Systems, Inc.
 * Copyright 1985 Dan Rosenblatt
 */

#include <stdio.h>

#include <descrip.h>
#include <ssdef.h>
#include <psldef.h>

main()
{
	char buf[128];
	struct dsc$descriptor_d mlbx;
	short chan;
	int e;

	printf("mailbox name>");
	scanf("%s",buf);

	mlbx.dsc$w_length = strlen(buf);
	mlbx.dsc$b_dtype = DSC$K_DTYPE_T;
	mlbx.dsc$b_class = DSC$K_CLASS_S;
	mlbx.dsc$a_pointer = buf;

	if ((e=sys$assign(&mlbx,&chan,PSL$C_USER,0)) != SS$_NORMAL) {
		printf("assign()=%d ",e);
		perror(buf);
		exit(SS$_ABORT);
	}
	if ((e=sys$delmbx(chan)) != SS$_NORMAL) {
		printf("delmbx()=%d ",e);
		perror(buf);
		exit(SS$_ABORT);
	}

	printf("mailbox %s deleted\n",buf);
	exit(SS$_NORMAL);
}
