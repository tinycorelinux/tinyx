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


#ifndef OS_H
#define OS_H

#include "misc.h"
#include <X11/Xalloca.h>
#include <stdarg.h>

#define NullFID ((FID) 0)

#define SCREEN_SAVER_OFF   0
#define SCREEN_SAVER_ON  1
#define SCREEN_SAVER_FORCER 2
#define SCREEN_SAVER_CYCLE  3

#ifndef MAX_REQUEST_SIZE
#define MAX_REQUEST_SIZE 65535
#endif
#ifndef MAX_BIG_REQUEST_SIZE
#define MAX_BIG_REQUEST_SIZE 4194303
#endif

// Need to mark a set of functions so LTO doesn't mangle them
#if defined(__GNUC__) && __GNUC__ >= 4
#define XFONT_LTO __attribute__((noinline)) __attribute__((used))
#define XFONT_LTO_VAR __attribute__((used))
#else
#define XFONT_LTO
#define XFONT_LTO_VAR
#endif

typedef pointer	FID;
typedef struct _FontPathRec *FontPathPtr;
typedef struct _NewClientRec *NewClientPtr;

#include <stdio.h>
#include <stdarg.h>

/* have to put $(SIGNAL_DEFINES) in DEFINES in Imakefile to get this right */
#ifdef SIGNALRETURNSINT
#define SIGVAL int
#else
#define SIGVAL void
#endif

void (*OsVendorVErrorFProc)(const char *, va_list args);

int WaitForSomething(
    int* /*pClientsReady*/
);

int ReadRequestFromClient(ClientPtr /*client*/);

Bool InsertFakeRequest(
    ClientPtr /*client*/,
    char* /*data*/,
    int /*count*/);

void ResetCurrentRequest(ClientPtr /*client*/);

void FlushAllOutput(void);

void FlushIfCriticalOutputPending(void);

void SetCriticalOutputPending(void);

int WriteToClient(ClientPtr /*who*/, int /*count*/, const char* /*buf*/);

void ResetOsBuffers(void);

void InitConnectionLimits(void);

void CreateWellKnownSockets(void);

void ResetWellKnownSockets(void);

void CloseWellKnownConnections(void);

const char *ClientAuthorized(
    ClientPtr /*client*/,
    unsigned int /*proto_n*/,
    char* /*auth_proto*/,
    unsigned int /*string_n*/,
    char* /*auth_string*/);

Bool EstablishNewConnections(
    ClientPtr /*clientUnused*/,
    pointer /*closure*/);

void CheckConnections(void);

void CloseDownConnection(ClientPtr /*client*/);

void AddGeneralSocket(int /*fd*/);

void RemoveGeneralSocket(int /*fd*/);

void AddEnabledDevice(int /*fd*/);

void RemoveEnabledDevice(int /*fd*/);

void OnlyListenToOneClient(ClientPtr /*client*/);

void ListenToAllClients(void);

void IgnoreClient(ClientPtr /*client*/);

void AttendClient(ClientPtr /*client*/);

void MakeClientGrabImpervious(ClientPtr /*client*/);

void MakeClientGrabPervious(ClientPtr /*client*/);

void AvailableClientInput(ClientPtr /* client */);

CARD32 XFONT_LTO GetTimeInMillis(void);

void AdjustWaitForDelay(
    pointer /*waitTime*/,
    unsigned long /*newdelay*/);

typedef	struct _OsTimerRec *OsTimerPtr;

typedef CARD32 (*OsTimerCallback)(
    OsTimerPtr /* timer */,
    CARD32 /* time */,
    pointer /* arg */);

void TimerInit(void);

Bool TimerForce(OsTimerPtr /* timer */);

#define TimerAbsolute (1<<0)
#define TimerForceOld (1<<1)

OsTimerPtr TimerSet(
    OsTimerPtr /* timer */,
    int /* flags */,
    CARD32 /* millis */,
    OsTimerCallback /* func */,
    pointer /* arg */);

void TimerCancel(OsTimerPtr /* pTimer */);
void TimerFree(OsTimerPtr /* pTimer */);

void SetScreenSaverTimer(void);
void FreeScreenSaverTimer(void);

SIGVAL AutoResetServer(int /*sig*/);

SIGVAL GiveUp(int /*sig*/);

void UseMsg(void);

void ProcessCommandLine(int /*argc*/, char* /*argv*/[]);

XFONT_LTO int set_font_authorizations(
    char ** /* authorizations */,
    int * /*authlen */,
    pointer /* client */);

void OsInitAllocator(void);

typedef SIGVAL (*OsSigHandlerPtr)(int /* sig */);

OsSigHandlerPtr OsSignal(int /* sig */, OsSigHandlerPtr /* handler */);

extern int auditTrailLevel;

void LockServer(void);
void UnlockServer(void);

int OsLookupColor(
    int	/*screen*/,
    char * /*name*/,
    unsigned /*len*/,
    unsigned short * /*pred*/,
    unsigned short * /*pgreen*/,
    unsigned short * /*pblue*/);

void OsInit(void);

void OsCleanup(Bool);

void OsVendorFatalError(void);

void OsVendorInit(void);

void OsBlockSignals (void);

void OsReleaseSignals (void);

int System(char *);
pointer Popen(char *, char *);
int Pclose(pointer);
pointer Fopen(char *, char *);
int Fclose(pointer);

void CheckUserParameters(int argc, char **argv, char **envp);
void CheckUserAuthorization(void);

int AddHost(
    ClientPtr	/*client*/,
    int         /*family*/,
    unsigned    /*length*/,
    const void * /*pAddr*/);

int RemoveHost(
    ClientPtr	/*client*/,
    int         /*family*/,
    unsigned    /*length*/,
    pointer     /*pAddr*/);

int GetHosts(
    pointer * /*data*/,
    int	    * /*pnHosts*/,
    int	    * /*pLen*/,
    BOOL    * /*pEnabled*/);

typedef struct sockaddr * sockaddrPtr;

int InvalidHost(sockaddrPtr /*saddr*/, int /*len*/, ClientPtr client);

int LocalClient(ClientPtr /* client */);

int LocalClientCred(ClientPtr, int *, int *);

int ChangeAccessControl(ClientPtr /*client*/, int /*fEnabled*/);

void AddLocalHosts(void);

void ResetHosts(const char *display);

void EnableLocalHost(void);

void DisableLocalHost(void);

void AccessUsingXdmcp(void);

void DefineSelf(int /*fd*/);

void AugmentSelf(pointer /*from*/, int /*len*/);

void InitAuthorization(char * /*filename*/);

/* int LoadAuthorization(void); */

void RegisterAuthorizations(void);

XID CheckAuthorization(
    unsigned int /*namelength*/,
    const char * /*name*/,
    unsigned int /*datalength*/,
    const char * /*data*/,
    ClientPtr /*client*/,
    const char ** /*reason*/
);

void ResetAuthorization(void);

int AddAuthorization(
    unsigned int	/*name_length*/,
    const char *		/*name*/,
    unsigned int	/*data_length*/,
    char *		/*data*/);

XID GenerateAuthorization(
    unsigned int   /* name_length */,
    char	*  /* name */,
    unsigned int   /* data_length */,
    char	*  /* data */,
    unsigned int * /* data_length_return */,
    char	** /* data_return */);


int ddxProcessArgument(int /*argc*/, char * /*argv*/ [], int /*i*/);

void ddxUseMsg(void);

/* int ReqLen(xReq *req, ClientPtr client)
 * Given a pointer to a *complete* request, return its length in bytes.
 * Note that if the request is a big request (as defined in the Big
 * Requests extension), the macro lies by returning 4 less than the
 * length that it actually occupies in the request buffer.  This is so you
 * can blindly compare the length with the various sz_<request> constants
 * in Xproto.h without having to know/care about big requests.
 */
#define ReqLen(_pxReq, _client) \
 ((_pxReq->length ? \
     (_client->swapped ? lswaps(_pxReq->length) : _pxReq->length) \
  : ((_client->swapped ? \
	lswapl(((CARD32*)_pxReq)[1]) : ((CARD32*)_pxReq)[1])-1) \
  ) << 2)

/* otherReqTypePtr CastxReq(xReq *req, otherReqTypePtr)
 * Cast the given request to one of type otherReqTypePtr to access
 * fields beyond the length field.
 */
#define CastxReq(_pxReq, otherReqTypePtr) \
    (_pxReq->length ? (otherReqTypePtr)_pxReq \
		    : (otherReqTypePtr)(((CARD32*)_pxReq)+1))

typedef struct {
    xReqPtr req;
    ClientPtr client;
    int numskipped;
} SkippedRequestInfoRec;

/* stuff for ReplyCallback */
extern CallbackListPtr ReplyCallback;
typedef struct {
    ClientPtr client;
    const void * replyData;
    unsigned long dataLenBytes;
    unsigned long bytesRemaining;
    Bool startOfReply;
} ReplyInfoRec;

/* stuff for FlushCallback */
extern CallbackListPtr FlushCallback;

void AbortDDX(void);
void ddxGiveUp(void);
int TimeSinceLastInputEvent(void);

/* Logging. */
typedef enum _LogParameter {
    XLOG_FLUSH,
    XLOG_SYNC,
    XLOG_VERBOSITY,
    XLOG_FILE_VERBOSITY
} LogParameter;

/* Flags for log messages. */
typedef enum {
    X_PROBED,			/* Value was probed */
    X_CONFIG,			/* Value was given in the config file */
    X_DEFAULT,			/* Value is a default */
    X_CMDLINE,			/* Value was given on the command line */
    X_NOTICE,			/* Notice */
    X_ERROR,			/* Error message */
    X_WARNING,			/* Warning message */
    X_INFO,			/* Informational message */
    X_NONE,			/* No prefix */
    X_NOT_IMPLEMENTED,		/* Not implemented */
    X_UNKNOWN = -1		/* unknown -- this must always be last */
} MessageType;

/* XXX Need to check which GCC versions have the format(printf) attribute. */
#if defined(__GNUC__) && \
    ((__GNUC__ > 2) || ((__GNUC__ == 2) && (__GNUC_MINOR__ > 4)))
#define _printf_attribute(a,b) __attribute((format(__printf__,a,b)))
#else
#define _printf_attribute(a,b) /**/
#endif

void AuditF(const char *f, ...) _printf_attribute(1,2);
void VAuditF(const char *f, va_list args);
void FatalError(const char *f, ...) _printf_attribute(1,2) XFONT_LTO
#if defined(__GNUC__) && \
    ((__GNUC__ > 2) || ((__GNUC__ == 2) && (__GNUC_MINOR__ > 4)))
__attribute((noreturn))
#endif
;

void VErrorF(const char *f, va_list args);
void ErrorF(const char *f, ...) _printf_attribute(1,2) XFONT_LTO;

#endif /* OS_H */
