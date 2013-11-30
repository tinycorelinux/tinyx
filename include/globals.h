
#ifndef _XSERV_GLOBAL_H_
#define _XSERV_GLOBAL_H_

#include "window.h"	/* for WindowPtr */

/* Global X server variables that are visible to mi, dix, os, and ddx */

extern CARD32 defaultScreenSaverTime;
extern CARD32 defaultScreenSaverInterval;
extern CARD32 ScreenSaverTime;
extern CARD32 ScreenSaverInterval;

#ifdef SCREENSAVER
extern Bool screenSaverSuspended;
#endif

extern char *defaultFontPath;
extern int monitorResolution;
extern int defaultColorVisualClass;

extern Bool Must_have_memory;
extern WindowPtr WindowTable[MAXSCREENS];
extern int GrabInProgress;
extern Bool noTestExtensions;

extern DDXPointRec dixScreenOrigins[MAXSCREENS];

#ifdef DPMSExtension
extern CARD32 DPMSStandbyTime;
extern CARD32 DPMSSuspendTime;
extern CARD32 DPMSOffTime;
extern CARD16 DPMSPowerLevel;
extern Bool DPMSEnabled;
extern Bool DPMSEnabledSwitch;
extern Bool DPMSDisabledSwitch;
extern Bool DPMSCapableFlag;
#endif

extern Bool noBigReqExtension;

extern Bool noDamageExtension;

#ifdef DBE
extern Bool noDbeExtension;
#endif

#ifdef DPMSExtension
extern Bool noDPMSExtension;
#endif


#ifdef SCREENSAVER
extern Bool noScreenSaverExtension;
#endif

extern Bool noMITShmExtension;


extern Bool noRRExtension;

extern Bool noRenderExtension;

extern Bool noShapeExtension;

extern Bool noSyncExtension;

#ifdef RES
extern Bool noResExtension;
#endif

#ifdef XF86BIGFONT
extern Bool noXFree86BigfontExtension;
#endif




extern Bool noXFixesExtension;


#endif /* !_XSERV_GLOBAL_H_ */
