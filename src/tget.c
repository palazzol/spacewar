/*
 * spacewar - simple simulation of termcap routines/database
 */

static struct {
	char *tnam;
	char *tcm,*tcl,*tce,*tso,*tse,*tus,*tue;
} *tptr,ttbl[] = {
	{"vt100","\001\033[%02d;%02dH",
	    "\033[;H\033[2J","\033[K","\033[7m","\033[m","\033[4m","\033[m"},
	{"tvi925","\040\033=%c%c",
	    "\032","\033T","\033G0","\033G<","\033)","\033("},
	{"h19","\040\033Y%c%c",
	    "\033E","\033K","\033p","\033q"},
	{0}
};

int tgetent(p1,termname)
char *p1,*termname;
{
	for (tptr=ttbl;tptr->tnam && strcmp(termname,tptr->tnam);++tptr) ;
	return((tptr->tnam) ? 1 : 0);
}

char *tgetstr(cap)
char *cap;
{
	if (!strcmp(cap,"cm")) // position to line l, column c
		return(tptr->tcm);
	if (!strcmp(cap,"cl")) // clear screen and home cursor
		return(tptr->tcl);
	if (!strcmp(cap,"ce")) // clear from cursor to end of line
		return(tptr->tce);
	if (!strcmp(cap,"so")) // enter standout mode
		return(tptr->tso);
	if (!strcmp(cap,"se")) // exit standout mode
		return(tptr->tse);
	if (!strcmp(cap,"us")) // enter underline mode
		return(tptr->tus);
	if (!strcmp(cap,"ue")) // exit underline mode
		return(tptr->tue);
	return(0);
}

char *tgoto(cm,x,y)
char *cm;
int x,y;
{
	static char buf[32];

	sprintf(buf,cm+1,y+*cm,x+*cm);
	return(buf);
}
