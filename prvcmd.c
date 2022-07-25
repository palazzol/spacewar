/*
 * Spacewar -  priviledged commands for SWMASTER
 *
 * Copyright 1984 obo Systems, Inc.
 * Copyright 1984 Dan Rosenblatt
 */

#include "spacewar.h"
#ifndef VMS
#include <sys/types.h>
#else /* BSD SYSIII SYSV */
#include <types.h>
#endif /* VMS */
#include "universe.h"
#include "login.h"
#include "sys.h"
#include "aln.h"
#include "build.h"
#include "obj.h"
#include "crft.h"
#include "torp.h"

static char *fmtip();

VOID prvcmd(plogin)
struct login *plogin;
{
	char *s=plogin->ln_input,buf[256];
	struct login *plgn;
	struct aln *paln;
	struct sys *psys;
	struct obj *pobj;
	struct universe *puniv;
	struct crft *pcrft;
	struct torp *ptorp;
	int i;

#ifdef DEBUG
	DBG("prvcmd(#%d/%s)\n",plogin-loginlst,plogin->ln_name);
#endif

	/* only allowed if priviledged */
	if (strcmp(SWMASTER,plogin->ln_name)) {
#ifdef DEBUG
		VDBG("prvcmd return\n");
#endif
		return;
	}

	if (!strcmp(".shutdown",s)) {
		shutdown(0);

	} else if (!strncmp(".logoff ",s,8)) {
	    for (s+=8,plgn=loginlst;plgn < loginlst+MAXLOGIN;++plgn) {
		if (!plgn->ln_tty) continue;
		if (!strcmp(plgn->ln_name,s)) logoff(plgn);
	    }

	} else if (!strncmp(".aln",s,4)) {
	    s += 4;
	    for (paln=alnlst;paln < alnlst+MAXALN;++paln) {
		sprintf(buf,
		"#%-3d (%8.1e %8.1e %8.1e) (%3.0f %3.0f %3.0f) (%2.0f %2.0f %2.0f)",
		paln-alnlst,paln->al_pstn[0],paln->al_pstn[1],paln->al_pstn[2],
		paln->al_vel[0],paln->al_vel[1],paln->al_vel[2],
		paln->al_thr[0],paln->al_thr[1],paln->al_thr[2]);
		output(plogin,'C',0,buf);
		sprintf(buf," %d %d %s",paln->al_dly,paln->al_htyp,
		fmtip(paln->al_lhit));
		output(plogin,'C',0,buf);
		sprintf(buf," %s %d\n",fmtip(paln->al_atck),paln->al_aeval);
		output(plogin,'C',0,buf);
		if (*s)
		    for (psys=paln->al_sys;psys < paln->al_sys+MSYS;++psys) {
			if (!psys->s_cap) continue; /* not in this craft */
			sprintf(buf,"\t%-8.8s %3d %3d %3d %5d %5d\n",
			subsysnam[psys-paln->al_sys],psys->s_pct,psys->s_edmg,
			psys->s_dmg,psys->s_lvl,psys->s_cap);
			output(plogin,'C',0,buf);
		    }
	    }

	} else if (!strncmp(".crft",s,5)) {
	    s += 5;
	    for (pcrft=crftlst;pcrft < crftlst+MAXCRFT;++pcrft) {
		if (!pcrft->cr_htyp) continue;
		sprintf(buf,"#%-3d %d %5ld %3ld %5ld %5ld %2d %s\n",
		pcrft-crftlst,pcrft->cr_htyp,pcrft->cr_flsp,pcrft->cr_crew,
		pcrft->cr_pnts,pcrft->cr_time,pcrft->cr_kill,
		fmtip(pcrft->cr_dock));
		output(plogin,'C',0,buf);
		sprintf(buf,"     (%8.1e %8.1e %8.1e) (%8.1e %8.1e %8.1e)\
(%3.0f %3.0f %3.0f) (%2.0f %2.0f %2.0f)\n     ",
		pcrft->cr_pstn[0],pcrft->cr_pstn[1],pcrft->cr_pstn[2],
		pcrft->cr_dir[0],pcrft->cr_dir[1],pcrft->cr_dir[2],
		pcrft->cr_vel[0],pcrft->cr_vel[1],pcrft->cr_vel[2],
		pcrft->cr_thr[0],pcrft->cr_thr[1],pcrft->cr_thr[2]);
		output(plogin,'C',0,buf);
		for (i=0;i < MHOM;++i) {
		    sprintf(buf,"%d:%s ",i,fmtip(pcrft->cr_hom[i]));
		    output(plogin,'C',0,buf);
		}
		sprintf(buf,"  %s %d %5.1f %ld %d\n",fmtip(pcrft->cr_auto),
		pcrft->cr_ffwd,DIV(pcrft->cr_vang,DEGTORAD),pcrft->cr_vdst,
		pcrft->cr_plvl);
		output(plogin,'C',0,buf);
		if (*s)
		    for (psys=pcrft->cr_sys;psys < pcrft->cr_sys+MSYS;++psys) {
			if (!psys->s_cap) continue; /* not in this craft */
			sprintf(buf,"\t%-8.8s %3d %3d %3d %5d %5d\n",
			subsysnam[psys-pcrft->cr_sys],psys->s_pct,psys->s_edmg,
			psys->s_dmg,psys->s_lvl,psys->s_cap);
			output(plogin,'C',0,buf);
		    }
	    }
	} else if (!strncmp(".login",s,6)) {
	    s += 6;
	    for (plgn=loginlst;plgn < loginlst+MAXLOGIN;++plgn) {
		if (!plgn->ln_tty) continue;
		if (*s) {
#ifdef VMS
		    sprintf(buf,
		    "#%-3d %2d %x '%-8.8s' '%-8.8s' '%c' '%-12.12s' %s",
		    plgn-loginlst,plgn->ln_tty,plgn->ln_pid,plgn->ln_name,
		    plgn->ln_term ? plgn->ln_term : "",
		    plgn->ln_iomode ? plgn->ln_iomode : ' ',
		    plgn->ln_crft,fmtip(plgn->ln_play));
#else /* BSD SYSIII SYSV */
		    sprintf(buf,
		    "#%-3d %2d %5d %5d '%-8.8s' '%-8.8s' '%c' '%-12.12s' %s",
		    plgn-loginlst,plgn->ln_tty,plgn->ln_playpid,
		    plgn->ln_readpid,plgn->ln_name,
		    plgn->ln_term ? plgn->ln_term : "",
		    plgn->ln_iomode ? plgn->ln_iomode : ' ',plgn->ln_crft,
		    fmtip(plgn->ln_play));
#endif /* VMS BSD SYSIII SYSV */
		    output(plogin,'C',0,buf);
		    sprintf(buf," '%c'\n     '%s'\n",
		    plgn->ln_stat ? plgn->ln_stat : ' ',plgn->ln_input);
		    output(plogin,'C',0,buf);
		} else {
		    sprintf(buf,"#%-3d '%-8.8s' '%-8.8s' '%c' '%-12.12s' %s",
		    plgn-loginlst,plgn->ln_name,
		    plgn->ln_term ? plgn->ln_term : "",
		    plgn->ln_iomode ? plgn->ln_iomode : ' ',plgn->ln_crft,
		    fmtip(plgn->ln_play));
		    output(plogin,'C',0,buf);
		    sprintf(buf," '%c'\n",plgn->ln_stat ? plgn->ln_stat : ' ');
		    output(plogin,'C',0,buf);
		}
	    }

	} else if (!strncmp(".obj",s,4)) {
	    s += 4;
	    for (pobj=objlst;pobj < objlst+MAXOBJ;++pobj) {
		sprintf(buf,"#%-3d %7ld %3d '%c' %s %5d %7ld %3.0f",
		pobj-objlst,pobj->oj_mass,pobj->oj_rad,pobj->oj_rep,
		fmtip(pobj->oj_octr),pobj->oj_oprd,pobj->oj_orad,
		DIV(pobj->oj_ocrpt,DEGTORAD));
		output(plogin,'C',0,buf);
		if (*s) {
		    sprintf(buf," %5.1f %5.1f %5.1f %3.0f %3.0f %3.0f",
		    pobj->oj_optx,pobj->oj_opty,pobj->oj_optz,
		    pobj->oj_vel[0],pobj->oj_vel[1],pobj->oj_vel[2]);
		    output(plogin,'C',0,buf);
		}
		output(plogin,'C',0,"\n");
	    }

	} else if (!strcmp(".torp",s)) {
	    for (ptorp=torplst;ptorp < torplst+MAXTORP;++ptorp) {
		if (!ptorp->tp_aim.ip_ptr) continue;
		sprintf(buf,"#%-3d (%8.1e %8.1e %8.1e) (%3.0f %3.0f %3.0f) \
(%2.0f %2.0f %2.0f) %s",
		ptorp-torplst,ptorp->tp_pstn[0],ptorp->tp_pstn[1],
		ptorp->tp_pstn[2],ptorp->tp_vel[0],ptorp->tp_vel[1],
		ptorp->tp_vel[2],ptorp->tp_thr[0],ptorp->tp_thr[1],
		ptorp->tp_thr[2],fmtip(ptorp->tp_aim));
		output(plogin,'C',0,buf);
		sprintf(buf," %s %ld %d",fmtip(ptorp->tp_fby),ptorp->tp_dist,
		ptorp->tp_dmg);
		output(plogin,'C',0,buf);
		sprintf(buf," %s\n",fmtip(ptorp->tp_lhit));
		output(plogin,'C',0,buf);
	    }

	} else if (!strcmp(".universe",s)) {
	    for (puniv=univlst;puniv < univlst+MAXUNIVERSE;++puniv) {
		if (!puniv->uv_type) continue;
		sprintf(buf,"#%-3d '%c/%c' %8.1e %8.1e %8.1e %7ld %5d",
		puniv-univlst,puniv->uv_type,puniv->uv_pctr,
		puniv->uv_pstn[0],puniv->uv_pstn[1],puniv->uv_pstn[2],
		puniv->uv_mass,puniv->uv_rad);
		output(plogin,'C',0,buf);
		switch(puniv->uv_type) {
		    case 'A':
			sprintf(buf," #%-3d\n",puniv->uv_ptr.uv_aln-alnlst);
			break;
		    case 'P':
			sprintf(buf," #%-3d\n",puniv->uv_ptr.uv_crft-crftlst);
			break;
		    case 'O':
			sprintf(buf," #%-3d\n",puniv->uv_ptr.uv_obj-objlst);
			break;
		    case 'T':
			sprintf(buf," #%-3d\n",puniv->uv_ptr.uv_torp-torplst);
			break;
		    default:
			strcpy(buf,"\n");
			break;
		}
		output(plogin,'C',0,buf);
	    }

	} else if (!strncmp(".lock",s,5)) {
	    static msg[80+1];
	    s += 5;
	    while (*s == ' ') ++s;
	    strcpy(msg,s);
	    prvlck((*s) ? msg : NULL);

	} else if (!strcmp(".unlock",s)) {
	    prvlck("");
	}

	output(plogin,0,0,0);
#ifdef DEBUG
	VDBG("prvcmd return\n");
#endif
}

static char *fmtip(ip)
idxptr ip;
{
	static char buf[16];
	if (ip.ip_ptr)
	    sprintf(buf,"#%-3d",ip.ip_ptr-univlst);
	else
	    strcpy(buf,"NULL");
	return(buf);
}
