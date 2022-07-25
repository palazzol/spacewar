/*
 * Spacewar - update movement of objects
 *
 * Copyright 1985 obo Systems, Inc.
 * Copyright 1985 Dan Rosenblatt
 */

#include "spacewar.h"
#include "universe.h"
#include "obj.h"

VOID updobjs()
{
	register struct obj *pobj;
	double tmpvec[3];
	int i;

#ifdef DEBUG
	DBG("updobjs()\n");
#endif

	for (pobj=objlst;pobj < objlst+MAXOBJ;++pobj) {

	    /* new orbital center relative plane theta */
	    if (pobj->oj_oprd) {
		pobj->oj_ocrpt = ADD(pobj->oj_ocrpt,
		DIV(TWOPI,FLOAT(pobj->oj_oprd)));
		pobj->oj_ocrpt = FMOD(pobj->oj_ocrpt,TWOPI);
	    }

	    /* new position by converting from spherical to rectangular, */
	    /* rotating from orbital plane, and translating from orbital */
	    /* center; figure velocity from old and new position	 */
	    vcopy(tmpvec,pobj->oj_pstn);
	    pobj->oj_pstn[0] = FLOAT(pobj->oj_orad);
	    pobj->oj_pstn[1] = pobj->oj_ocrpt;
	    pobj->oj_pstn[2] = DIV(PI,2.);
	    sptort(pobj->oj_pstn,pobj->oj_pstn);
	    vecmul(pobj->oj_pstn,pobj->oj_rmat,pobj->oj_pstn);
	    for (i=0;i < 3;++i)
		pobj->oj_pstn[i] = ADD(pobj->oj_pstn[i],
		pobj->oj_octr.ip_ptr->uv_pstn[i]);
	    vchngd(pobj->oj_univ.ip_ptr);
	    vdiff(pobj->oj_pstn,tmpvec,pobj->oj_vel);
	}

#ifdef DEBUG
	VDBG("updobjs return\n");
#endif
}
