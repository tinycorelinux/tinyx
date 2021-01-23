/*
 *
 * Copyright © 1999 Keith Packard
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Keith Packard not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Keith Packard makes no
 * representations about the suitability of this software for any purpose.  It
 * is provided "as is" without express or implied warranty.
 *
 * KEITH PACKARD DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL KEITH PACKARD BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _KDRIVE_H_
#define _KDRIVE_H_

#include <stdio.h>
#include <string.h>
#include <X11/X.h>
#include <X11/Xproto.h>
#include <X11/Xos.h>
#include "scrnintstr.h"
#include "pixmapstr.h"
#include "windowstr.h"
#include "servermd.h"
#include "colormapst.h"
#include "gcstruct.h"
#include "input.h"
#include "mipointer.h"
#include "mi.h"
#include "dix.h"
#include "fb.h"
#include "fboverlay.h"
#include "shadow.h"
#include "randrstr.h"
#include "globals.h"

#define KD_DPMS_NORMAL	    0
#define KD_DPMS_STANDBY	    1
#define KD_DPMS_SUSPEND	    2
#define KD_DPMS_POWERDOWN   3
#define KD_DPMS_MAX	    KD_DPMS_POWERDOWN

#ifndef KD_MAX_CARD_ADDRESS
#define KD_MAX_CARD_ADDRESS 8
#endif

typedef struct _KdCardInfo {
	const struct _KdCardFuncs *cfuncs;
	void *closure;
	void *driver;
	struct _KdScreenInfo *screenList;
	int selected;
	struct _KdCardInfo *next;

	Bool needSync;
	int lastMarker;
} KdCardInfo;

extern KdCardInfo *kdCardInfo;

/*
 * Configuration information per X screen
 */
typedef struct _KdFrameBuffer {
	CARD8 *frameBuffer;
	int depth;
	int bitsPerPixel;
	int pixelStride;
	int byteStride;
	Bool shadow;
	unsigned long visuals;
	Pixel redMask, greenMask, blueMask;
	void *closure;
} KdFrameBuffer;

#define RR_Rotate_All	(RR_Rotate_0|RR_Rotate_90|RR_Rotate_180|RR_Rotate_270)
#define RR_Reflect_All	(RR_Reflect_X|RR_Reflect_Y)

typedef struct _KdScreenInfo {
	struct _KdScreenInfo *next;
	KdCardInfo *card;
	ScreenPtr pScreen;
	void *driver;
	Rotation randr;		/* rotation and reflection */
	int width;
	int height;
	int rate;
	int width_mm;
	int height_mm;
	int subpixel_order;
	Bool dumb;
	Bool softCursor;
	int mynum;
	DDXPointRec origin;
	KdFrameBuffer fb;
	CARD8 *memory_base;
	unsigned long memory_size;
	unsigned long off_screen_base;
} KdScreenInfo;

typedef struct _KdCardFuncs {
	Bool(*cardinit) (KdCardInfo *);	/* detect and map device */
	Bool(*scrinit) (KdScreenInfo *);	/* initialize screen information */
	Bool(*initScreen) (ScreenPtr);	/* initialize ScreenRec */
	Bool(*finishInitScreen) (ScreenPtr pScreen);
	Bool(*createRes) (ScreenPtr);	/* create screen resources */
	void (*preserve) (KdCardInfo *);	/* save graphics card state */
	 Bool(*enable) (ScreenPtr);	/* set up for rendering */
	 Bool(*dpms) (ScreenPtr, int);	/* set DPMS screen saver */
	void (*disable) (ScreenPtr);	/* turn off rendering */
	void (*restore) (KdCardInfo *);	/* restore graphics card state */
	void (*scrfini) (KdScreenInfo *);	/* close down screen */
	void (*cardfini) (KdCardInfo *);	/* close down */

	 Bool(*initCursor) (ScreenPtr);	/* detect and map cursor */
	void (*enableCursor) (ScreenPtr);	/* enable cursor */
	void (*disableCursor) (ScreenPtr);	/* disable cursor */
	void (*finiCursor) (ScreenPtr);	/* close down */
	void (*recolorCursor) (ScreenPtr, int, xColorItem *);

	 Bool(*initAccel) (ScreenPtr);
	void (*enableAccel) (ScreenPtr);
	void (*disableAccel) (ScreenPtr);
	void (*finiAccel) (ScreenPtr);

	void (*getColors) (ScreenPtr, int, xColorItem *);
	void (*putColors) (ScreenPtr, int, xColorItem *);

} KdCardFuncs;

#define KD_MAX_PSEUDO_DEPTH 8
#define KD_MAX_PSEUDO_SIZE	    (1 << KD_MAX_PSEUDO_DEPTH)

typedef struct {
	KdScreenInfo *screen;
	KdCardInfo *card;

	Bool enabled;
	Bool closed;
	int bytesPerPixel;

	int dpmsState;

	ColormapPtr pInstalledmap;	/* current colormap */
	xColorItem systemPalette[KD_MAX_PSEUDO_SIZE];	/* saved windows colors */

	CreateScreenResourcesProcPtr CreateScreenResources;
	CloseScreenProcPtr CloseScreen;
} KdPrivScreenRec, *KdPrivScreenPtr;

typedef enum _kdMouseState {
	start,
	button_1_pend,
	button_1_down,
	button_2_down,
	button_3_pend,
	button_3_down,
	synth_2_down_13,
	synth_2_down_3,
	synth_2_down_1,
	num_input_states
} KdMouseState;

#define KD_MAX_BUTTON  7

typedef struct _KdMouseInfo {
	struct _KdMouseInfo *next;
	void *driver;
	void *closure;
	char *name;
	char *prot;
	char map[KD_MAX_BUTTON];
	int nbutton;
	Bool emulateMiddleButton;
	unsigned long emulationTimeout;
	Bool timeoutPending;
	KdMouseState mouseState;
	Bool eventHeld;
	xEvent heldEvent;
	unsigned char buttonState;
	int emulationDx, emulationDy;
	int inputType;
	Bool transformCoordinates;
} KdMouseInfo;

extern KdMouseInfo *kdMouseInfo;

KdMouseInfo *KdMouseInfoAdd(void);

void KdParseMouse(const char *);

typedef struct _KdMouseFuncs {
	Bool(*Init) (void);
	void (*Fini) (void);
} KdMouseFuncs;

typedef struct _KdKeyboardFuncs {
	void (*Load) (void);
	int (*Init) (void);
	void (*Leds) (int);
	void (*Bell) (int, int, int);
	void (*Fini) (void);
	int LockLed;
} KdKeyboardFuncs;

typedef struct _KdOsFuncs {
	int (*Init) (void);
	void (*Enable) (void);
	 Bool(*SpecialKey) (KeySym);
	void (*Disable) (void);
	void (*Fini) (void);
	void (*pollEvents) (void);
} KdOsFuncs;

typedef enum _KdSyncPolarity {
	KdSyncNegative, KdSyncPositive
} KdSyncPolarity;

typedef struct _KdMonitorTiming {
	/* label */
	int horizontal;
	int vertical;
	int rate;
	/* pixel clock */
	int clock;		/* in KHz */
	/* horizontal timing */
	int hfp;		/* front porch */
	int hbp;		/* back porch */
	int hblank;		/* blanking */
	KdSyncPolarity hpol;	/* polarity */
	/* vertical timing */
	int vfp;		/* front porch */
	int vbp;		/* back porch */
	int vblank;		/* blanking */
	KdSyncPolarity vpol;	/* polarity */
} KdMonitorTiming;

typedef struct _KdMouseMatrix {
	int matrix[2][3];
} KdMouseMatrix;

/*
 * This is the only completely portable way to
 * compute this info.
 */

#ifndef BitsPerPixel
#define BitsPerPixel(d) (\
    PixmapWidthPaddingInfo[d].notPower2 ? \
    (PixmapWidthPaddingInfo[d].bytesPerPixel * 8) : \
    ((1 << PixmapWidthPaddingInfo[d].padBytesLog2) * 8 / \
    (PixmapWidthPaddingInfo[d].padRoundUp+1)))
#endif

extern int kdScreenPrivateIndex;
extern Bool kdSwitchPending;
extern Bool kdDisableZaphod;
extern Bool kdDontZap;
extern int kdVirtualTerminal;
extern const KdOsFuncs *kdOsFuncs;

#define KdGetScreenPriv(pScreen) ((KdPrivScreenPtr) \
				  (pScreen)->devPrivates[kdScreenPrivateIndex].ptr)
#define KdSetScreenPriv(pScreen,v) ((pScreen)->devPrivates[kdScreenPrivateIndex].ptr = \
				    (pointer) v)
#define KdScreenPriv(pScreen) KdPrivScreenPtr pScreenPriv = KdGetScreenPriv(pScreen)

/* kcmap.c */
void KdEnableColormap(ScreenPtr pScreen);

void KdDisableColormap(ScreenPtr pScreen);

void KdInstallColormap(ColormapPtr pCmap);

void KdUninstallColormap(ColormapPtr pCmap);

int KdListInstalledColormaps(ScreenPtr pScreen, Colormap * pCmaps);

void KdStoreColors(ColormapPtr pCmap, int ndef, xColorItem * pdefs);

/* kdrive.c */
extern miPointerScreenFuncRec kdPointerScreenFuncs;

void KdDisableScreen(ScreenPtr pScreen);

Bool KdEnableScreen(ScreenPtr pScreen);

void KdSuspend(void);

void KdResume(void);

void KdProcessSwitch(void);

Rotation KdAddRotation(Rotation a, Rotation b);

Rotation KdSubRotation(Rotation a, Rotation b);

void KdUseMsg(void);

int KdProcessArgument(int argc, char **argv, int i);

void KdOsInit(const KdOsFuncs * const pOsFuncs);

void
KdInitCard(ScreenInfo * pScreenInfo, KdCardInfo * card, int argc, char **argv);

void KdInitOutput(ScreenInfo * pScreenInfo, int argc, char **argv);

void KdSetSubpixelOrder(ScreenPtr pScreen, Rotation randr);

void kdVersion(const char name[]);

/* kinfo.c */
KdCardInfo *KdCardInfoAdd(const KdCardFuncs * const funcs, void *closure);

KdCardInfo *KdCardInfoLast(void);

KdScreenInfo *KdScreenInfoAdd(KdCardInfo * ci);

/* kinput.c */
void KdInitInput(const KdMouseFuncs *, const KdKeyboardFuncs *);

int KdAllocInputType(void);

Bool
KdRegisterFd(int type, int fd, void (*read) (int fd, void *closure),
	     void *closure);

void
KdRegisterFdEnableDisable(int fd,
			  int (*enable) (int fd, void *closure),
			  void (*disable) (int fd, void *closure));

void KdUnregisterFds(int type, Bool do_close);

void KdEnqueueKeyboardEvent(unsigned char scan_code, unsigned char is_up);

#define KD_BUTTON_1	0x01
#define KD_BUTTON_2	0x02
#define KD_BUTTON_3	0x04
#define KD_BUTTON_4	0x08
#define KD_BUTTON_5	0x10
#define KD_MOUSE_DELTA	0x80000000

void KdEnqueueMouseEvent(KdMouseInfo * mi, unsigned long flags, int x, int y);

void KdEnqueueMotionEvent(KdMouseInfo * mi, int x, int y);

void KdReleaseAllKeys(void);

void KdSetLed(int led, Bool on);

void KdSetMouseMatrix(KdMouseMatrix * matrix);

void
KdComputeMouseMatrix(KdMouseMatrix * matrix, Rotation randr, int width,
		     int height);

void
KdBlockHandler(int screen,
	       pointer blockData, pointer timeout, pointer readmask);

void
KdWakeupHandler(int screen,
		pointer data, unsigned long result, pointer readmask);

void KdDisableInput(void);

void KdEnableInput(void);

void ProcessInputEvents(void);

extern const KdMouseFuncs LinuxMouseFuncs;
extern const KdKeyboardFuncs LinuxKeyboardFuncs;

/* kmap.c */

#define KD_MAPPED_MODE_REGISTERS    0
#define KD_MAPPED_MODE_FRAMEBUFFER  1

void *KdMapDevice(CARD32 addr, CARD32 size);

void KdUnmapDevice(void *addr, CARD32 size);

void KdSetMappedMode(CARD32 addr, CARD32 size, int mode);

void KdResetMappedMode(CARD32 addr, CARD32 size, int mode);

/* kmode.c */
const KdMonitorTiming *KdFindMode(KdScreenInfo * screen,
				  Bool(*supported) (KdScreenInfo *,
						    const KdMonitorTiming *));

/* kshadow.c */
Bool KdShadowFbAlloc(KdScreenInfo * screen, Bool rotate);

void KdShadowFbFree(KdScreenInfo * screen);

Bool
KdShadowSet(ScreenPtr pScreen, int randr, ShadowUpdateProc update,
	    ShadowWindowProc window);

void KdShadowUnset(ScreenPtr pScreen);

/* function prototypes to be implemented by the drivers */
void InitCard(char *name) XFONT_LTO;

#endif				/* _KDRIVE_H_ */
