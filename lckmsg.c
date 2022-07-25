/*
 * Spacewar - return a lock message if the system is locked (no play allowed)
 *
 * Copyright 1985 obo Systems, Inc.
 * Copyright 1985 Dan Rosenblatt
 */

#include "spacewar.h"
#ifndef VMS
#include <sys/types.h>
#else /* BSD SYSIII SYSV */
#include <types.h>
#endif /* VMS */
#include <time.h>

static char *msg;	/* NULL if normal locking applies */
			/* ptr to NULL str if unlocked by SWMASTER */
			/* ptr to text if locked by SWMASTER */

char *lckmsg()
{
	time_t clock;
	struct tm *curtm,*localtime();

#ifdef DEBUG
	DBG("lckmsg()\n");
#endif

	/* controlled by SWMASTER */
	if (msg)
	    return((*msg) ? msg : NULL);

	/* get current date&time */
	time(&clock);
	curtm = localtime(&clock);

	/* OK if Sat or Sun */
	if (curtm->tm_wday == 0 || curtm->tm_wday == 6)
	    return(NULL);

	/* OK if before 8AM or after 5PM */
	if (curtm->tm_hour < 8 || curtm->tm_hour >= 17)
	    return(NULL);

	/* OK if during lunch: 1130PM to 1PM */
	if (curtm->tm_hour < 13 && curtm->tm_hour*100 + curtm->tm_min >= 1130)
	    return(NULL);

	return("A lockout exists from 0800-1130,1300-1700 Mon-Fri");
}

VOID prvlck(s)
char *s;
{
	msg = s;
}
