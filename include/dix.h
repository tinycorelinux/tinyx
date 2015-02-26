/***********************************************************

Copyright 1987, 1998  The Open Group

Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of The Open Group shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from The Open Group.


Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of Digital not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

#ifndef DIX_H
#define DIX_H

#include "gc.h"
#include "window.h"
#include "input.h"

#define EARLIER -1
#define SAMETIME 0
#define LATER 1

#define NullClient ((ClientPtr) 0)
#define REQUEST(type) \
	register type *stuff = (type *)client->requestBuffer


#define REQUEST_SIZE_MATCH(req)\
    if ((sizeof(req) >> 2) != client->req_len)\
         return(BadLength)

#define REQUEST_AT_LEAST_SIZE(req) \
    if ((sizeof(req) >> 2) > client->req_len )\
         return(BadLength)

#define REQUEST_FIXED_SIZE(req, n)\
    if (((sizeof(req) >> 2) > client->req_len) || \
        (((sizeof(req) + (n) + 3) >> 2) != client->req_len)) \
         return(BadLength)

#define LEGAL_NEW_RESOURCE(id,client)\
    if (!LegalNewID(id,client)) \
    {\
	client->errorValue = id;\
        return(BadIDChoice);\
    }

/* XXX if you are using this macro, you are probably not generating Match
 * errors where appropriate */
#define LOOKUP_DRAWABLE(did, client)\
    ((client->lastDrawableID == did) ? \
     client->lastDrawable : (DrawablePtr)LookupDrawable(did, client))


#define VERIFY_DRAWABLE(pDraw, did, client)\
    if (client->lastDrawableID == did)\
	pDraw = client->lastDrawable;\
    else \
    {\
	pDraw = (DrawablePtr) LookupIDByClass(did, RC_DRAWABLE);\
	if (!pDraw) \
	{\
	    client->errorValue = did; \
	    return BadDrawable;\
	}\
	if (pDraw->type == UNDRAWABLE_WINDOW)\
	    return BadMatch;\
    }

#define VERIFY_GEOMETRABLE(pDraw, did, client)\
    if (client->lastDrawableID == did)\
	pDraw = client->lastDrawable;\
    else \
    {\
	pDraw = (DrawablePtr) LookupIDByClass(did, RC_DRAWABLE);\
	if (!pDraw) \
	{\
	    client->errorValue = did; \
	    return BadDrawable;\
	}\
    }

#define VERIFY_GC(pGC, rid, client)\
    if (client->lastGCID == rid)\
        pGC = client->lastGC;\
    else\
	pGC = (GC *)LookupIDByType(rid, RT_GC);\
    if (!pGC)\
    {\
	client->errorValue = rid;\
	return (BadGC);\
    }

#define SECURITY_VERIFY_DRAWABLE(pDraw, did, client, mode)\
	VERIFY_DRAWABLE(pDraw, did, client)

#define SECURITY_VERIFY_GEOMETRABLE(pDraw, did, client, mode)\
	VERIFY_GEOMETRABLE(pDraw, did, client)

#define SECURITY_VERIFY_GC(pGC, rid, client, mode)\
	VERIFY_GC(pGC, rid, client)


/*
 * We think that most hardware implementations of DBE will want
 * LookupID*(dbe_back_buffer_id) to return the window structure that the
 * id is a back buffer for.  Since both front and back buffers will
 * return the same structure, you need to be able to distinguish
 * somewhere what kind of buffer (front/back) was being asked for, so
 * that ddx can render to the right place.  That's the problem that the
 * following code solves.  Note: we couldn't embed this in the LookupID*
 * functions because the VALIDATE_DRAWABLE_AND_GC macro often circumvents
 * those functions by checking a one-element cache.  That's why we're
 * mucking with VALIDATE_DRAWABLE_AND_GC.
 *
 * If you put -DNEED_DBE_BUF_BITS into PervasiveDBEDefines, the window
 * structure will have two additional bits defined, srcBuffer and
 * dstBuffer, and their values will be maintained via the macros
 * SET_DBE_DSTBUF and SET_DBE_SRCBUF (below).  If you also
 * put -DNEED_DBE_BUF_VALIDATE into PervasiveDBEDefines, the function
 * DbeValidateBuffer will be called any time the bits change to give you
 * a chance to do some setup.  See the DBE code for more details on this
 * function.  We put in these levels of conditionality so that you can do
 * just what you need to do, and no more.  If neither of these defines
 * are used, the bits won't be there, and VALIDATE_DRAWABLE_AND_GC will
 * be unchanged.	dpw
 */

#if defined(NEED_DBE_BUF_BITS)
#define SET_DBE_DSTBUF(_pDraw, _drawID) \
        SET_DBE_BUF(_pDraw, _drawID, dstBuffer, TRUE)
#define SET_DBE_SRCBUF(_pDraw, _drawID) \
        SET_DBE_BUF(_pDraw, _drawID, srcBuffer, FALSE)
#if defined (NEED_DBE_BUF_VALIDATE)
#define SET_DBE_BUF(_pDraw, _drawID, _whichBuffer, _dstbuf) \
    if (_pDraw->type == DRAWABLE_WINDOW)\
    {\
	int thisbuf = (_pDraw->id == _drawID);\
	if (thisbuf != ((WindowPtr)_pDraw)->_whichBuffer)\
	{\
	     ((WindowPtr)_pDraw)->_whichBuffer = thisbuf;\
	     DbeValidateBuffer((WindowPtr)_pDraw, _drawID, _dstbuf);\
	}\
     }
#else /* want buffer bits, but don't need to call DbeValidateBuffer */
#define SET_DBE_BUF(_pDraw, _drawID, _whichBuffer, _dstbuf) \
    if (_pDraw->type == DRAWABLE_WINDOW)\
    {\
	((WindowPtr)_pDraw)->_whichBuffer = (_pDraw->id == _drawID);\
    }
#endif /* NEED_DBE_BUF_VALIDATE */
#else /* don't want buffer bits in window */
#define SET_DBE_DSTBUF(_pDraw, _drawID) /**/
#define SET_DBE_SRCBUF(_pDraw, _drawID) /**/
#endif /* NEED_DBE_BUF_BITS */

#define VALIDATE_DRAWABLE_AND_GC(drawID, pDraw, pGC, client)\
    if ((stuff->gc == INVALID) || (client->lastGCID != stuff->gc) ||\
	(client->lastDrawableID != drawID))\
    {\
	SECURITY_VERIFY_GEOMETRABLE(pDraw, drawID, client, SecurityWriteAccess);\
	SECURITY_VERIFY_GC(pGC, stuff->gc, client, SecurityReadAccess);\
	if ((pGC->depth != pDraw->depth) ||\
	    (pGC->pScreen != pDraw->pScreen))\
	    return (BadMatch);\
	client->lastDrawable = pDraw;\
	client->lastDrawableID = drawID;\
	client->lastGC = pGC;\
	client->lastGCID = stuff->gc;\
    }\
    else\
    {\
        pGC = client->lastGC;\
        pDraw = client->lastDrawable;\
    }\
    SET_DBE_DSTBUF(pDraw, drawID);\
    if (pGC->serialNumber != pDraw->serialNumber)\
	ValidateGC(pDraw, pGC);


#define WriteReplyToClient(pClient, size, pReply) { \
   if ((pClient)->swapped) \
      (*ReplySwapVector[((xReq *)(pClient)->requestBuffer)->reqType]) \
           (pClient, (int)(size), pReply); \
      else (void) WriteToClient(pClient, (int)(size), (char *)(pReply)); }

#define WriteSwappedDataToClient(pClient, size, pbuf) \
   if ((pClient)->swapped) \
      (*(pClient)->pSwapReplyFunc)(pClient, (int)(size), pbuf); \
   else (void) WriteToClient (pClient, (int)(size), (char *)(pbuf));

typedef struct _TimeStamp *TimeStampPtr;

#ifndef _XTYPEDEF_CLIENTPTR
typedef struct _Client *ClientPtr; /* also in misc.h */
#define _XTYPEDEF_CLIENTPTR
#endif

typedef struct _WorkQueue	*WorkQueuePtr;

extern ClientPtr requestingClient;
extern ClientPtr clients[MAXCLIENTS];
extern ClientPtr serverClient;
extern int currentMaxClients;
extern char dispatchExceptionAtReset;

typedef int HWEventQueueType;
typedef HWEventQueueType* HWEventQueuePtr;

extern HWEventQueuePtr checkForInput[2];

typedef struct _TimeStamp {
    CARD32 months;	/* really ~49.7 days */
    CARD32 milliseconds;
}           TimeStamp;

/* dispatch.c */

void SetInputCheck(
    HWEventQueuePtr /*c0*/,
    HWEventQueuePtr /*c1*/);

void CloseDownClient(
    ClientPtr /*client*/);

void UpdateCurrentTime(void);

void UpdateCurrentTimeIf(void);

void InitSelections(void);

void FlushClientCaches(XID /*id*/);

int dixDestroyPixmap(
    pointer /*value*/,
    XID /*pid*/);

void CloseDownRetainedResources(void);

void InitClient(
    ClientPtr /*client*/,
    int /*i*/,
    pointer /*ospriv*/);

ClientPtr NextAvailableClient(
    pointer /*ospriv*/);

void SendErrorToClient(
    ClientPtr /*client*/,
    unsigned int /*majorCode*/,
    unsigned int /*minorCode*/,
    XID /*resId*/,
    int /*errorCode*/);

void DeleteWindowFromAnySelections(
    WindowPtr /*pWin*/);

void MarkClientException(
    ClientPtr /*client*/);

int GetGeometry(
    ClientPtr /*client*/,
    xGetGeometryReply* /* wa */);

int SendConnSetup(
    ClientPtr /*client*/,
    const char* /*reason*/);

int DoGetImage(
    ClientPtr	/*client*/,
    int /*format*/,
    Drawable /*drawable*/,
    int /*x*/,
    int /*y*/,
    int /*width*/,
    int /*height*/,
    Mask /*planemask*/,
    xGetImageReply ** /*im_return*/);

/* dixutils.c */

int CompareISOLatin1Lowered(
    unsigned char * /*a*/,
    int alen,
    unsigned char * /*b*/,
    int blen);


WindowPtr LookupWindow(
    XID /*rid*/,
    ClientPtr /*client*/);

pointer LookupDrawable(
    XID /*rid*/,
    ClientPtr /*client*/);

#define SecurityLookupWindow(rid, client, access_mode) \
	LookupWindow(rid, client)

#define SecurityLookupDrawable(rid, client, access_mode) \
	LookupDrawable(rid, client)


ClientPtr LookupClient(
    XID /*rid*/,
    ClientPtr /*client*/);

void NoopDDA(void);

int AlterSaveSetForClient(
    ClientPtr /*client*/,
    WindowPtr /*pWin*/,
    unsigned /*mode*/,
    Bool /*toRoot*/,
    Bool /*remap*/);

void DeleteWindowFromAnySaveSet(
    WindowPtr /*pWin*/);

void BlockHandler(
    pointer /*pTimeout*/,
    pointer /*pReadmask*/);

void WakeupHandler(
    int /*result*/,
    pointer /*pReadmask*/);

typedef void (* WakeupHandlerProcPtr)(
    pointer /* blockData */,
    int /* result */,
    pointer /* pReadmask */);

Bool RegisterBlockAndWakeupHandlers(
    BlockHandlerProcPtr /*blockHandler*/,
    WakeupHandlerProcPtr /*wakeupHandler*/,
    pointer /*blockData*/);

void RemoveBlockAndWakeupHandlers(
    BlockHandlerProcPtr /*blockHandler*/,
    WakeupHandlerProcPtr /*wakeupHandler*/,
    pointer /*blockData*/);

void InitBlockAndWakeupHandlers(void);

void ProcessWorkQueue(void);

void ProcessWorkQueueZombies(void);

Bool QueueWorkProc(
    Bool (* /*function*/)(
        ClientPtr /*clientUnused*/,
        pointer /*closure*/),
    ClientPtr /*client*/,
    pointer /*closure*/
);

typedef Bool (* ClientSleepProcPtr)(
    ClientPtr /*client*/,
    pointer /*closure*/);

Bool ClientSleep(
    ClientPtr /*client*/,
    ClientSleepProcPtr /* function */,
    pointer /*closure*/);

#ifndef ___CLIENTSIGNAL_DEFINED___
#define ___CLIENTSIGNAL_DEFINED___
Bool XFONT_LTO ClientSignal(
    ClientPtr /*client*/);
#endif /* ___CLIENTSIGNAL_DEFINED___ */

void ClientWakeup(
    ClientPtr /*client*/);

Bool ClientIsAsleep(
    ClientPtr /*client*/);

/* atom.c */

XFONT_LTO Atom MakeAtom(
    char * /*string*/,
    unsigned /*len*/,
    Bool /*makeit*/);

XFONT_LTO Bool ValidAtom(
    Atom /*atom*/);

XFONT_LTO char *NameForAtom(
    Atom /*atom*/);

void AtomError(void);

void FreeAllAtoms(void);

void InitAtoms(void);

/* events.c */

void SetMaskForEvent(
    Mask /* mask */,
    int /* event */);


Bool IsParent(
    WindowPtr /* maybeparent */,
    WindowPtr /* child */);

WindowPtr GetCurrentRootWindow(void);

WindowPtr GetSpriteWindow(void);


void NoticeEventTime(xEventPtr /* xE */);

void EnqueueEvent(
    xEventPtr /* xE */,
    DeviceIntPtr /* device */,
    int	/* count */);

void ComputeFreezes(void);

void CheckGrabForSyncs(
    DeviceIntPtr /* dev */,
    Bool /* thisMode */,
    Bool /* otherMode */);

void ActivatePointerGrab(
    DeviceIntPtr /* mouse */,
    GrabPtr /* grab */,
    TimeStamp /* time */,
    Bool /* autoGrab */);

void DeactivatePointerGrab(
    DeviceIntPtr /* mouse */);

void ActivateKeyboardGrab(
    DeviceIntPtr /* keybd */,
    GrabPtr /* grab */,
    TimeStamp /* time */,
    Bool /* passive */);

void DeactivateKeyboardGrab(
    DeviceIntPtr /* keybd */);

void AllowSome(
    ClientPtr	/* client */,
    TimeStamp /* time */,
    DeviceIntPtr /* thisDev */,
    int /* newState */);

void ReleaseActiveGrabs(
    ClientPtr client);

int DeliverEventsToWindow(
    WindowPtr /* pWin */,
    xEventPtr /* pEvents */,
    int /* count */,
    Mask /* filter */,
    GrabPtr /* grab */,
    int /* mskidx */);

int DeliverDeviceEvents(
    WindowPtr /* pWin */,
    xEventPtr /* xE */,
    GrabPtr /* grab */,
    WindowPtr /* stopAt */,
    DeviceIntPtr /* dev */,
    int /* count */);

void DefineInitialRootWindow(
    WindowPtr /* win */);

void WindowHasNewCursor(
    WindowPtr /* pWin */);

Bool CheckDeviceGrabs(
    DeviceIntPtr /* device */,
    xEventPtr /* xE */,
    int /* checkFirst */,
    int /* count */);

void DeliverFocusedEvent(
    DeviceIntPtr /* keybd */,
    xEventPtr /* xE */,
    WindowPtr /* window */,
    int /* count */);

void DeliverGrabbedEvent(
    xEventPtr /* xE */,
    DeviceIntPtr /* thisDev */,
    Bool /* deactivateGrab */,
    int /* count */);


void RecalculateDeliverableEvents(
    WindowPtr /* pWin */);

int OtherClientGone(
    pointer /* value */,
    XID /* id */);

void DoFocusEvents(
    DeviceIntPtr /* dev */,
    WindowPtr /* fromWin */,
    WindowPtr /* toWin */,
    int /* mode */);

int SetInputFocus(
    ClientPtr /* client */,
    DeviceIntPtr /* dev */,
    Window /* focusID */,
    CARD8 /* revertTo */,
    Time /* ctime */,
    Bool /* followOK */);

int GrabDevice(
    ClientPtr /* client */,
    DeviceIntPtr /* dev */,
    unsigned /* this_mode */,
    unsigned /* other_mode */,
    Window /* grabWindow */,
    unsigned /* ownerEvents */,
    Time /* ctime */,
    Mask /* mask */,
    CARD8 * /* status */);

void InitEvents(void);

void CloseDownEvents(void);

void DeleteWindowFromAnyEvents(
    WindowPtr	/* pWin */,
    Bool /* freeResources */);


Mask EventMaskForClient(
    WindowPtr /* pWin */,
    ClientPtr /* client */);



int DeliverEvents(
    WindowPtr /*pWin*/,
    xEventPtr /*xE*/,
    int /*count*/,
    WindowPtr /*otherParent*/);


void WriteEventsToClient(
    ClientPtr /*pClient*/,
    int	     /*count*/,
    xEventPtr /*events*/);

int TryClientEvents(
    ClientPtr /*client*/,
    xEventPtr /*pEvents*/,
    int /*count*/,
    Mask /*mask*/,
    Mask /*filter*/,
    GrabPtr /*grab*/);

void WindowsRestructured(void);


void
ScreenRestructured (ScreenPtr pScreen);

void ResetClientPrivates(void);

int AllocateClientPrivateIndex(void);

Bool AllocateClientPrivate(
    int /*index*/,
    unsigned /*amount*/);

/*
 *  callback manager stuff
 */

#ifndef _XTYPEDEF_CALLBACKLISTPTR
typedef struct _CallbackList *CallbackListPtr; /* also in misc.h */
#define _XTYPEDEF_CALLBACKLISTPTR
#endif

typedef void (*CallbackProcPtr) (
    CallbackListPtr *, pointer, pointer);

typedef Bool (*AddCallbackProcPtr) (
    CallbackListPtr *, CallbackProcPtr, pointer);

typedef Bool (*DeleteCallbackProcPtr) (
    CallbackListPtr *, CallbackProcPtr, pointer);

typedef void (*CallCallbacksProcPtr) (
    CallbackListPtr *, pointer);

typedef void (*DeleteCallbackListProcPtr) (
    CallbackListPtr *);

typedef struct _CallbackProcs {
    AddCallbackProcPtr		AddCallback;
    DeleteCallbackProcPtr	DeleteCallback;
    CallCallbacksProcPtr	CallCallbacks;
    DeleteCallbackListProcPtr	DeleteCallbackList;
} CallbackFuncsRec, *CallbackFuncsPtr;

Bool CreateCallbackList(
    CallbackListPtr * /*pcbl*/,
    CallbackFuncsPtr /*cbfuncs*/);

Bool AddCallback(
    CallbackListPtr * /*pcbl*/,
    CallbackProcPtr /*callback*/,
    pointer /*data*/);

Bool DeleteCallback(
    CallbackListPtr * /*pcbl*/,
    CallbackProcPtr /*callback*/,
    pointer /*data*/);

void CallCallbacks(
    CallbackListPtr * /*pcbl*/,
    pointer /*call_data*/);

void DeleteCallbackList(
    CallbackListPtr * /*pcbl*/);

void InitCallbackManager(void);

/*
 *  ServerGrabCallback stuff
 */

extern CallbackListPtr ServerGrabCallback;

typedef enum {SERVER_GRABBED, SERVER_UNGRABBED,
	      CLIENT_PERVIOUS, CLIENT_IMPERVIOUS } ServerGrabState;

typedef struct {
    ClientPtr client;
    ServerGrabState grabstate;
} ServerGrabInfoRec;

/*
 *  EventCallback stuff
 */

extern CallbackListPtr EventCallback;

typedef struct {
    ClientPtr client;
    xEventPtr events;
    int count;
} EventInfoRec;

/*
 *  DeviceEventCallback stuff
 */

extern CallbackListPtr DeviceEventCallback;

typedef struct {
    xEventPtr events;
    int count;
} DeviceEventInfoRec;

/*
 * SelectionCallback stuff
 */

extern CallbackListPtr SelectionCallback;

typedef enum {
    SelectionSetOwner,
    SelectionWindowDestroy,
    SelectionClientClose
} SelectionCallbackKind;

typedef struct {
    struct _Selection	    *selection;
    SelectionCallbackKind   kind;
} SelectionInfoRec;

/* strcasecmp.c */
#if NEED_STRCASECMP
#define strcasecmp xstrcasecmp
int xstrcasecmp(char *s1, char *s2);
#endif

/* ffs.c */
int ffs(int i);

#endif /* DIX_H */
