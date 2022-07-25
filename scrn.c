/*
 * Spacewar - (play) screen output stuff
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
#include "universe.h"
#include "login.h"
#include "flds.h"
#include "sys.h"
#include "crft.h"
#include "build.h"
#include "aln.h"
#include "obj.h"
#include "torp.h"

static VOID dofld();
extern long gametime;

VOID background(pcrft)
struct crft *pcrft;
{
	static struct {
		char bg_row,bg_col,*bg_str;
	} bg[]={
		{0,	0,	"-------DIRECTION-------"},
		{2,	0,	"--------POSITION-------"},
		{4,	0,	"--------VELOCITY-------"},
		{7,	0,	"ANGLE"},
		{7,	6,	"---DST---"},
		{7,	16,	"SEE"},
		{7,	20,	"BAD"},
		{10,	0,	"---------HOMING--------"},
		{11,	0,	"#1:"},
		{12,	0,	"#2:"},
		{13,	0,	"#3:"},
		{11,	12,	"#4:"},
		{12,	12,	"#5:"},
		{13,	12,	"#6:"},
		{15,	0,	"RADIO:"},
		{0,	59,	"SYSTEM"},
		{0,	69,	"LEVEL"},
		{0,	75,	"%DMG"}
	};
	int i;
	register struct flddesc *pfld;

	for (pfld=flds+FLD_BACKGROUND,i=0;i < sizeof(bg)/sizeof(bg[0]);++i) {
		pfld->f_row = bg[i].bg_row;
		pfld->f_col = bg[i].bg_col;
		output(pcrft->cr_lgn,'L',FLD_BACKGROUND,bg[i].bg_str);
	}
}

VOID nums(pcrft)
register struct crft *pcrft;
{
	int i,j;
	struct sys *psys;

	/* direction */
	dofld(pcrft,FLD_DIR1,0,BIT_DIR1,DIV(pcrft->cr_dir[1],DEGTORAD));
	dofld(pcrft,FLD_DIR2,0,BIT_DIR2,DIV(pcrft->cr_dir[2],DEGTORAD));
	if (pcrft->cr_ffwd)
	    dofld(pcrft,FLD_AUTOFFWD,0,BIT_AUTOFFWD,"FFWD");
	else if (pcrft->cr_auto.ip_ptr)
	    dofld(pcrft,FLD_AUTOFFWD,0,BIT_AUTOFFWD,"AUTO");
	else
	    dofld(pcrft,FLD_AUTOFFWD,0,BIT_AUTOFFWD,"");

	/* position */
	dofld(pcrft,FLD_PN1,0,BIT_PN1,pcrft->cr_pstn[0]);
	dofld(pcrft,FLD_PN2,0,BIT_PN2,pcrft->cr_pstn[1]);
	dofld(pcrft,FLD_PN3,0,BIT_PN3,pcrft->cr_pstn[2]);

	/* velocity */
	dofld(pcrft,FLD_VEL1,0,BIT_VEL1,pcrft->cr_vel[0]);
	dofld(pcrft,FLD_VEL2,0,BIT_VEL2,pcrft->cr_vel[1]);
	dofld(pcrft,FLD_VEL3,0,BIT_VEL3,pcrft->cr_vel[2]);

	/* viewscreen attributes */
	dofld(pcrft,FLD_VANGL,0,BIT_VANGL,DIV(pcrft->cr_vang,DEGTORAD));
	dofld(pcrft,FLD_VDIST,0,BIT_VDIST,pcrft->cr_vdst);

	/* sensors */
	dofld(pcrft,FLD_SSEE,0,BIT_SSEE,pcrft->cr_sens[0]);
	dofld(pcrft,FLD_SBAD,0,BIT_SBAD,pcrft->cr_sens[1]);

	/* homing */
	for (i=0;i < MHOM;++i)
	    if (pcrft->cr_hom[i].ip_ptr)
		dofld(pcrft,FLD_HOMCHAN+i,0,BIT_HOMCHAN+i,pcrft->cr_hdst[i]);
	    else {
		char *savfmt;
		struct flddesc *pfld=flds+(FLD_HOMCHAN+i);
		savfmt = pfld->f_fmt;
		pfld->f_fmt = "%-7.7s";
		dofld(pcrft,FLD_HOMCHAN+i,0,BIT_HOMCHAN+i,"");
		pfld->f_fmt = savfmt;
	    }

	/* subsystems */
	for (j=0,psys=pcrft->cr_sys,i=0;i < MSYS;++psys,++i) {
	    if (!psys->s_cap) continue; /* not in this craft */
	    dofld(pcrft,FLD_SNAME,i,BIT_SNAME,++j,subsysnam[i]);
	    dofld(pcrft,FLD_SLEVEL,i,BIT_SLEVEL,psys->s_lvl);
	    dofld(pcrft,FLD_SDMG,i,BIT_SDMG,psys->s_dmg);
	}

	/* time */
	output(pcrft->cr_lgn,'H',FLD_TIME,gametime);
}

/*VARARGS4*/
static VOID dofld(pcrft,fld,grp,bit,val1,val2,val3,val4)
struct crft *pcrft;
int fld,grp,bit;
int val1,val2,val3,val4;
{
	extern int nabit();

	bit += grp * flds[fld].f_grpw;
	if (nabit(pcrft->cr_chng,bit)) {
		output(pcrft->cr_lgn,'H',(grp<<8)|fld,val1,val2,val3,val4);
		bitoff(pcrft->cr_chng,bit);
	}
}

VOID view(pcrft)
register struct crft *pcrft;
{
	char vnew[15][31],buf[31+1];
	double vdst[15][31],tmpdst,tmpvec[3],hlfvang;
	dsplcmnt tmpdspl;
	long ldst;
	int row,col,savrow,savcol;
	register struct universe *puniv;
	static char vinit[]="\
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\
@@@@@@@@       |       @@@@@@@@\
@@@@@          |          @@@@@\
@@@            |            @@@\
@@             |             @@\
@              |              @\
@              |              @\
@-------------- --------------@\
@              |              @\
@              |              @\
@@             |             @@\
@@@            |            @@@\
@@@@@          |          @@@@@\
@@@@@@@@       |       @@@@@@@@\
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\
";

	/* init to 'nothing there' */
	bcopy((char *)vnew,vinit,sizeof(vnew));
	binit((char *)vdst,sizeof(vdst));
	savrow = flds[FLD_VIEWSCREEN].f_row;
	savcol = flds[FLD_VIEWSCREEN].f_col;

	/* special case: draw entire viewscreen */
	if (pcrft->cr_scrn[0][0] == NULL) {
	    buf[31] = NULL;
	    bcopy((char *)pcrft->cr_scrn,vinit,sizeof(pcrft->cr_scrn));
	    for (row=0;row < 15;++row) {
		bcopy(buf,pcrft->cr_scrn[row],31);
		flds[FLD_VIEWSCREEN].f_row = savrow + row;
		output(pcrft->cr_lgn,'L',FLD_VIEWSCREEN,buf);
	    }
	}

	/* every object except own ship */
	for (puniv=univlst;puniv < univlst+MAXUNIVERSE;++puniv) {
	    if (!puniv->uv_type) continue;
	    if (puniv->uv_ptr.uv_crft == pcrft) continue;

	    /* translate and rotate to center viewscreen on */
	    /* z-axis then see if its within cone of vision */
	    /*vdiff(puniv->uv_pstn,pcrft->cr_pstn,tmpvec);*/
	    tmpdspl = vdisp(puniv,pcrft->cr_lgn->ln_play.ip_ptr,'v');
	    vecmul(/*tmpvec*/tmpdspl.vec,pcrft->cr_rmat,tmpvec);
	    rttosp(tmpvec,tmpvec);
	    hlfvang = DIV(pcrft->cr_vang,2.);
	    if (SUB(hlfvang,tmpvec[2]) >= 0.) {

		/* determine where it shows on viewscreen */
		/* closer object wins if they coincide    */
		tmpdst = tmpvec[0];
		if (tmpdst == 0.) tmpdst = -1.;
		tmpvec[0] = DIV(tmpvec[2],hlfvang);
		tmpvec[2] = DIV(PI,2.);
		sptort(tmpvec,tmpvec);
		row = INT(MUL(tmpvec[1],7.)) + 7;
		col = INT(MUL(tmpvec[0],15.)) + 15;
		if (vdst[row][col] == 0. || SUB(vdst[row][col],tmpdst) > 0.) {
		    vdst[row][col] = tmpdst;
		    ldst = INT(tmpdst);
		    if (ldst <= pcrft->cr_vdst)
			vnew[row][col] = puniv->uv_pctr;
		    else if (ldst <= 5000)
			vnew[row][col] = '.';
		    else
			vdst[row][col] = 0.;
		}
	    }
	}

	/* correct actual viewscreen (only put out differences) */
	buf[1] = NULL;
	for (row=0;row < 15;++row) {
	    for (col=0;col < 31;++col) {
		if (pcrft->cr_scrn[row][col] != vnew[row][col]) {
		    flds[FLD_VIEWSCREEN].f_row = savrow + row;
		    flds[FLD_VIEWSCREEN].f_col = savcol + col;
		    buf[0] = pcrft->cr_scrn[row][col] = vnew[row][col];
		    output(pcrft->cr_lgn,(vdst[row][col] <= 0.) ? 'L' : 'H',
		    FLD_VIEWSCREEN,buf);
		}
	    }
	}

	flds[FLD_VIEWSCREEN].f_row = savrow;
	flds[FLD_VIEWSCREEN].f_col = savcol;
}
