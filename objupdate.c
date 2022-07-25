/*
 * Spacewar - update universe orbiting objects to external file
 *
 * Copyright 1985 obo Systems, Inc.
 * Copyright 1985 Dan Rosenblatt
 */

#include <stdio.h>
#include "spacewar.h"
#include "universe.h"
#include "obj.h"

VOID objupdate()
{
	char buf[128];
	FILE *fobj;
	register struct obj *p=objlst;

#ifdef DEBUG
	DBG("objupdate()\n");
#endif

	/* write to secondary file to be renamed */
	strcpy(buf,SWOBJ);
#ifndef VMS
	strcat(buf,"x");
#endif
	if (!(fobj = fopen(buf,"w"))) {
	    perror(buf);
#ifdef DEBUG
	    VDBG("objupdate return\n");
#endif
	    return;
	}

	/* write the exact number of objects */
	for (p=objlst;p < objlst+MAXOBJ;++p)
	    fprintf(fobj,"%ld\t%d\t%c\t%d\t%d\t%ld\t%.5f\t%.1f\t%.1f\t%.1f\n",
	    p->oj_mass,p->oj_rad,(p->oj_rep == ' ') ? '.' : p->oj_rep,
	    p->oj_octr.ip_ptr-univlst,p->oj_oprd,p->oj_orad,p->oj_ocrpt,
	    p->oj_optx,p->oj_opty,p->oj_optz);

	if (ferror(fobj) || fclose(fobj)) {
	    perror(buf);
#ifdef DEBUG
	    VDBG("objupdate return\n");
#endif
	    return;
	}

#ifndef VMS
	/* rename secondary file to be primary */
	if (unlink(SWOBJ) || link(buf,SWOBJ) || unlink(buf)) {
	    perror(buf);
	}
#endif

#ifdef DEBUG
	VDBG("objupdate return\n");
#endif
}
