/*
 * Spacewar - (re)build a ship
 *
 * Copyright 1984 obo Systems, Inc.
 * Copyright 1984 Dan Rosenblatt
 */

#include <sys/types.h>
#include <dbm.h>
#include "spacewar.h"
#include "universe.h"
#include "obj.h"
#include "login.h"
#include "sys.h"
#include "crft.h"
#include "build.h"

char *subsysnam[MAXSYS]= {
	"Hull",
	"Solar E.",
	"Antm. E.",
	"Warp",
	"Rockets",
	"Dilith.",
	"Phasers",
	"Shields",
	"Torps.",
	"Dmg.Con.",
	"Security"
};

struct sysc config[MAXSYS][MAXHTYP+1] = {	/* varies by system, hull */

/* Hull */{
	/* 0 */	{0},
	/* 1 */	{16500,	120,	0,	0,	40,	0,	15,	100},
	/* 2 */	{18000,	100,	0,	0,	40,	0,	12,	100},
	/* 3 */	{17500,	110,	0,	0,	40,	0,	9,	100},
	/* 4 */	{0,	0,	0,	0,	40,	0,	15,	100},
	/* 5 */	{0,	0,	0,	0,	35,	0,	12,	100},
	/* 6 */	{0,	0,	0,	0,	45,	0,	9,	100},
	/* 7 */	{0,	0,	0,	0,	40,	0,	10,	100},
	/* 8 */	{0,	0,	0,	0,	40,	0,	10,	100},
	/* 9 */	{0,	0,	0,	0,	40,	0,	10,	100}
	},

/* Solar E. */{
	/* 0 */	{0},
	/* 1 */ {250,	1,	450,	4,	100,	0,	35,	30},
	/* 2 */	{250,	1,	1000,	10,	100,	0,	50,	40},
	/* 3 */	{250,	1,	550,	5,	100,	0,	40,	35},
	/* 4 */	{0,	0,	0,	0,	100,	0,	35,	30},
	/* 5 */	{0,	0,	0,	0,	100,	0,	50,	40},
	/* 6 */	{0,	0,	0,	0,	100,	0,	40,	35},
	/* 7 */	{0,	0,	0,	0,	100,	0,	20,	80},
	/* 8 */	{0,	0,	0,	0,	100,	0,	25,	75},
	/* 9 */	{0,	0,	0,	0,	100,	0,	25,	70}
	},

/* Antm. E. */{
	/* 0 */	{0},
	/* 1 */ {500,	1,	2400,	8,	50,	0,	35,	100},
	/* 2 */	{500,	1,	1500,	5,	50,	0,	30,	100},
	/* 3 */	{500,	1,	3000,	10,	50,	0,	40,	100},
	/* 4 */	{0,	0,	0,	0,	70,	0,	50,	80},
	/* 5 */	{0,	0,	0,	0,	60,	0,	30,	100},
	/* 6 */	{0,	0,	0,	0,	65,	0,	40,	90},
	/* 7 */	{0,	0,	0,	0,	50,	0,	35,	70},
	/* 8 */	{0,	0,	0,	0,	50,	0,	45,	85},
	/* 9 */	{0,	0,	0,	0,	45,	0,	70,	100}
	},

/* Warp */{
	/* 0 */ {0},
	/* 1 */	{1000,	10,	2000,	20,	70,	3000,	3000,	100},
	/* 2 */	{1000,	10,	2500,	25,	70,	4000,	4000,	100},
	/* 3 */	{1000,	10,	5000,	40,	70,	7000,	7000,	100},
	/* 4 */	{0,	0,	0,	0,	70,	3000,	3000,	100},
	/* 5 */	{0,	0,	0,	0,	70,	4000,	4000,	90},
	/* 6 */	{0,	0,	0,	0,	70,	6000,	6000,	80},
	/* 7 */	{0,	0,	0,	0,	50,	2000,	2000,	100},
	/* 8 */	{0,	0,	0,	0,	60,	4000,	4000,	60},
	/* 9 */	{0,	0,	0,	0,	80,	10000,	10000,	50}
	},

/* Rockets */{
	/* 0 */	{0},
	/* 1 */	{200,	4,	5000,	20,	30,	7500,	15,	45},
	/* 2 */	{200,	4,	3750,	15,	30,	5625,	12,	30},
	/* 3 */	{200,	4,	2500,	10,	30,	3750,	9,	20},
	/* 4 */	{0,	0,	0,	0,	40,	10000,	15,	45},
	/* 5 */	{0,	0,	0,	0,	40,	10000,	12,	30},
	/* 6 */	{0,	0,	0,	0,	40,	10000,	9,	20},
	/* 7 */	{0,	0,	0,	0,	40,	10000,	20,	50},
	/* 8 */	{0,	0,	0,	0,	40,	10000,	25,	40},
	/* 9 */	{0,	0,	0,	0,	50,	10000,	8,	45}
	},

/* Dilith. */{
	/* 0 */	{0},
	/* 1 */	{500,	4,	4000,	10,	10,	5000,	5000,	50},
	/* 2 */	{500,	4,	4000,	10,	10,	5000,	5000,	50},
	/* 3 */	{500,	4,	4000,	10,	10,	5000,	5000,	50},
	/* 4 */	{0,	0,	0,	0,	10,	5000,	5000,	60},
	/* 5 */	{0,	0,	0,	0,	15,	5500,	5500,	50},
	/* 6 */	{0,	0,	0,	0,	10,	4500,	4500,	70},
	/* 7 */	{0,	0,	0,	0,	15,	5000,	5000,	55},
	/* 8 */	{0,	0,	0,	0,	20,	6000,	6000,	70},
	/* 9 */	{0,	0,	0,	0,	20,	5800,	5800,	65}
	},

/* Phasers */{
	/* 0 */	{0},
	/* 1 */	{500,	5,	2400,	8,	40,	0,	80,	70},
	/* 2 */	{500,	5,	2100,	7,	40,	0,	70,	80},
	/* 3 */	{500,	5,	3000,	10,	40,	0,	100,	100},
	/* 4 */	{0,	0,	0,	0,	40,	0,	150,	75},
	/* 5 */	{0,	0,	0,	0,	40,	0,	140,	80},
	/* 6 */	{0,	0,	0,	0,	40,	0,	160,	70},
	/* 7 */	{0,	0,	0,	0,	40,	0,	140,	60},
	/* 8 */	{0,	0,	0,	0,	45,	0,	170,	85},
	/* 9 */	{0,	0,	0,	0,	90,	0,	250,	100}
	},

/* Shields */{
	/* 0 */	{0},
	/* 1 */	{1500,	2,	700,	7,	20,	50,	70,	100},
	/* 2 */	{1500,	2,	1000,	10,	20,	50,	85,	90},
	/* 3 */	{1500,	2,	500,	5,	20,	50,	75,	90},
	/* 4 */ {0,	0,	0,	0,	20,	20,	70,	100},
	/* 5 */	{0,	0,	0,	0,	20,	20,	85,	90},
	/* 6 */	{0,	0,	0,	0,	20,	20,	60,	80},
	/* 7 */	{0,	0,	0,	0,	20,	20,	70,	80},
	/* 8 */	{0,	0,	0,	0,	50,	10,	50,	100},
	/* 9 */	{0,	0,	0,	0,	80,	10,	90,	75}
	},

/* Torps. */{
	/* 0 */	{0},
	/* 1 */	{1000,	6,	500,	5,	60,	35,	10,	20},
	/* 2 */	{1000,	6,	1000,	10,	60,	35,	20,	20},
	/* 3 */	{1000,	6,	600,	6,	60,	35,	12,	40}
	},

/* Dmg.Con. */{
	/* 0 */	{0},
	/* 1 */	{0,	1,	30,	30,	20,	0,	100,	100},
	/* 2 */	{0,	1,	30,	30,	20,	0,	100,	100},
	/* 3 */	{0,	1,	30,	30,	20,	0,	100,	100},
	/* 4 */	{0,	0,	0,	0,	20,	0,	80,	100},
	/* 5 */	{0,	0,	0,	0,	20,	0,	70,	100},
	/* 6 */	{0,	0,	0,	0,	20,	0,	80,	100},
	/* 7 */	{0,	0,	0,	0,	20,	0,	70,	100},
	/* 8 */	{0,	0,	0,	0,	20,	0,	80,	100},
	/* 9 */	{0,	0,	0,	0,	20,	0,	80,	100}
	},

/* Security */{
	/* 0 */	{0},
	/* 1 */	{0,	1,	30,	30,	20,	0,	100,	0},
	/* 2 */	{0,	1,	30,	30,	20,	0,	100,	0},
	/* 3 */	{0,	1,	30,	30,	20,	0,	100,	0}
	}
};

static struct crftkey getcrkey;
static struct crft getcrdat;
static struct syskey getskey;
static struct sys getsdat;
static datum dbmkey,dbmdata;
static struct sysc *getsc;
static int getsys();
static int getcrft();
static void fixdmg(),putcrft(),putsys(),delsys();
extern long atol();

// add missing headers
#include <string.h>

void build(plogin)
struct login *plogin;
{
	char buf[80+1];

	DBG("build(#%d/%s)\n",plogin-loginlst,plogin->ln_name);

	/****************/
	/* no craft yet */
	/****************/
	VDBG("build: #%d/%s doing %s/%d\n",plogin-loginlst,plogin->ln_name,
	plogin->ln_crft,(int)plogin->ln_substat);
	if (!plogin->ln_crft[0]) {

	    /* nothing - prompt for craft name */
	    if (!plogin->ln_input[0]) {
		VDBG("build: prompt for craft name\n");
		output(plogin,'C',0,
		"A single dot (.) on a line by itself terminates Build.\n\n");
		output(plogin,'C',0,"What is the name of your ship?");
		output(plogin,0,0,0);
		VDBG("build return\n");
		return;

	    /* craft name - store and get or create craft */
	    } else {
		VDBG("build: get/create craft '%s'\n",plogin->ln_input);
		plogin->ln_input[sizeof(plogin->ln_crft)-1] = 0;
		strcpy(plogin->ln_crft,plogin->ln_input);
		if (!strcmp(plogin->ln_input,".")) {
		    plogin->ln_crft[0] = 0;
		    plogin->ln_stat = 0;
		    plogin->ln_substat = NULL;
		    output(plogin,'C',0,PROMPT);
		    output(plogin,0,0,0);
		    VDBG("build return\n");
		    return;
		}

		/* exists */
		if (getcrft(plogin)) {
		    VDBG("build: craft exists\n");

		    /* has a valid hull type */
		    if (getcrdat.cr_htyp) {
			VDBG("build: has valid hull %d\n",getcrdat.cr_htyp);

			/* must be docked or priviledged */
			if ((!getcrdat.cr_dock.ip_ofst ||
			getcrdat.cr_dock.ip_ofst >= MAXOBJ) &&
			!getcrdat.cr_plvl) {
			    VDBG("build: docked=%d plvl=%d\n",
			    getcrdat.cr_dock.ip_ofst,getcrdat.cr_plvl);
			    plogin->ln_crft[0] = 0;
			    plogin->ln_stat = 0;
			    plogin->ln_substat = NULL;
			    output(plogin,'C',0,"Must be docked to rebuild.");
			    output(plogin,'C',0,PROMPT);
			    output(plogin,0,0,0);
			    VDBG("build return\n");
			    return;

			/* docked - fix hull damage; set next subsys prompt */
			} else {
			    VDBG("build: docked=%d plvl=%d\n",
			    getcrdat.cr_dock.ip_ofst,getcrdat.cr_plvl);
			    if (!getsys(plogin,HULL)) {
				perror("build: docked and can't find HULL");
				plogin->ln_crft[0] = 0;
				plogin->ln_stat = 0;
				plogin->ln_substat = NULL;
				output(plogin,'C',0,PROMPT);
				output(plogin,0,0,0);
				VDBG("build return\n");
				return;
			    }
			    fixdmg();
			    putsys(plogin);
			    plogin->ln_substat = (char *)(HULL+1);
			}

		    /* invalid hull - set up prompting for hull type */
		    } else {
			VDBG("build: has invalid hull %d\n",getcrdat.cr_htyp);
			plogin->ln_substat = (char *)(HULL);
		    }

		/* craft doesn't exist - create it, */
		/* set up prompting for hull type */
		} else {
		    VDBG("build: creating craft\n");
		    binit((char *)&getcrdat,sizeof(getcrdat));
		    getcrdat.cr_dock.ip_ofst = -1;
		    dbmdata.dptr = (char *)&getcrdat;
		    dbmdata.dsize = CRDATSIZ;
		    if (store(dbmkey,dbmdata)) {
			output(plogin,'C',0,
			"(sigh) database collision - try another ship name\n\n\
What is the name of your ship?");
			output(plogin,0,0,0);
			plogin->ln_crft[0] = 0;
			VDBG("build return\n");
			return;
		    }
		    plogin->ln_substat = (char *)(HULL);
		}
	    }

	/******************************/
	/* get craft - error if can't */
	/******************************/
	} else if (!getcrft(plogin)) {
		perror("build: can't find craft\n");
		plogin->ln_crft[0] = 0;
		plogin->ln_stat = 0;
		plogin->ln_substat = NULL;
		output(plogin,'C',0,PROMPT);
		output(plogin,0,0,0);
		VDBG("build return\n");
		return;

	/*************************************/
	/* no hull type - input is hull type */
	/*************************************/
	} else if (!getcrdat.cr_htyp) {
		VDBG("build: craft given hull '%s'\n",plogin->ln_input);

		if (!strcmp(plogin->ln_input,".")) {
		    plogin->ln_crft[0] = 0;
		    plogin->ln_stat = 0;
		    plogin->ln_substat = NULL;
		    output(plogin,'C',0,PROMPT);
		    output(plogin,0,0,0);
		    VDBG("build return\n");
		    return;
		}

		/* must be 1, 2, or 3 */
		if (strcmp("1",plogin->ln_input) &&
		strcmp("2",plogin->ln_input) &&
		strcmp("3",plogin->ln_input))
			output(plogin,'C',0,"Bad hull type\n\n");

		/* update craft */
		/* create hull subsystem */
		/* set next subsys prompt */
		else {
			getcrdat.cr_htyp = plogin->ln_input[0] - '0';
			getsc = &config[HULL][getcrdat.cr_htyp];
			getcrdat.cr_flsp = getsc->sc_bsp;
			getcrdat.cr_crew = getsc->sc_bcr;
			putcrft(plogin);

			getsys(plogin,HULL); /* should fail */
			getsdat.s_pct = getsc->sc_rpct;
			getsdat.s_edmg = getsc->sc_edmg;
			getsdat.s_dmg = 0;
			getsdat.s_lvl = getsc->sc_ilvl;
			getsdat.s_cap = getsc->sc_cap;
			putsys(plogin);

			plogin->ln_substat = (char *)(HULL+1);
		}

	/**********************************************/
	/* null response - fix damage and leave as is */
	/**********************************************/
	} else if (!plogin->ln_input[0]) {
		VDBG("build: null response for subsys %d\n",
		(int)plogin->ln_substat);

		/* only if system exists */
		if (getsys(plogin,(int)plogin->ln_substat)) {
			fixdmg();
			putsys(plogin);
		}

		plogin->ln_substat = (char *)((int)plogin->ln_substat + 1);

	/*****************************************************************/
	/* zero - delete subsystem if at starbase, zero it out otherwise */
	/*****************************************************************/
	} else if (!strcmp("0",plogin->ln_input)) {
	    VDBG("build: zero percent for subsys %d\n",(int)plogin->ln_substat);

	    /* only if system exists */
	    if (getsys(plogin,(int)plogin->ln_substat)) {
		VDBG("build: subsys exists\n");
		fixdmg();

		/* delete if priviledged or brand new or */
		/* docked at starbase freeing up space and crew */
		getsc = &config[getskey.s_type][getcrdat.cr_htyp];
		if (getcrdat.cr_plvl || getcrdat.cr_dock.ip_ofst < 0 ||
		(getcrdat.cr_dock.ip_ofst > 0 &&
		univlst[getcrdat.cr_dock.ip_ofst].uv_pctr == '#')) {
		    VDBG("build: deleting subsys\n");
		    getcrdat.cr_flsp += getsc->sc_bsp +
		    (getsc->sc_fsp * getsdat.s_pct) / 100L;
		    getcrdat.cr_crew += getsc->sc_bcr +
		    (getsc->sc_fcr * getsdat.s_pct) / 100L;
		    delsys(plogin);

		/* otherwise set to 0 keeping base space and */
		/* crew freeing incremental space and crew */
		} else {
		    VDBG("build: zero out subsys\n");
		    getcrdat.cr_flsp += (getsc->sc_fsp * getsdat.s_pct) / 100L;
		    getcrdat.cr_crew += (getsc->sc_fcr * getsdat.s_pct) / 100L;
		    getsdat.s_pct = 0;
		    putsys(plogin);
		}
		putcrft(plogin);
	    }

	    plogin->ln_substat = (char *)((int)plogin->ln_substat + 1);

	/************************************************************/
	/* create/update to new percentage if enough space and crew */
	/************************************************************/
	} else {
	    long pct = atol(plogin->ln_input);
	    long maxspcpct,maxcrwpct,maxpct;
	    long fspc = getcrdat.cr_flsp;	/* current free space */
	    long fcrw = getcrdat.cr_crew;	/* current free crew */

	    VDBG("build: %s%% for subsys %d\n",plogin->ln_input,
	    (int)plogin->ln_substat);
	    if (!strcmp(plogin->ln_input,".")) {
		plogin->ln_crft[0] = 0;
		plogin->ln_stat = 0;
		plogin->ln_substat = NULL;
		output(plogin,'C',0,PROMPT);
		output(plogin,0,0,0);
		VDBG("build return\n");
		return;
	    }

	    /***************************************/
	    /* compute maximum percentage possible */
	    /***************************************/
	    if (getsys(plogin,(int)plogin->ln_substat)) {
		VDBG("build: subsys exists with %d%%\n",getsdat.s_pct);
		getsc = &config[getskey.s_type][getcrdat.cr_htyp];
		fspc += getsc->sc_bsp + (getsc->sc_fsp * getsdat.s_pct) / 100L;
		fcrw += getsc->sc_bcr + (getsc->sc_fcr * getsdat.s_pct) / 100L;
	    } else {
		VDBG("build: subsys doesn't exist\n");
		binit((char *)&getsdat,sizeof(getsdat));
	    }
	    maxspcpct = ((fspc - getsc->sc_bsp) * 100L) / getsc->sc_fsp;
	    maxcrwpct = ((fcrw - getsc->sc_bcr) * 100L) / getsc->sc_fcr;
	    maxpct = (maxspcpct < maxcrwpct) ? maxspcpct : maxcrwpct;
	    if (maxpct < 0L)
		maxpct = 0L;
	    else if (maxpct > 100L)
		maxpct = 100L;
	    
	    /* check request against limits */
	    if (pct < 0L || pct > maxpct) {
		VDBG("build: %ld%% not between %ld and %ld\n",pct,0,maxpct);
		sprintf(buf,
		"\nPercent of subsystem '%s' must be between 0 and %ld?",
		subsysnam[(int)plogin->ln_substat],maxpct);
		output(plogin,'C',0,buf);
		output(plogin,0,0,0);
		VDBG("build return\n");
		return;

	    /* create/update subsystem; update craft free space and crew */
	    } else {
		VDBG("build: subsys from %d%% to %ld%%\n",getsdat.s_pct,pct);
		getsdat.s_pct = pct;
		fixdmg();
		putsys(plogin);

		getsc = &config[getskey.s_type][getcrdat.cr_htyp];
		getcrdat.cr_flsp = fspc - (getsc->sc_bsp +
		(getsc->sc_fsp * pct) / 100L);
		getcrdat.cr_crew = fcrw - (getsc->sc_bcr +
		(getsc->sc_fcr * pct) / 100L);
		putcrft(plogin);

		plogin->ln_substat = (char *)((int)plogin->ln_substat + 1);
	    }
	}

	/*****************************/
	/* prompt for next subsystem */
	/*****************************/
	if ((int)plogin->ln_substat == HULL) {
	    output(plogin,'C',0,
	    "Hull type (1-rocket, 2-sphere, 3-Enterprise)>");
	    output(plogin,0,0,0);
	} else if ((int)plogin->ln_substat < MAXSYS) {
	    getsc = &config[(int)plogin->ln_substat][getcrdat.cr_htyp];
	    sprintf(buf,"\n\nSubsystem '%s'\n",subsysnam[(int)plogin->ln_substat]);
	    output(plogin,'C',0,buf);
	    sprintf(buf,"Your ship has free space of %ld and free crew of %ld\n",
	    getcrdat.cr_flsp,getcrdat.cr_crew);
	    output(plogin,'C',0,buf);
	    if (getsys(plogin,(int)plogin->ln_substat)) {
		sprintf(buf,"Your ship currently has %d%% of this subsystem\n",
		getsdat.s_pct);
		output(plogin,'C',0,buf);
	    }
	    sprintf(buf,
	    "For this subsystem, the base space is %ld and the base crew is %ld\n",
	    getsc->sc_bsp,getsc->sc_bcr);
	    output(plogin,'C',0,buf);
	    sprintf(buf,
	    "100%% installation uses an additional %ld space and %ld crew\n",
	    getsc->sc_fsp,getsc->sc_fcr);
	    output(plogin,'C',0,buf);
	    output(plogin,'C',0,"What percent do you want?");
	    output(plogin,0,0,0);

	/* all done */
	} else {
	    plogin->ln_crft[0] = 0;
	    plogin->ln_stat = 0;
	    plogin->ln_substat = NULL;
	    output(plogin,'C',0,PROMPT);
	    output(plogin,0,0,0);
	}
	VDBG("build return\n");
}

static int getcrft(plogin)
struct login *plogin;
{
	binit((char *)&getcrkey,sizeof(getcrkey));
	getcrkey.cr_crftkey = CRAFT;
	strcpy(getcrkey.cr_plyr,plogin->ln_name);
	strcpy(getcrkey.cr_name,plogin->ln_crft);
	dbmkey.dptr = (char *)&getcrkey;
	dbmkey.dsize = sizeof(getcrkey);
	dbmdata = fetch(dbmkey);
	VDBG("getcrft - %s %s\n",(dbmdata.dptr) ? "found" : "couldn't find",
	getcrkey.cr_name);
	if (!dbmdata.dptr)
		return(0);
	bytecopy((char *)&getcrdat,dbmdata.dptr,CRDATSIZ);
	return(1);
}

/* assumes getcrkey and getcrdat are current */
static void putcrft(plogin)
struct login *plogin;
{
	VDBG("putcrft - %s\n",getcrkey.cr_name);
	dbmkey.dptr = (char *)&getcrkey;
	dbmkey.dsize = sizeof(getcrkey);
	dbmdata.dptr = (char *)&getcrdat;
	dbmdata.dsize = CRDATSIZ;
	if (store(dbmkey,dbmdata)) {
		perror("putcrft: can't update craft");
		plogin->ln_crft[0] = 0;
		plogin->ln_stat = 0;
		plogin->ln_substat = NULL;
		output(plogin,'C',0,PROMPT);
		output(plogin,0,0,0);
	}
}

static int getsys(plogin,styp)
struct login *plogin;
int styp;
{
	binit((char *)&getskey,sizeof(getskey));
	getskey.s_syskey = SUBSYS;
	strcpy(getskey.s_plyr,plogin->ln_name);
	strcpy(getskey.s_crft,plogin->ln_crft);
	getskey.s_type = styp;
	dbmkey.dptr = (char *)&getskey;
	dbmkey.dsize = sizeof(getskey);
	dbmdata = fetch(dbmkey);
	VDBG("getsys - %s %d\n",(dbmdata.dptr) ? "found" : "couldn't find",styp);
	if (!dbmdata.dptr)
		return(0);
	bytecopy((char *)&getsdat,dbmdata.dptr,sizeof(getsdat));
	return(1);
}

/* assumes getskey and getsdat are current */
static void putsys(plogin)
struct login *plogin;
{
	VDBG("putsys - %d\n",getskey.s_type);
	dbmkey.dptr = (char *)&getskey;
	dbmkey.dsize = sizeof(getskey);
	dbmdata.dptr = (char *)&getsdat;
	dbmdata.dsize = sizeof(getsdat);
	if (store(dbmkey,dbmdata)) {
		perror("putsys: can't update subsystem");
		plogin->ln_crft[0] = 0;
		plogin->ln_stat = 0;
		plogin->ln_substat = NULL;
		output(plogin,'C',0,PROMPT);
		output(plogin,0,0,0);
	}
}

/* assumes getskey is current */
static void delsys(plogin)
struct login *plogin;
{
	VDBG("delsys - %d\n",getskey.s_type);
	dbmkey.dptr = (char *)&getskey;
	dbmkey.dsize = sizeof(getskey);
	if (delete(dbmkey)) {
		perror("delsys: can't delete subsystem");
		plogin->ln_crft[0] = 0;
		plogin->ln_stat = 0;
		plogin->ln_substat = NULL;
		output(plogin,'C',0,PROMPT);
		output(plogin,0,0,0);
	}
}

/* assumes getcrdat, getskey, and getsdat are current */
static void fixdmg()
{
	/* if priviledged or docked at a starbase, fix all damage */
	if (getcrdat.cr_plvl ||
	(getcrdat.cr_dock.ip_ofst > 0 &&
	univlst[getcrdat.cr_dock.ip_ofst].uv_pctr == '#'))
		getsdat.s_dmg = 0;

	/* fix temporary damage to 0, permanent damage to 45 */
	getsdat.s_dmg = (getsdat.s_dmg > 45) ? 45 : 0;

	/* update passive parameters and level */
	getsc = &config[getskey.s_type][getcrdat.cr_htyp];
	getsdat.s_edmg = getsc->sc_edmg;
	getsdat.s_lvl =  getsc->sc_ilvl;
	getsdat.s_cap =  getsc->sc_cap;
	if (getskey.s_type == ROCKETS)
		getsdat.s_lvl = ((long)getsdat.s_lvl * (long)getsdat.s_pct) / 100L;
	else if (getskey.s_type == TORPS)
		getsdat.s_lvl = (getsdat.s_cap * getsdat.s_pct) / 100;
}
