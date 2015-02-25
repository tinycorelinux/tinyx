/*
 *
 * Copyright © 2000 SuSE, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of SuSE not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  SuSE makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * SuSE DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL SuSE
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Keith Packard, SuSE, Inc.
 */

#ifdef HAVE_DIX_CONFIG_H
#include <dix-config.h>
#endif

#include <X11/X.h>
#include <X11/Xproto.h>
#include "misc.h"
#include "os.h"
#include "dixstruct.h"
#include "resource.h"
#include "scrnintstr.h"
#include "windowstr.h"
#include "pixmapstr.h"
#include "colormapst.h"
#include "extnsionst.h"
#include "servermd.h"
#include <X11/extensions/render.h>
#include <X11/extensions/renderproto.h>
#include "picturestr.h"
#include "glyphstr.h"
#include <X11/Xfuncproto.h>
#include "cursorstr.h"

#if HAVE_STDINT_H
#include <stdint.h>
#elif !defined(UINT32_MAX)
#define UINT32_MAX 0xffffffffU
#endif

static int ProcRenderQueryVersion(ClientPtr pClient);

static int ProcRenderQueryPictFormats(ClientPtr pClient);

static int ProcRenderQueryPictIndexValues(ClientPtr pClient);

static int ProcRenderQueryDithers(ClientPtr pClient);

static int ProcRenderCreatePicture(ClientPtr pClient);

static int ProcRenderChangePicture(ClientPtr pClient);

static int ProcRenderSetPictureClipRectangles(ClientPtr pClient);

static int ProcRenderFreePicture(ClientPtr pClient);

static int ProcRenderComposite(ClientPtr pClient);

static int ProcRenderScale(ClientPtr pClient);

static int ProcRenderTrapezoids(ClientPtr pClient);

static int ProcRenderTriangles(ClientPtr pClient);

static int ProcRenderTriStrip(ClientPtr pClient);

static int ProcRenderTriFan(ClientPtr pClient);

static int ProcRenderColorTrapezoids(ClientPtr pClient);

static int ProcRenderColorTriangles(ClientPtr pClient);

static int ProcRenderTransform(ClientPtr pClient);

static int ProcRenderCreateGlyphSet(ClientPtr pClient);

static int ProcRenderReferenceGlyphSet(ClientPtr pClient);

static int ProcRenderFreeGlyphSet(ClientPtr pClient);

static int ProcRenderAddGlyphs(ClientPtr pClient);

static int ProcRenderAddGlyphsFromPicture(ClientPtr pClient);

static int ProcRenderFreeGlyphs(ClientPtr pClient);

static int ProcRenderCompositeGlyphs(ClientPtr pClient);

static int ProcRenderFillRectangles(ClientPtr pClient);

static int ProcRenderCreateCursor(ClientPtr pClient);

static int ProcRenderSetPictureTransform(ClientPtr pClient);

static int ProcRenderQueryFilters(ClientPtr pClient);

static int ProcRenderSetPictureFilter(ClientPtr pClient);

static int ProcRenderCreateAnimCursor(ClientPtr pClient);

static int ProcRenderAddTraps(ClientPtr pClient);

static int ProcRenderCreateSolidFill(ClientPtr pClient);

static int ProcRenderCreateLinearGradient(ClientPtr pClient);

static int ProcRenderCreateRadialGradient(ClientPtr pClient);

static int ProcRenderCreateConicalGradient(ClientPtr pClient);

static int ProcRenderDispatch(ClientPtr pClient);

static int SProcRenderQueryVersion(ClientPtr pClient);

static int SProcRenderQueryPictFormats(ClientPtr pClient);

static int SProcRenderQueryPictIndexValues(ClientPtr pClient);

static int SProcRenderQueryDithers(ClientPtr pClient);

static int SProcRenderCreatePicture(ClientPtr pClient);

static int SProcRenderChangePicture(ClientPtr pClient);

static int SProcRenderSetPictureClipRectangles(ClientPtr pClient);

static int SProcRenderFreePicture(ClientPtr pClient);

static int SProcRenderComposite(ClientPtr pClient);

static int SProcRenderScale(ClientPtr pClient);

static int SProcRenderTrapezoids(ClientPtr pClient);

static int SProcRenderTriangles(ClientPtr pClient);

static int SProcRenderTriStrip(ClientPtr pClient);

static int SProcRenderTriFan(ClientPtr pClient);

static int SProcRenderColorTrapezoids(ClientPtr pClient);

static int SProcRenderColorTriangles(ClientPtr pClient);

static int SProcRenderTransform(ClientPtr pClient);

static int SProcRenderCreateGlyphSet(ClientPtr pClient);

static int SProcRenderReferenceGlyphSet(ClientPtr pClient);

static int SProcRenderFreeGlyphSet(ClientPtr pClient);

static int SProcRenderAddGlyphs(ClientPtr pClient);

static int SProcRenderAddGlyphsFromPicture(ClientPtr pClient);

static int SProcRenderFreeGlyphs(ClientPtr pClient);

static int SProcRenderCompositeGlyphs(ClientPtr pClient);

static int SProcRenderFillRectangles(ClientPtr pClient);

static int SProcRenderCreateCursor(ClientPtr pClient);

static int SProcRenderSetPictureTransform(ClientPtr pClient);

static int SProcRenderQueryFilters(ClientPtr pClient);

static int SProcRenderSetPictureFilter(ClientPtr pClient);

static int SProcRenderCreateAnimCursor(ClientPtr pClient);

static int SProcRenderAddTraps(ClientPtr pClient);

static int SProcRenderCreateSolidFill(ClientPtr pClient);

static int SProcRenderCreateLinearGradient(ClientPtr pClient);

static int SProcRenderCreateRadialGradient(ClientPtr pClient);

static int SProcRenderCreateConicalGradient(ClientPtr pClient);

static int SProcRenderDispatch(ClientPtr pClient);

static int (*ProcRenderVector[RenderNumberRequests]) (ClientPtr) = {
ProcRenderQueryVersion,
        ProcRenderQueryPictFormats,
        ProcRenderQueryPictIndexValues,
        ProcRenderQueryDithers,
        ProcRenderCreatePicture,
        ProcRenderChangePicture,
        ProcRenderSetPictureClipRectangles,
        ProcRenderFreePicture,
        ProcRenderComposite,
        ProcRenderScale,
        ProcRenderTrapezoids,
        ProcRenderTriangles,
        ProcRenderTriStrip,
        ProcRenderTriFan,
        ProcRenderColorTrapezoids,
        ProcRenderColorTriangles,
        ProcRenderTransform,
        ProcRenderCreateGlyphSet,
        ProcRenderReferenceGlyphSet,
        ProcRenderFreeGlyphSet,
        ProcRenderAddGlyphs,
        ProcRenderAddGlyphsFromPicture,
        ProcRenderFreeGlyphs,
        ProcRenderCompositeGlyphs,
        ProcRenderCompositeGlyphs,
        ProcRenderCompositeGlyphs,
        ProcRenderFillRectangles,
        ProcRenderCreateCursor,
        ProcRenderSetPictureTransform,
        ProcRenderQueryFilters,
        ProcRenderSetPictureFilter,
        ProcRenderCreateAnimCursor,
        ProcRenderAddTraps,
        ProcRenderCreateSolidFill,
        ProcRenderCreateLinearGradient,
        ProcRenderCreateRadialGradient, ProcRenderCreateConicalGradient};

static int (*SProcRenderVector[RenderNumberRequests]) (ClientPtr) = {
SProcRenderQueryVersion,
        SProcRenderQueryPictFormats,
        SProcRenderQueryPictIndexValues,
        SProcRenderQueryDithers,
        SProcRenderCreatePicture,
        SProcRenderChangePicture,
        SProcRenderSetPictureClipRectangles,
        SProcRenderFreePicture,
        SProcRenderComposite,
        SProcRenderScale,
        SProcRenderTrapezoids,
        SProcRenderTriangles,
        SProcRenderTriStrip,
        SProcRenderTriFan,
        SProcRenderColorTrapezoids,
        SProcRenderColorTriangles,
        SProcRenderTransform,
        SProcRenderCreateGlyphSet,
        SProcRenderReferenceGlyphSet,
        SProcRenderFreeGlyphSet,
        SProcRenderAddGlyphs,
        SProcRenderAddGlyphsFromPicture,
        SProcRenderFreeGlyphs,
        SProcRenderCompositeGlyphs,
        SProcRenderCompositeGlyphs,
        SProcRenderCompositeGlyphs,
        SProcRenderFillRectangles,
        SProcRenderCreateCursor,
        SProcRenderSetPictureTransform,
        SProcRenderQueryFilters,
        SProcRenderSetPictureFilter,
        SProcRenderCreateAnimCursor,
        SProcRenderAddTraps,
        SProcRenderCreateSolidFill,
        SProcRenderCreateLinearGradient,
        SProcRenderCreateRadialGradient, SProcRenderCreateConicalGradient};

static void
 RenderResetProc(ExtensionEntry * extEntry);

#if 0
static CARD8 RenderReqCode;
#endif
int RenderErrBase;

int RenderClientPrivateIndex;

typedef struct _RenderClient {
    int major_version;
    int minor_version;
} RenderClientRec, *RenderClientPtr;

#define GetRenderClient(pClient)    ((RenderClientPtr) (pClient)->devPrivates[RenderClientPrivateIndex].ptr)

static void
RenderClientCallback(CallbackListPtr *list, pointer closure, pointer data)
{
    NewClientInfoRec *clientinfo = (NewClientInfoRec *) data;

    ClientPtr pClient = clientinfo->client;

    RenderClientPtr pRenderClient = GetRenderClient(pClient);

    pRenderClient->major_version = 0;
    pRenderClient->minor_version = 0;
}

void
RenderExtensionInit(void)
{
    ExtensionEntry *extEntry;

    if (!PictureType)
        return;
    if (!PictureFinishInit())
        return;
    RenderClientPrivateIndex = AllocateClientPrivateIndex();
    if (!AllocateClientPrivate(RenderClientPrivateIndex,
                               sizeof(RenderClientRec)))
        return;
    if (!AddCallback(&ClientStateCallback, RenderClientCallback, 0))
        return;

    extEntry = AddExtension(RENDER_NAME, 0, RenderNumberErrors,
                            ProcRenderDispatch, SProcRenderDispatch,
                            RenderResetProc, StandardMinorOpcode);
    if (!extEntry)
        return;
#if 0
    RenderReqCode = (CARD8) extEntry->base;
#endif
    RenderErrBase = extEntry->errorBase;
}

static void
RenderResetProc(ExtensionEntry * extEntry)
{
    ResetPicturePrivateIndex();
    ResetGlyphSetPrivateIndex();
}

static int
ProcRenderQueryVersion(ClientPtr client)
{
    RenderClientPtr pRenderClient = GetRenderClient(client);

    xRenderQueryVersionReply rep = {0};


    REQUEST(xRenderQueryVersionReq);

    pRenderClient->major_version = stuff->majorVersion;
    pRenderClient->minor_version = stuff->minorVersion;

    REQUEST_SIZE_MATCH(xRenderQueryVersionReq);
    rep.type = X_Reply;
    rep.length = 0;
    rep.sequenceNumber = client->sequence;
    rep.majorVersion = RENDER_MAJOR;
    rep.minorVersion = RENDER_MINOR;
    if (client->swapped) {
        swaps(&rep.sequenceNumber);
        swapl(&rep.length);
        swapl(&rep.majorVersion);
        swapl(&rep.minorVersion);
    }
    WriteToClient(client, sizeof(xRenderQueryVersionReply), (char *) &rep);
    return (client->noClientException);
}

static VisualPtr
findVisual(ScreenPtr pScreen, VisualID vid)
{
    VisualPtr pVisual;

    int v;

    for (v = 0; v < pScreen->numVisuals; v++) {
        pVisual = pScreen->visuals + v;
        if (pVisual->vid == vid)
            return pVisual;
    }
    return 0;
}

extern char *ConnectionInfo;

static int
ProcRenderQueryPictFormats(ClientPtr client)
{
    RenderClientPtr pRenderClient = GetRenderClient(client);

    xRenderQueryPictFormatsReply *reply;

    xPictScreen *pictScreen;

    xPictDepth *pictDepth;

    xPictVisual *pictVisual;

    xPictFormInfo *pictForm;

    CARD32 *pictSubpixel;

    ScreenPtr pScreen;

    VisualPtr pVisual;

    DepthPtr pDepth;

    int v, d;

    PictureScreenPtr ps;

    PictFormatPtr pFormat;

    int nformat;
    int ndepth;
    int nvisual;
    int rlength;
    int s;
    int numScreens;
    int numSubpixel;

/*    REQUEST(xRenderQueryPictFormatsReq); */

    REQUEST_SIZE_MATCH(xRenderQueryPictFormatsReq);

    numScreens = screenInfo.numScreens;
    ndepth = nformat = nvisual = 0;
    for (s = 0; s < numScreens; s++) {
        pScreen = screenInfo.screens[s];
        for (d = 0; d < pScreen->numDepths; d++) {
            pDepth = pScreen->allowedDepths + d;
            ++ndepth;

            for (v = 0; v < pDepth->numVids; v++) {
                pVisual = findVisual(pScreen, pDepth->vids[v]);
                if (pVisual &&
                    PictureMatchVisual(pScreen, pDepth->depth, pVisual))
                    ++nvisual;
            }
        }
        ps = GetPictureScreenIfSet(pScreen);
        if (ps)
            nformat += ps->nformats;
    }
    if (pRenderClient->major_version == 0 && pRenderClient->minor_version < 6)
        numSubpixel = 0;
    else
        numSubpixel = numScreens;

    rlength = (sizeof(xRenderQueryPictFormatsReply) +
               nformat * sizeof(xPictFormInfo) +
               numScreens * sizeof(xPictScreen) +
               ndepth * sizeof(xPictDepth) +
               nvisual * sizeof(xPictVisual) + numSubpixel * sizeof(CARD32));
    reply = malloc(rlength);
    if (!reply)
        return BadAlloc;
    reply->type = X_Reply;
    reply->sequenceNumber = client->sequence;
    reply->length = (rlength - sizeof(xGenericReply)) >> 2;
    reply->numFormats = nformat;
    reply->numScreens = numScreens;
    reply->numDepths = ndepth;
    reply->numVisuals = nvisual;
    reply->numSubpixel = numSubpixel;

    pictForm = (xPictFormInfo *) (reply + 1);

    for (s = 0; s < numScreens; s++) {
        pScreen = screenInfo.screens[s];
        ps = GetPictureScreenIfSet(pScreen);
        if (ps) {
            for (nformat = 0, pFormat = ps->formats;
                 nformat < ps->nformats; nformat++, pFormat++) {
                pictForm->id = pFormat->id;
                pictForm->type = pFormat->type;
                pictForm->depth = pFormat->depth;
                pictForm->direct.red = pFormat->direct.red;
                pictForm->direct.redMask = pFormat->direct.redMask;
                pictForm->direct.green = pFormat->direct.green;
                pictForm->direct.greenMask = pFormat->direct.greenMask;
                pictForm->direct.blue = pFormat->direct.blue;
                pictForm->direct.blueMask = pFormat->direct.blueMask;
                pictForm->direct.alpha = pFormat->direct.alpha;
                pictForm->direct.alphaMask = pFormat->direct.alphaMask;
                if (pFormat->type == PictTypeIndexed &&
                    pFormat->index.pColormap)
                    pictForm->colormap = pFormat->index.pColormap->mid;
                else
                    pictForm->colormap = None;
                if (client->swapped) {
                    swapl(&pictForm->id);
                    swaps(&pictForm->direct.red);
                    swaps(&pictForm->direct.redMask);
                    swaps(&pictForm->direct.green);
                    swaps(&pictForm->direct.greenMask);
                    swaps(&pictForm->direct.blue);
                    swaps(&pictForm->direct.blueMask);
                    swaps(&pictForm->direct.alpha);
                    swaps(&pictForm->direct.alphaMask);
                    swapl(&pictForm->colormap);
                }
                pictForm++;
            }
        }
    }

    pictScreen = (xPictScreen *) pictForm;
    for (s = 0; s < numScreens; s++) {
        pScreen = screenInfo.screens[s];
        pictDepth = (xPictDepth *) (pictScreen + 1);
        ndepth = 0;
        for (d = 0; d < pScreen->numDepths; d++) {
            pictVisual = (xPictVisual *) (pictDepth + 1);
            pDepth = pScreen->allowedDepths + d;

            nvisual = 0;
            for (v = 0; v < pDepth->numVids; v++) {
                pVisual = findVisual(pScreen, pDepth->vids[v]);
                if (pVisual && (pFormat = PictureMatchVisual(pScreen,
                                                             pDepth->depth,
                                                             pVisual))) {
                    pictVisual->visual = pVisual->vid;
                    pictVisual->format = pFormat->id;
                    if (client->swapped) {
                        swapl(&pictVisual->visual);
                        swapl(&pictVisual->format);
                    }
                    pictVisual++;
                    nvisual++;
                }
            }
            pictDepth->depth = pDepth->depth;
            pictDepth->nPictVisuals = nvisual;
            if (client->swapped) {
                swaps(&pictDepth->nPictVisuals);
            }
            ndepth++;
            pictDepth = (xPictDepth *) pictVisual;
        }
        pictScreen->nDepth = ndepth;
        ps = GetPictureScreenIfSet(pScreen);
        if (ps)
            pictScreen->fallback = ps->fallback->id;
        else
            pictScreen->fallback = 0;
        if (client->swapped) {
            swapl(&pictScreen->nDepth);
            swapl(&pictScreen->fallback);
        }
        pictScreen = (xPictScreen *) pictDepth;
    }
    pictSubpixel = (CARD32 *) pictScreen;

    for (s = 0; s < numSubpixel; s++) {
        pScreen = screenInfo.screens[s];
        ps = GetPictureScreenIfSet(pScreen);
        if (ps)
            *pictSubpixel = ps->subpixel;
        else
            *pictSubpixel = SubPixelUnknown;
        if (client->swapped) {
            swapl(pictSubpixel);
        }
        ++pictSubpixel;
    }

    if (client->swapped) {
        swaps(&reply->sequenceNumber);
        swapl(&reply->length);
        swapl(&reply->numFormats);
        swapl(&reply->numScreens);
        swapl(&reply->numDepths);
        swapl(&reply->numVisuals);
        swapl(&reply->numSubpixel);
    }
    WriteToClient(client, rlength, (char *) reply);
    free(reply);
    return client->noClientException;
}

static int
ProcRenderQueryPictIndexValues(ClientPtr client)
{
    PictFormatPtr pFormat;

    int num;

    int rlength;

    int i;

    REQUEST(xRenderQueryPictIndexValuesReq);
    xRenderQueryPictIndexValuesReply *reply;

    xIndexValue *values;

    REQUEST_AT_LEAST_SIZE(xRenderQueryPictIndexValuesReq);

    pFormat = (PictFormatPtr) SecurityLookupIDByType(client,
                                                     stuff->format,
                                                     PictFormatType,
                                                     SecurityReadAccess);

    if (!pFormat) {
        client->errorValue = stuff->format;
        return RenderErrBase + BadPictFormat;
    }
    if (pFormat->type != PictTypeIndexed) {
        client->errorValue = stuff->format;
        return BadMatch;
    }
    num = pFormat->index.nvalues;
    rlength = (sizeof(xRenderQueryPictIndexValuesReply) +
               num * sizeof(xIndexValue));
    reply = calloc(1, rlength);
    if (!reply)
        return BadAlloc;

    reply->type = X_Reply;
    reply->sequenceNumber = client->sequence;
    reply->length = (rlength - sizeof(xGenericReply)) >> 2;
    reply->numIndexValues = num;

    values = (xIndexValue *) (reply + 1);

    memcpy(reply + 1, pFormat->index.pValues, num * sizeof(xIndexValue));

    if (client->swapped) {
        for (i = 0; i < num; i++) {
            swapl(&values[i].pixel);
            swaps(&values[i].red);
            swaps(&values[i].green);
            swaps(&values[i].blue);
            swaps(&values[i].alpha);
        }
        swaps(&reply->sequenceNumber);
        swapl(&reply->length);
        swapl(&reply->numIndexValues);
    }

    WriteToClient(client, rlength, (char *) reply);
    free(reply);
    return (client->noClientException);
}

static int
ProcRenderQueryDithers(ClientPtr client)
{
    return BadImplementation;
}

static int
ProcRenderCreatePicture(ClientPtr client)
{
    PicturePtr pPicture;

    DrawablePtr pDrawable;

    PictFormatPtr pFormat;

    int len;

    int error;

    REQUEST(xRenderCreatePictureReq);

    REQUEST_AT_LEAST_SIZE(xRenderCreatePictureReq);

    LEGAL_NEW_RESOURCE(stuff->pid, client);
    SECURITY_VERIFY_DRAWABLE(pDrawable, stuff->drawable, client,
                             SecurityWriteAccess);
    pFormat = (PictFormatPtr) SecurityLookupIDByType(client,
                                                     stuff->format,
                                                     PictFormatType,
                                                     SecurityReadAccess);
    if (!pFormat) {
        client->errorValue = stuff->format;
        return RenderErrBase + BadPictFormat;
    }
    if (pFormat->depth != pDrawable->depth)
        return BadMatch;
    len = client->req_len - (sizeof(xRenderCreatePictureReq) >> 2);
    if (Ones(stuff->mask) != len)
        return BadLength;

    pPicture = CreatePicture(stuff->pid,
                             pDrawable,
                             pFormat,
                             stuff->mask, (XID *) (stuff + 1), client, &error);
    if (!pPicture)
        return error;
    if (!AddResource(stuff->pid, PictureType, (pointer) pPicture))
        return BadAlloc;
    return Success;
}

static int
ProcRenderChangePicture(ClientPtr client)
{
    PicturePtr pPicture;

    REQUEST(xRenderChangePictureReq);
    int len;

    REQUEST_AT_LEAST_SIZE(xRenderChangePictureReq);
    VERIFY_PICTURE(pPicture, stuff->picture, client, SecurityWriteAccess,
                   RenderErrBase + BadPicture);

    len = client->req_len - (sizeof(xRenderChangePictureReq) >> 2);
    if (Ones(stuff->mask) != len)
        return BadLength;

    return ChangePicture(pPicture, stuff->mask, (XID *) (stuff + 1),
                         (DevUnion *) 0, client);
}

static int
ProcRenderSetPictureClipRectangles(ClientPtr client)
{
    REQUEST(xRenderSetPictureClipRectanglesReq);
    PicturePtr pPicture;

    int nr;

    int result;

    REQUEST_AT_LEAST_SIZE(xRenderSetPictureClipRectanglesReq);
    VERIFY_PICTURE(pPicture, stuff->picture, client, SecurityWriteAccess,
                   RenderErrBase + BadPicture);
    if (!pPicture->pDrawable)
        return BadDrawable;

    nr = (client->req_len << 2) - sizeof(xRenderChangePictureReq);
    if (nr & 4)
        return BadLength;
    nr >>= 3;
    result = SetPictureClipRects(pPicture,
                                 stuff->xOrigin, stuff->yOrigin,
                                 nr, (xRectangle *) &stuff[1]);
    if (client->noClientException != Success)
        return (client->noClientException);
    else
        return (result);
}

static int
ProcRenderFreePicture(ClientPtr client)
{
    PicturePtr pPicture;

    REQUEST(xRenderFreePictureReq);

    REQUEST_SIZE_MATCH(xRenderFreePictureReq);

    VERIFY_PICTURE(pPicture, stuff->picture, client, SecurityDestroyAccess,
                   RenderErrBase + BadPicture);
    FreeResource(stuff->picture, RT_NONE);
    return (client->noClientException);
}

static Bool
PictOpValid(CARD8 op)
{
    if ( /*PictOpMinimum <= op && */ op <= PictOpMaximum)
        return TRUE;
    if (PictOpDisjointMinimum <= op && op <= PictOpDisjointMaximum)
        return TRUE;
    if (PictOpConjointMinimum <= op && op <= PictOpConjointMaximum)
        return TRUE;
    return FALSE;
}

static int
ProcRenderComposite(ClientPtr client)
{
    PicturePtr pSrc, pMask, pDst;

    REQUEST(xRenderCompositeReq);

    REQUEST_SIZE_MATCH(xRenderCompositeReq);
    if (!PictOpValid(stuff->op)) {
        client->errorValue = stuff->op;
        return BadValue;
    }
    VERIFY_PICTURE(pDst, stuff->dst, client, SecurityWriteAccess,
                   RenderErrBase + BadPicture);
    if (!pDst->pDrawable)
        return BadDrawable;
    VERIFY_PICTURE(pSrc, stuff->src, client, SecurityReadAccess,
                   RenderErrBase + BadPicture);
    VERIFY_ALPHA(pMask, stuff->mask, client, SecurityReadAccess,
                 RenderErrBase + BadPicture);
    if ((pSrc->pDrawable &&
         pSrc->pDrawable->pScreen != pDst->pDrawable->pScreen) || (pMask &&
                                                                   pMask->
                                                                   pDrawable &&
                                                                   pDst->
                                                                   pDrawable->
                                                                   pScreen !=
                                                                   pMask->
                                                                   pDrawable->
                                                                   pScreen))
        return BadMatch;
    CompositePicture(stuff->op,
                     pSrc,
                     pMask,
                     pDst,
                     stuff->xSrc,
                     stuff->ySrc,
                     stuff->xMask,
                     stuff->yMask,
                     stuff->xDst, stuff->yDst, stuff->width, stuff->height);
    return Success;
}

static int
ProcRenderScale(ClientPtr client)
{
    return BadImplementation;
}

static int
ProcRenderTrapezoids(ClientPtr client)
{
    int ntraps;

    PicturePtr pSrc, pDst;

    PictFormatPtr pFormat;

    REQUEST(xRenderTrapezoidsReq);

    REQUEST_AT_LEAST_SIZE(xRenderTrapezoidsReq);
    if (!PictOpValid(stuff->op)) {
        client->errorValue = stuff->op;
        return BadValue;
    }
    VERIFY_PICTURE(pSrc, stuff->src, client, SecurityReadAccess,
                   RenderErrBase + BadPicture);
    VERIFY_PICTURE(pDst, stuff->dst, client, SecurityWriteAccess,
                   RenderErrBase + BadPicture);
    if (!pDst->pDrawable)
        return BadDrawable;
    if (pSrc->pDrawable && pSrc->pDrawable->pScreen != pDst->pDrawable->pScreen)
        return BadMatch;
    if (stuff->maskFormat) {
        pFormat = (PictFormatPtr) SecurityLookupIDByType(client,
                                                         stuff->maskFormat,
                                                         PictFormatType,
                                                         SecurityReadAccess);
        if (!pFormat) {
            client->errorValue = stuff->maskFormat;
            return RenderErrBase + BadPictFormat;
        }
    }
    else
        pFormat = 0;
    ntraps = (client->req_len << 2) - sizeof(xRenderTrapezoidsReq);
    if (ntraps % sizeof(xTrapezoid))
        return BadLength;
    ntraps /= sizeof(xTrapezoid);
    if (ntraps)
        CompositeTrapezoids(stuff->op, pSrc, pDst, pFormat,
                            stuff->xSrc, stuff->ySrc,
                            ntraps, (xTrapezoid *) & stuff[1]);
    return client->noClientException;
}

static int
ProcRenderTriangles(ClientPtr client)
{
    int ntris;

    PicturePtr pSrc, pDst;

    PictFormatPtr pFormat;

    REQUEST(xRenderTrianglesReq);

    REQUEST_AT_LEAST_SIZE(xRenderTrianglesReq);
    if (!PictOpValid(stuff->op)) {
        client->errorValue = stuff->op;
        return BadValue;
    }
    VERIFY_PICTURE(pSrc, stuff->src, client, SecurityReadAccess,
                   RenderErrBase + BadPicture);
    VERIFY_PICTURE(pDst, stuff->dst, client, SecurityWriteAccess,
                   RenderErrBase + BadPicture);
    if (!pDst->pDrawable)
        return BadDrawable;
    if (pSrc->pDrawable && pSrc->pDrawable->pScreen != pDst->pDrawable->pScreen)
        return BadMatch;
    if (stuff->maskFormat) {
        pFormat = (PictFormatPtr) SecurityLookupIDByType(client,
                                                         stuff->maskFormat,
                                                         PictFormatType,
                                                         SecurityReadAccess);
        if (!pFormat) {
            client->errorValue = stuff->maskFormat;
            return RenderErrBase + BadPictFormat;
        }
    }
    else
        pFormat = 0;
    ntris = (client->req_len << 2) - sizeof(xRenderTrianglesReq);
    if (ntris % sizeof(xTriangle))
        return BadLength;
    ntris /= sizeof(xTriangle);
    if (ntris)
        CompositeTriangles(stuff->op, pSrc, pDst, pFormat,
                           stuff->xSrc, stuff->ySrc,
                           ntris, (xTriangle *) & stuff[1]);
    return client->noClientException;
}

static int
ProcRenderTriStrip(ClientPtr client)
{
    int npoints;

    PicturePtr pSrc, pDst;

    PictFormatPtr pFormat;

    REQUEST(xRenderTrianglesReq);

    REQUEST_AT_LEAST_SIZE(xRenderTrianglesReq);
    if (!PictOpValid(stuff->op)) {
        client->errorValue = stuff->op;
        return BadValue;
    }
    VERIFY_PICTURE(pSrc, stuff->src, client, SecurityReadAccess,
                   RenderErrBase + BadPicture);
    VERIFY_PICTURE(pDst, stuff->dst, client, SecurityWriteAccess,
                   RenderErrBase + BadPicture);
    if (!pDst->pDrawable)
        return BadDrawable;
    if (pSrc->pDrawable && pSrc->pDrawable->pScreen != pDst->pDrawable->pScreen)
        return BadMatch;
    if (stuff->maskFormat) {
        pFormat = (PictFormatPtr) SecurityLookupIDByType(client,
                                                         stuff->maskFormat,
                                                         PictFormatType,
                                                         SecurityReadAccess);
        if (!pFormat) {
            client->errorValue = stuff->maskFormat;
            return RenderErrBase + BadPictFormat;
        }
    }
    else
        pFormat = 0;
    npoints = ((client->req_len << 2) - sizeof(xRenderTriStripReq));
    if (npoints & 4)
        return (BadLength);
    npoints >>= 3;
    if (npoints >= 3)
        CompositeTriStrip(stuff->op, pSrc, pDst, pFormat,
                          stuff->xSrc, stuff->ySrc,
                          npoints, (xPointFixed *) & stuff[1]);
    return client->noClientException;
}

static int
ProcRenderTriFan(ClientPtr client)
{
    int npoints;

    PicturePtr pSrc, pDst;

    PictFormatPtr pFormat;

    REQUEST(xRenderTrianglesReq);

    REQUEST_AT_LEAST_SIZE(xRenderTrianglesReq);
    if (!PictOpValid(stuff->op)) {
        client->errorValue = stuff->op;
        return BadValue;
    }
    VERIFY_PICTURE(pSrc, stuff->src, client, SecurityReadAccess,
                   RenderErrBase + BadPicture);
    VERIFY_PICTURE(pDst, stuff->dst, client, SecurityWriteAccess,
                   RenderErrBase + BadPicture);
    if (!pDst->pDrawable)
        return BadDrawable;
    if (pSrc->pDrawable && pSrc->pDrawable->pScreen != pDst->pDrawable->pScreen)
        return BadMatch;
    if (stuff->maskFormat) {
        pFormat = (PictFormatPtr) SecurityLookupIDByType(client,
                                                         stuff->maskFormat,
                                                         PictFormatType,
                                                         SecurityReadAccess);
        if (!pFormat) {
            client->errorValue = stuff->maskFormat;
            return RenderErrBase + BadPictFormat;
        }
    }
    else
        pFormat = 0;
    npoints = ((client->req_len << 2) - sizeof(xRenderTriStripReq));
    if (npoints & 4)
        return (BadLength);
    npoints >>= 3;
    if (npoints >= 3)
        CompositeTriFan(stuff->op, pSrc, pDst, pFormat,
                        stuff->xSrc, stuff->ySrc,
                        npoints, (xPointFixed *) & stuff[1]);
    return client->noClientException;
}

static int
ProcRenderColorTrapezoids(ClientPtr client)
{
    return BadImplementation;
}

static int
ProcRenderColorTriangles(ClientPtr client)
{
    return BadImplementation;
}

static int
ProcRenderTransform(ClientPtr client)
{
    return BadImplementation;
}

static int
ProcRenderCreateGlyphSet(ClientPtr client)
{
    GlyphSetPtr glyphSet;

    PictFormatPtr format;

    int f;

    REQUEST(xRenderCreateGlyphSetReq);

    REQUEST_SIZE_MATCH(xRenderCreateGlyphSetReq);

    LEGAL_NEW_RESOURCE(stuff->gsid, client);
    format = (PictFormatPtr) SecurityLookupIDByType(client,
                                                    stuff->format,
                                                    PictFormatType,
                                                    SecurityReadAccess);
    if (!format) {
        client->errorValue = stuff->format;
        return RenderErrBase + BadPictFormat;
    }
    switch (format->depth) {
    case 1:
        f = GlyphFormat1;
        break;
    case 4:
        f = GlyphFormat4;
        break;
    case 8:
        f = GlyphFormat8;
        break;
    case 16:
        f = GlyphFormat16;
        break;
    case 32:
        f = GlyphFormat32;
        break;
    default:
        return BadMatch;
    }
    if (format->type != PictTypeDirect)
        return BadMatch;
    glyphSet = AllocateGlyphSet(f, format);
    if (!glyphSet)
        return BadAlloc;
    if (!AddResource(stuff->gsid, GlyphSetType, (pointer) glyphSet))
        return BadAlloc;
    return Success;
}

static int
ProcRenderReferenceGlyphSet(ClientPtr client)
{
    GlyphSetPtr glyphSet;

    REQUEST(xRenderReferenceGlyphSetReq);

    REQUEST_SIZE_MATCH(xRenderReferenceGlyphSetReq);

    LEGAL_NEW_RESOURCE(stuff->gsid, client);

    glyphSet = (GlyphSetPtr) SecurityLookupIDByType(client,
                                                    stuff->existing,
                                                    GlyphSetType,
                                                    SecurityWriteAccess);
    if (!glyphSet) {
        client->errorValue = stuff->existing;
        return RenderErrBase + BadGlyphSet;
    }
    glyphSet->refcnt++;
    if (!AddResource(stuff->gsid, GlyphSetType, (pointer) glyphSet))
        return BadAlloc;
    return client->noClientException;
}

#define NLOCALDELTA	64
#define NLOCALGLYPH	256

static int
ProcRenderFreeGlyphSet(ClientPtr client)
{
    GlyphSetPtr glyphSet;

    REQUEST(xRenderFreeGlyphSetReq);

    REQUEST_SIZE_MATCH(xRenderFreeGlyphSetReq);
    glyphSet = (GlyphSetPtr) SecurityLookupIDByType(client,
                                                    stuff->glyphset,
                                                    GlyphSetType,
                                                    SecurityDestroyAccess);
    if (!glyphSet) {
        client->errorValue = stuff->glyphset;
        return RenderErrBase + BadGlyphSet;
    }
    FreeResource(stuff->glyphset, RT_NONE);
    return client->noClientException;
}

typedef struct _GlyphNew {
    Glyph id;
    GlyphPtr glyph;
} GlyphNewRec, *GlyphNewPtr;

static int
ProcRenderAddGlyphs(ClientPtr client)
{
    GlyphSetPtr glyphSet;

    REQUEST(xRenderAddGlyphsReq);
    GlyphNewRec glyphsLocal[NLOCALGLYPH];

    GlyphNewPtr glyphsBase, glyphs;

    GlyphPtr glyph;

    int remain, nglyphs;

    CARD32 *gids;

    xGlyphInfo *gi;

    CARD8 *bits;

    int size;

    int err = BadAlloc;

    REQUEST_AT_LEAST_SIZE(xRenderAddGlyphsReq);
    glyphSet = (GlyphSetPtr) SecurityLookupIDByType(client,
                                                    stuff->glyphset,
                                                    GlyphSetType,
                                                    SecurityWriteAccess);
    if (!glyphSet) {
        client->errorValue = stuff->glyphset;
        return RenderErrBase + BadGlyphSet;
    }

    nglyphs = stuff->nglyphs;
    if (nglyphs > UINT32_MAX / sizeof(GlyphNewRec))
        return BadAlloc;

    if (nglyphs <= NLOCALGLYPH)
        glyphsBase = glyphsLocal;
    else {
        glyphsBase = malloc(nglyphs * sizeof(GlyphNewRec));
        if (!glyphsBase)
            return BadAlloc;
    }

    remain = (client->req_len << 2) - sizeof(xRenderAddGlyphsReq);

    glyphs = glyphsBase;

    gids = (CARD32 *) (stuff + 1);
    gi = (xGlyphInfo *) (gids + nglyphs);
    bits = (CARD8 *) (gi + nglyphs);
    remain -= (sizeof(CARD32) + sizeof(xGlyphInfo)) * nglyphs;
    while (remain >= 0 && nglyphs) {
        glyph = AllocateGlyph(gi, glyphSet->fdepth);
        if (!glyph) {
            err = BadAlloc;
            goto bail;
        }

        glyphs->glyph = glyph;
        glyphs->id = *gids;

        size = glyph->size - sizeof(xGlyphInfo);
        if (remain < size)
            break;
        memcpy((CARD8 *) (glyph + 1), bits, size);

        if (size & 3)
            size += 4 - (size & 3);
        bits += size;
        remain -= size;
        gi++;
        gids++;
        glyphs++;
        nglyphs--;
    }
    if (nglyphs || remain) {
        err = BadLength;
        goto bail;
    }
    nglyphs = stuff->nglyphs;
    if (!ResizeGlyphSet(glyphSet, nglyphs)) {
        err = BadAlloc;
        goto bail;
    }
    glyphs = glyphsBase;
    while (nglyphs--) {
        AddGlyph(glyphSet, glyphs->glyph, glyphs->id);
        glyphs++;
    }

    if (glyphsBase != glyphsLocal)
        free(glyphsBase);
    return client->noClientException;
 bail:
    while (glyphs != glyphsBase) {
        --glyphs;
        free(glyphs->glyph);
    }
    if (glyphsBase != glyphsLocal)
        free(glyphsBase);
    return err;
}

static int
ProcRenderAddGlyphsFromPicture(ClientPtr client)
{
    return BadImplementation;
}

static int
ProcRenderFreeGlyphs(ClientPtr client)
{
    REQUEST(xRenderFreeGlyphsReq);
    GlyphSetPtr glyphSet;

    int nglyph;

    CARD32 *gids;

    CARD32 glyph;

    REQUEST_AT_LEAST_SIZE(xRenderFreeGlyphsReq);
    glyphSet = (GlyphSetPtr) SecurityLookupIDByType(client,
                                                    stuff->glyphset,
                                                    GlyphSetType,
                                                    SecurityWriteAccess);
    if (!glyphSet) {
        client->errorValue = stuff->glyphset;
        return RenderErrBase + BadGlyphSet;
    }
    nglyph = ((client->req_len << 2) - sizeof(xRenderFreeGlyphsReq)) >> 2;
    gids = (CARD32 *) (stuff + 1);
    while (nglyph-- > 0) {
        glyph = *gids++;
        if (!DeleteGlyph(glyphSet, glyph)) {
            client->errorValue = glyph;
            return RenderErrBase + BadGlyph;
        }
    }
    return client->noClientException;
}

static int
ProcRenderCompositeGlyphs(ClientPtr client)
{
    GlyphSetPtr glyphSet;

    GlyphSet gs;

    PicturePtr pSrc, pDst;

    PictFormatPtr pFormat;

    GlyphListRec listsLocal[NLOCALDELTA];

    GlyphListPtr lists, listsBase;

    GlyphPtr glyphsLocal[NLOCALGLYPH];

    Glyph glyph;

    GlyphPtr *glyphs, *glyphsBase;

    xGlyphElt *elt;

    CARD8 *buffer, *end;

    int nglyph;

    int nlist;

    int space;

    int size;

    int n;

    REQUEST(xRenderCompositeGlyphsReq);

    REQUEST_AT_LEAST_SIZE(xRenderCompositeGlyphsReq);

    switch (stuff->renderReqType) {
    default:
        size = 1;
        break;
    case X_RenderCompositeGlyphs16:
        size = 2;
        break;
    case X_RenderCompositeGlyphs32:
        size = 4;
        break;
    }

    if (!PictOpValid(stuff->op)) {
        client->errorValue = stuff->op;
        return BadValue;
    }
    VERIFY_PICTURE(pSrc, stuff->src, client, SecurityReadAccess,
                   RenderErrBase + BadPicture);
    VERIFY_PICTURE(pDst, stuff->dst, client, SecurityWriteAccess,
                   RenderErrBase + BadPicture);
    if (!pDst->pDrawable)
        return BadDrawable;
    if (pSrc->pDrawable && pSrc->pDrawable->pScreen != pDst->pDrawable->pScreen)
        return BadMatch;
    if (stuff->maskFormat) {
        pFormat = (PictFormatPtr) SecurityLookupIDByType(client,
                                                         stuff->maskFormat,
                                                         PictFormatType,
                                                         SecurityReadAccess);
        if (!pFormat) {
            client->errorValue = stuff->maskFormat;
            return RenderErrBase + BadPictFormat;
        }
    }
    else
        pFormat = 0;

    glyphSet = (GlyphSetPtr) SecurityLookupIDByType(client,
                                                    stuff->glyphset,
                                                    GlyphSetType,
                                                    SecurityReadAccess);
    if (!glyphSet) {
        client->errorValue = stuff->glyphset;
        return RenderErrBase + BadGlyphSet;
    }

    buffer = (CARD8 *) (stuff + 1);
    end = (CARD8 *) stuff + (client->req_len << 2);
    nglyph = 0;
    nlist = 0;
    while (buffer + sizeof(xGlyphElt) < end) {
        elt = (xGlyphElt *) buffer;
        buffer += sizeof(xGlyphElt);

        if (elt->len == 0xff) {
            buffer += 4;
        }
        else {
            nlist++;
            nglyph += elt->len;
            space = size * elt->len;
            if (space & 3)
                space += 4 - (space & 3);
            buffer += space;
        }
    }
    if (nglyph <= NLOCALGLYPH)
        glyphsBase = glyphsLocal;
    else {
        glyphsBase = (GlyphPtr *) ALLOCATE_LOCAL(nglyph * sizeof(GlyphPtr));
        if (!glyphsBase)
            return BadAlloc;
    }
    if (nlist <= NLOCALDELTA)
        listsBase = listsLocal;
    else {
        listsBase = (GlyphListPtr) ALLOCATE_LOCAL(nlist * sizeof(GlyphListRec));
        if (!listsBase)
            return BadAlloc;
    }
    buffer = (CARD8 *) (stuff + 1);
    glyphs = glyphsBase;
    lists = listsBase;
    while (buffer + sizeof(xGlyphElt) < end) {
        elt = (xGlyphElt *) buffer;
        buffer += sizeof(xGlyphElt);

        if (elt->len == 0xff) {
            if (buffer + sizeof(GlyphSet) < end) {
                memcpy(&gs, buffer, sizeof(GlyphSet));
                glyphSet = (GlyphSetPtr) SecurityLookupIDByType(client,
                                                                gs,
                                                                GlyphSetType,
                                                                SecurityReadAccess);
                if (!glyphSet) {
                    client->errorValue = gs;
                    if (glyphsBase != glyphsLocal)
                        DEALLOCATE_LOCAL(glyphsBase);
                    if (listsBase != listsLocal)
                        DEALLOCATE_LOCAL(listsBase);
                    return RenderErrBase + BadGlyphSet;
                }
            }
            buffer += 4;
        }
        else {
            lists->xOff = elt->deltax;
            lists->yOff = elt->deltay;
            lists->format = glyphSet->format;
            lists->len = 0;
            n = elt->len;
            while (n--) {
                if (buffer + size <= end) {
                    switch (size) {
                    case 1:
                        glyph = *((CARD8 *) buffer);
                        break;
                    case 2:
                        glyph = *((CARD16 *) buffer);
                        break;
                    case 4:
                    default:
                        glyph = *((CARD32 *) buffer);
                        break;
                    }
                    if ((*glyphs = FindGlyph(glyphSet, glyph))) {
                        lists->len++;
                        glyphs++;
                    }
                }
                buffer += size;
            }
            space = size * elt->len;
            if (space & 3)
                buffer += 4 - (space & 3);
            lists++;
        }
    }
    if (buffer > end)
        return BadLength;

    CompositeGlyphs(stuff->op,
                    pSrc,
                    pDst,
                    pFormat,
                    stuff->xSrc, stuff->ySrc, nlist, listsBase, glyphsBase);

    if (glyphsBase != glyphsLocal)
        DEALLOCATE_LOCAL(glyphsBase);
    if (listsBase != listsLocal)
        DEALLOCATE_LOCAL(listsBase);

    return client->noClientException;
}

static int
ProcRenderFillRectangles(ClientPtr client)
{
    PicturePtr pDst;

    int things;

    REQUEST(xRenderFillRectanglesReq);

    REQUEST_AT_LEAST_SIZE(xRenderFillRectanglesReq);
    if (!PictOpValid(stuff->op)) {
        client->errorValue = stuff->op;
        return BadValue;
    }
    VERIFY_PICTURE(pDst, stuff->dst, client, SecurityWriteAccess,
                   RenderErrBase + BadPicture);
    if (!pDst->pDrawable)
        return BadDrawable;

    things = (client->req_len << 2) - sizeof(xRenderFillRectanglesReq);
    if (things & 4)
        return (BadLength);
    things >>= 3;

    CompositeRects(stuff->op,
                   pDst, &stuff->color, things, (xRectangle *) &stuff[1]);

    return client->noClientException;
}

static void
SetBit(unsigned char *line, int x, int bit)
{
    unsigned char mask;

    if (screenInfo.bitmapBitOrder == LSBFirst)
        mask = (1 << (x & 7));
    else
        mask = (0x80 >> (x & 7));
    /* XXX assumes byte order is host byte order */
    line += (x >> 3);
    if (bit)
        *line |= mask;
    else
        *line &= ~mask;
}

#define DITHER_DIM 2

static CARD32 orderedDither[DITHER_DIM][DITHER_DIM] = {
    {1, 3,},
    {4, 2,},
};

#define DITHER_SIZE  ((sizeof orderedDither / sizeof orderedDither[0][0]) + 1)

static int
ProcRenderCreateCursor(ClientPtr client)
{
    REQUEST(xRenderCreateCursorReq);
    PicturePtr pSrc;

    ScreenPtr pScreen;

    unsigned short width, height;

    CARD32 *argbbits, *argb;

    unsigned char *srcbits, *srcline;

    unsigned char *mskbits, *mskline;

    int stride;

    int x, y;

    int nbytes_mono;

    CursorMetricRec cm;

    CursorPtr pCursor;

    CARD32 twocolor[3];

    int ncolor;

    REQUEST_SIZE_MATCH(xRenderCreateCursorReq);
    LEGAL_NEW_RESOURCE(stuff->cid, client);

    VERIFY_PICTURE(pSrc, stuff->src, client, SecurityReadAccess,
                   RenderErrBase + BadPicture);
    if (!pSrc->pDrawable)
        return BadDrawable;
    pScreen = pSrc->pDrawable->pScreen;
    width = pSrc->pDrawable->width;
    height = pSrc->pDrawable->height;
    if (stuff->x > width || stuff->y > height)
        return (BadMatch);
    argbbits = malloc(width * height * sizeof(CARD32));
    if (!argbbits)
        return (BadAlloc);

    stride = BitmapBytePad(width);
    nbytes_mono = stride * height;
    srcbits = malloc(nbytes_mono);
    if (!srcbits) {
        free(argbbits);
        return (BadAlloc);
    }
    mskbits = malloc(nbytes_mono);
    if (!mskbits) {
        free(argbbits);
        free(srcbits);
        return (BadAlloc);
    }
    bzero((char *) mskbits, nbytes_mono);
    bzero((char *) srcbits, nbytes_mono);

    if (pSrc->format == PICT_a8r8g8b8) {
        (*pScreen->GetImage) (pSrc->pDrawable,
                              0, 0, width, height, ZPixmap,
                              0xffffffff, (pointer) argbbits);
    }
    else {
        PixmapPtr pPixmap;

        PicturePtr pPicture;

        PictFormatPtr pFormat;

        int error;

        pFormat = PictureMatchFormat(pScreen, 32, PICT_a8r8g8b8);
        if (!pFormat) {
            free(argbbits);
            free(srcbits);
            free(mskbits);
            return (BadImplementation);
        }
        pPixmap = (*pScreen->CreatePixmap) (pScreen, width, height, 32);
        if (!pPixmap) {
            free(argbbits);
            free(srcbits);
            free(mskbits);
            return (BadAlloc);
        }
        pPicture = CreatePicture(0, &pPixmap->drawable, pFormat, 0, 0,
                                 client, &error);
        if (!pPicture) {
            free(argbbits);
            free(srcbits);
            free(mskbits);
            return error;
        }
        (*pScreen->DestroyPixmap) (pPixmap);
        CompositePicture(PictOpSrc,
                         pSrc, 0, pPicture, 0, 0, 0, 0, 0, 0, width, height);
        (*pScreen->GetImage) (pPicture->pDrawable,
                              0, 0, width, height, ZPixmap,
                              0xffffffff, (pointer) argbbits);
        FreePicture(pPicture, 0);
    }
    /*
     * Check whether the cursor can be directly supported by
     * the core cursor code
     */
    ncolor = 0;
    argb = argbbits;
    for (y = 0; ncolor <= 2 && y < height; y++) {
        for (x = 0; ncolor <= 2 && x < width; x++) {
            CARD32 p = *argb++;

            CARD32 a = (p >> 24);

            if (a == 0)         /* transparent */
                continue;
            if (a == 0xff) {    /* opaque */
                int n;

                for (n = 0; n < ncolor; n++)
                    if (p == twocolor[n])
                        break;
                if (n == ncolor)
                    twocolor[ncolor++] = p;
            }
            else
                ncolor = 3;
        }
    }

    /*
     * Convert argb image to two plane cursor
     */
    srcline = srcbits;
    mskline = mskbits;
    argb = argbbits;
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            CARD32 p = *argb++;

            if (ncolor <= 2) {
                CARD32 a = ((p >> 24));

                SetBit(mskline, x, a != 0);
                SetBit(srcline, x, a != 0 && p == twocolor[0]);
            }
            else {
                CARD32 a = ((p >> 24) * DITHER_SIZE + 127) / 255;

                CARD32 i = ((CvtR8G8B8toY15(p) >> 7) * DITHER_SIZE + 127) / 255;

                CARD32 d =
                    orderedDither[y & (DITHER_DIM - 1)][x & (DITHER_DIM - 1)];
                /* Set mask from dithered alpha value */
                SetBit(mskline, x, a > d);
                /* Set src from dithered intensity value */
                SetBit(srcline, x, a > d && i <= d);
            }
        }
        srcline += stride;
        mskline += stride;
    }
    /*
     * Dither to white and black if the cursor has more than two colors
     */
    if (ncolor > 2) {
        twocolor[0] = 0xff000000;
        twocolor[1] = 0xffffffff;
    }
    else {
        free(argbbits);
        argbbits = 0;
    }

#define GetByte(p,s)	(((p) >> (s)) & 0xff)
#define GetColor(p,s)	(GetByte(p,s) | (GetByte(p,s) << 8))

    cm.width = width;
    cm.height = height;
    cm.xhot = stuff->x;
    cm.yhot = stuff->y;
    pCursor = AllocCursorARGB(srcbits, mskbits, argbbits, &cm,
                              GetColor(twocolor[0], 16),
                              GetColor(twocolor[0], 8),
                              GetColor(twocolor[0], 0),
                              GetColor(twocolor[1], 16),
                              GetColor(twocolor[1], 8),
                              GetColor(twocolor[1], 0));
    if (pCursor && AddResource(stuff->cid, RT_CURSOR, (pointer) pCursor))
        return (client->noClientException);
    return BadAlloc;
}

static int
ProcRenderSetPictureTransform(ClientPtr client)
{
    REQUEST(xRenderSetPictureTransformReq);
    PicturePtr pPicture;

    int result;

    REQUEST_SIZE_MATCH(xRenderSetPictureTransformReq);
    VERIFY_PICTURE(pPicture, stuff->picture, client, SecurityWriteAccess,
                   RenderErrBase + BadPicture);
    result =
        SetPictureTransform(pPicture, (PictTransform *) & stuff->transform);
    if (client->noClientException != Success)
        return (client->noClientException);
    else
        return (result);
}

static int
ProcRenderQueryFilters(ClientPtr client)
{
    REQUEST(xRenderQueryFiltersReq);
    DrawablePtr pDrawable;

    xRenderQueryFiltersReply *reply;

    int nbytesName;

    int nnames;

    ScreenPtr pScreen;

    PictureScreenPtr ps;

    int i, j;

    int len;

    int total_bytes;

    INT16 *aliases;

    char *names;

    REQUEST_SIZE_MATCH(xRenderQueryFiltersReq);
    SECURITY_VERIFY_DRAWABLE(pDrawable, stuff->drawable, client,
                             SecurityReadAccess);

    pScreen = pDrawable->pScreen;
    nbytesName = 0;
    nnames = 0;
    ps = GetPictureScreenIfSet(pScreen);
    if (ps) {
        for (i = 0; i < ps->nfilters; i++)
            nbytesName += 1 + strlen(ps->filters[i].name);
        for (i = 0; i < ps->nfilterAliases; i++)
            nbytesName += 1 + strlen(ps->filterAliases[i].alias);
        nnames = ps->nfilters + ps->nfilterAliases;
    }
    len = ((nnames + 1) >> 1) + ((nbytesName + 3) >> 2);
    total_bytes = sizeof(xRenderQueryFiltersReply) + (len << 2);
    reply = malloc(total_bytes);
    if (!reply)
        return BadAlloc;
    aliases = (INT16 *) (reply + 1);
    names = (char *) (aliases + ((nnames + 1) & ~1));

    reply->type = X_Reply;
    reply->sequenceNumber = client->sequence;
    reply->length = len;
    reply->numAliases = nnames;
    reply->numFilters = nnames;
    if (ps) {

        /* fill in alias values */
        for (i = 0; i < ps->nfilters; i++)
            aliases[i] = FilterAliasNone;
        for (i = 0; i < ps->nfilterAliases; i++) {
            for (j = 0; j < ps->nfilters; j++)
                if (ps->filterAliases[i].filter_id == ps->filters[j].id)
                    break;
            if (j == ps->nfilters) {
                for (j = 0; j < ps->nfilterAliases; j++)
                    if (ps->filterAliases[i].filter_id ==
                        ps->filterAliases[j].alias_id) {
                        break;
                    }
                if (j == ps->nfilterAliases)
                    j = FilterAliasNone;
                else
                    j = j + ps->nfilters;
            }
            aliases[i + ps->nfilters] = j;
        }

        /* fill in filter names */
        for (i = 0; i < ps->nfilters; i++) {
            j = strlen(ps->filters[i].name);
            *names++ = j;
            strncpy(names, ps->filters[i].name, j);
            names += j;
        }

        /* fill in filter alias names */
        for (i = 0; i < ps->nfilterAliases; i++) {
            j = strlen(ps->filterAliases[i].alias);
            *names++ = j;
            strncpy(names, ps->filterAliases[i].alias, j);
            names += j;
        }
    }

    if (client->swapped) {

        for (i = 0; i < reply->numAliases; i++) {
            swaps(&aliases[i]);
        }
        swaps(&reply->sequenceNumber);
        swapl(&reply->length);
        swapl(&reply->numAliases);
        swapl(&reply->numFilters);
    }
    WriteToClient(client, total_bytes, (char *) reply);
    free(reply);

    return (client->noClientException);
}

static int
ProcRenderSetPictureFilter(ClientPtr client)
{
    REQUEST(xRenderSetPictureFilterReq);
    PicturePtr pPicture;

    int result;

    xFixed *params;

    int nparams;

    char *name;

    REQUEST_AT_LEAST_SIZE(xRenderSetPictureFilterReq);
    VERIFY_PICTURE(pPicture, stuff->picture, client, SecurityWriteAccess,
                   RenderErrBase + BadPicture);
    name = (char *) (stuff + 1);
    params = (xFixed *) (name + ((stuff->nbytes + 3) & ~3));
    nparams = ((xFixed *) stuff + client->req_len) - params;
    result = SetPictureFilter(pPicture, name, stuff->nbytes, params, nparams);
    return result;
}

static int
ProcRenderCreateAnimCursor(ClientPtr client)
{
    REQUEST(xRenderCreateAnimCursorReq);
    CursorPtr *cursors;

    CARD32 *deltas;

    CursorPtr pCursor;

    int ncursor;

    xAnimCursorElt *elt;

    int i;

    int ret;

    REQUEST_AT_LEAST_SIZE(xRenderCreateAnimCursorReq);
    LEGAL_NEW_RESOURCE(stuff->cid, client);
    if (client->req_len & 1)
        return BadLength;
    ncursor =
        (client->req_len - (SIZEOF(xRenderCreateAnimCursorReq) >> 2)) >> 1;
    cursors = malloc(ncursor * (sizeof(CursorPtr) + sizeof(CARD32)));
    if (!cursors)
        return BadAlloc;
    deltas = (CARD32 *) (cursors + ncursor);
    elt = (xAnimCursorElt *) (stuff + 1);
    for (i = 0; i < ncursor; i++) {
        cursors[i] = (CursorPtr) SecurityLookupIDByType(client, elt->cursor,
                                                        RT_CURSOR,
                                                        SecurityReadAccess);
        if (!cursors[i]) {
            free(cursors);
            client->errorValue = elt->cursor;
            return BadCursor;
        }
        deltas[i] = elt->delay;
        elt++;
    }
    ret = AnimCursorCreate(cursors, deltas, ncursor, &pCursor);
    free(cursors);
    if (ret != Success)
        return ret;

    if (AddResource(stuff->cid, RT_CURSOR, (pointer) pCursor))
        return client->noClientException;
    return BadAlloc;
}

static int
ProcRenderAddTraps(ClientPtr client)
{
    int ntraps;

    PicturePtr pPicture;

    REQUEST(xRenderAddTrapsReq);

    REQUEST_AT_LEAST_SIZE(xRenderAddTrapsReq);
    VERIFY_PICTURE(pPicture, stuff->picture, client, SecurityWriteAccess,
                   RenderErrBase + BadPicture);
    if (!pPicture->pDrawable)
        return BadDrawable;
    ntraps = (client->req_len << 2) - sizeof(xRenderAddTrapsReq);
    if (ntraps % sizeof(xTrap))
        return BadLength;
    ntraps /= sizeof(xTrap);
    if (ntraps)
        AddTraps(pPicture,
                 stuff->xOff, stuff->yOff, ntraps, (xTrap *) & stuff[1]);
    return client->noClientException;
}

static int
ProcRenderCreateSolidFill(ClientPtr client)
{
    PicturePtr pPicture;

    int error = 0;

    REQUEST(xRenderCreateSolidFillReq);

    REQUEST_AT_LEAST_SIZE(xRenderCreateSolidFillReq);

    LEGAL_NEW_RESOURCE(stuff->pid, client);

    pPicture = CreateSolidPicture(stuff->pid, &stuff->color, &error);
    if (!pPicture)
        return error;
    if (!AddResource(stuff->pid, PictureType, (pointer) pPicture))
        return BadAlloc;
    return Success;
}

static int
ProcRenderCreateLinearGradient(ClientPtr client)
{
    PicturePtr pPicture;

    int len;

    int error = 0;

    xFixed *stops;

    xRenderColor *colors;

    REQUEST(xRenderCreateLinearGradientReq);

    REQUEST_AT_LEAST_SIZE(xRenderCreateLinearGradientReq);

    LEGAL_NEW_RESOURCE(stuff->pid, client);

    len = (client->req_len << 2) - sizeof(xRenderCreateLinearGradientReq);
    if (len != stuff->nStops * (sizeof(xFixed) + sizeof(xRenderColor)))
        return BadLength;

    stops = (xFixed *) (stuff + 1);
    colors = (xRenderColor *) (stops + stuff->nStops);

    pPicture = CreateLinearGradientPicture(stuff->pid, &stuff->p1, &stuff->p2,
                                           stuff->nStops, stops, colors,
                                           &error);
    if (!pPicture)
        return error;
    if (!AddResource(stuff->pid, PictureType, (pointer) pPicture))
        return BadAlloc;
    return Success;
}

static int
ProcRenderCreateRadialGradient(ClientPtr client)
{
    PicturePtr pPicture;

    int len;

    int error = 0;

    xFixed *stops;

    xRenderColor *colors;

    REQUEST(xRenderCreateRadialGradientReq);

    REQUEST_AT_LEAST_SIZE(xRenderCreateRadialGradientReq);

    LEGAL_NEW_RESOURCE(stuff->pid, client);

    len = (client->req_len << 2) - sizeof(xRenderCreateRadialGradientReq);
    if (len != stuff->nStops * (sizeof(xFixed) + sizeof(xRenderColor)))
        return BadLength;

    stops = (xFixed *) (stuff + 1);
    colors = (xRenderColor *) (stops + stuff->nStops);

    pPicture =
        CreateRadialGradientPicture(stuff->pid, &stuff->inner, &stuff->outer,
                                    stuff->inner_radius, stuff->outer_radius,
                                    stuff->nStops, stops, colors, &error);
    if (!pPicture)
        return error;
    if (!AddResource(stuff->pid, PictureType, (pointer) pPicture))
        return BadAlloc;
    return Success;
}

static int
ProcRenderCreateConicalGradient(ClientPtr client)
{
    PicturePtr pPicture;

    int len;

    int error = 0;

    xFixed *stops;

    xRenderColor *colors;

    REQUEST(xRenderCreateConicalGradientReq);

    REQUEST_AT_LEAST_SIZE(xRenderCreateConicalGradientReq);

    LEGAL_NEW_RESOURCE(stuff->pid, client);

    len = (client->req_len << 2) - sizeof(xRenderCreateConicalGradientReq);
    if (len != stuff->nStops * (sizeof(xFixed) + sizeof(xRenderColor)))
        return BadLength;

    stops = (xFixed *) (stuff + 1);
    colors = (xRenderColor *) (stops + stuff->nStops);

    pPicture =
        CreateConicalGradientPicture(stuff->pid, &stuff->center, stuff->angle,
                                     stuff->nStops, stops, colors, &error);
    if (!pPicture)
        return error;
    if (!AddResource(stuff->pid, PictureType, (pointer) pPicture))
        return BadAlloc;
    return Success;
}

static int
ProcRenderDispatch(ClientPtr client)
{
    REQUEST(xReq);

    if (stuff->data < RenderNumberRequests)
        return (*ProcRenderVector[stuff->data]) (client);
    else
        return BadRequest;
}

static int
SProcRenderQueryVersion(ClientPtr client)
{

    REQUEST(xRenderQueryVersionReq);

    swaps(&stuff->length);
    swapl(&stuff->majorVersion);
    swapl(&stuff->minorVersion);
    return (*ProcRenderVector[stuff->renderReqType]) (client);
}

static int
SProcRenderQueryPictFormats(ClientPtr client)
{

    REQUEST(xRenderQueryPictFormatsReq);
    swaps(&stuff->length);
    return (*ProcRenderVector[stuff->renderReqType]) (client);
}

static int
SProcRenderQueryPictIndexValues(ClientPtr client)
{

    REQUEST(xRenderQueryPictIndexValuesReq);
    swaps(&stuff->length);
    swapl(&stuff->format);
    return (*ProcRenderVector[stuff->renderReqType]) (client);
}

static int
SProcRenderQueryDithers(ClientPtr client)
{
    return BadImplementation;
}

static int
SProcRenderCreatePicture(ClientPtr client)
{

    REQUEST(xRenderCreatePictureReq);
    swaps(&stuff->length);
    swapl(&stuff->pid);
    swapl(&stuff->drawable);
    swapl(&stuff->format);
    swapl(&stuff->mask);
    SwapRestL(stuff);
    return (*ProcRenderVector[stuff->renderReqType]) (client);
}

static int
SProcRenderChangePicture(ClientPtr client)
{

    REQUEST(xRenderChangePictureReq);
    swaps(&stuff->length);
    swapl(&stuff->picture);
    swapl(&stuff->mask);
    SwapRestL(stuff);
    return (*ProcRenderVector[stuff->renderReqType]) (client);
}

static int
SProcRenderSetPictureClipRectangles(ClientPtr client)
{

    REQUEST(xRenderSetPictureClipRectanglesReq);
    swaps(&stuff->length);
    swapl(&stuff->picture);
    SwapRestS(stuff);
    return (*ProcRenderVector[stuff->renderReqType]) (client);
}

static int
SProcRenderFreePicture(ClientPtr client)
{

    REQUEST(xRenderFreePictureReq);
    swaps(&stuff->length);
    swapl(&stuff->picture);
    return (*ProcRenderVector[stuff->renderReqType]) (client);
}

static int
SProcRenderComposite(ClientPtr client)
{

    REQUEST(xRenderCompositeReq);
    swaps(&stuff->length);
    swapl(&stuff->src);
    swapl(&stuff->mask);
    swapl(&stuff->dst);
    swaps(&stuff->xSrc);
    swaps(&stuff->ySrc);
    swaps(&stuff->xMask);
    swaps(&stuff->yMask);
    swaps(&stuff->xDst);
    swaps(&stuff->yDst);
    swaps(&stuff->width);
    swaps(&stuff->height);
    return (*ProcRenderVector[stuff->renderReqType]) (client);
}

static int
SProcRenderScale(ClientPtr client)
{

    REQUEST(xRenderScaleReq);
    swaps(&stuff->length);
    swapl(&stuff->src);
    swapl(&stuff->dst);
    swapl(&stuff->colorScale);
    swapl(&stuff->alphaScale);
    swaps(&stuff->xSrc);
    swaps(&stuff->ySrc);
    swaps(&stuff->xDst);
    swaps(&stuff->yDst);
    swaps(&stuff->width);
    swaps(&stuff->height);
    return (*ProcRenderVector[stuff->renderReqType]) (client);
}

static int
SProcRenderTrapezoids(ClientPtr client)
{

    REQUEST(xRenderTrapezoidsReq);

    REQUEST_AT_LEAST_SIZE(xRenderTrapezoidsReq);
    swaps(&stuff->length);
    swapl(&stuff->src);
    swapl(&stuff->dst);
    swapl(&stuff->maskFormat);
    swaps(&stuff->xSrc);
    swaps(&stuff->ySrc);
    SwapRestL(stuff);
    return (*ProcRenderVector[stuff->renderReqType]) (client);
}

static int
SProcRenderTriangles(ClientPtr client)
{

    REQUEST(xRenderTrianglesReq);

    REQUEST_AT_LEAST_SIZE(xRenderTrianglesReq);
    swaps(&stuff->length);
    swapl(&stuff->src);
    swapl(&stuff->dst);
    swapl(&stuff->maskFormat);
    swaps(&stuff->xSrc);
    swaps(&stuff->ySrc);
    SwapRestL(stuff);
    return (*ProcRenderVector[stuff->renderReqType]) (client);
}

static int
SProcRenderTriStrip(ClientPtr client)
{

    REQUEST(xRenderTriStripReq);

    REQUEST_AT_LEAST_SIZE(xRenderTriStripReq);
    swaps(&stuff->length);
    swapl(&stuff->src);
    swapl(&stuff->dst);
    swapl(&stuff->maskFormat);
    swaps(&stuff->xSrc);
    swaps(&stuff->ySrc);
    SwapRestL(stuff);
    return (*ProcRenderVector[stuff->renderReqType]) (client);
}

static int
SProcRenderTriFan(ClientPtr client)
{

    REQUEST(xRenderTriFanReq);

    REQUEST_AT_LEAST_SIZE(xRenderTriFanReq);
    swaps(&stuff->length);
    swapl(&stuff->src);
    swapl(&stuff->dst);
    swapl(&stuff->maskFormat);
    swaps(&stuff->xSrc);
    swaps(&stuff->ySrc);
    SwapRestL(stuff);
    return (*ProcRenderVector[stuff->renderReqType]) (client);
}

static int
SProcRenderColorTrapezoids(ClientPtr client)
{
    return BadImplementation;
}

static int
SProcRenderColorTriangles(ClientPtr client)
{
    return BadImplementation;
}

static int
SProcRenderTransform(ClientPtr client)
{
    return BadImplementation;
}

static int
SProcRenderCreateGlyphSet(ClientPtr client)
{

    REQUEST(xRenderCreateGlyphSetReq);
    swaps(&stuff->length);
    swapl(&stuff->gsid);
    swapl(&stuff->format);
    return (*ProcRenderVector[stuff->renderReqType]) (client);
}

static int
SProcRenderReferenceGlyphSet(ClientPtr client)
{

    REQUEST(xRenderReferenceGlyphSetReq);
    swaps(&stuff->length);
    swapl(&stuff->gsid);
    swapl(&stuff->existing);
    return (*ProcRenderVector[stuff->renderReqType]) (client);
}

static int
SProcRenderFreeGlyphSet(ClientPtr client)
{

    REQUEST(xRenderFreeGlyphSetReq);
    swaps(&stuff->length);
    swapl(&stuff->glyphset);
    return (*ProcRenderVector[stuff->renderReqType]) (client);
}

static int
SProcRenderAddGlyphs(ClientPtr client)
{

    int i;

    CARD32 *gids;

    void *end;

    xGlyphInfo *gi;

    REQUEST(xRenderAddGlyphsReq);
    swaps(&stuff->length);
    swapl(&stuff->glyphset);
    swapl(&stuff->nglyphs);
    if (stuff->nglyphs & 0xe0000000)
        return BadLength;
    end = (CARD8 *) stuff + (client->req_len << 2);
    gids = (CARD32 *) (stuff + 1);
    gi = (xGlyphInfo *) (gids + stuff->nglyphs);
    if ((char *) end - (char *) (gids + stuff->nglyphs) < 0)
        return BadLength;
    if ((char *) end - (char *) (gi + stuff->nglyphs) < 0)
        return BadLength;
    for (i = 0; i < stuff->nglyphs; i++) {
        swapl(&gids[i]);
        swaps(&gi[i].width);
        swaps(&gi[i].height);
        swaps(&gi[i].x);
        swaps(&gi[i].y);
        swaps(&gi[i].xOff);
        swaps(&gi[i].yOff);
    }
    return (*ProcRenderVector[stuff->renderReqType]) (client);
}

static int
SProcRenderAddGlyphsFromPicture(ClientPtr client)
{
    return BadImplementation;
}

static int
SProcRenderFreeGlyphs(ClientPtr client)
{

    REQUEST(xRenderFreeGlyphsReq);
    swaps(&stuff->length);
    swapl(&stuff->glyphset);
    SwapRestL(stuff);
    return (*ProcRenderVector[stuff->renderReqType]) (client);
}

static int
SProcRenderCompositeGlyphs(ClientPtr client)
{

    xGlyphElt *elt;

    CARD8 *buffer;

    CARD8 *end;

    int space;

    int i;

    int size;

    REQUEST(xRenderCompositeGlyphsReq);

    switch (stuff->renderReqType) {
    default:
        size = 1;
        break;
    case X_RenderCompositeGlyphs16:
        size = 2;
        break;
    case X_RenderCompositeGlyphs32:
        size = 4;
        break;
    }

    swaps(&stuff->length);
    swapl(&stuff->src);
    swapl(&stuff->dst);
    swapl(&stuff->maskFormat);
    swapl(&stuff->glyphset);
    swaps(&stuff->xSrc);
    swaps(&stuff->ySrc);
    buffer = (CARD8 *) (stuff + 1);
    end = (CARD8 *) stuff + (client->req_len << 2);
    while (buffer + sizeof(xGlyphElt) < end) {
        elt = (xGlyphElt *) buffer;
        buffer += sizeof(xGlyphElt);

        swaps(&elt->deltax);
        swaps(&elt->deltay);

        i = elt->len;
        if (i == 0xff) {
            swapl((int *) buffer);
            buffer += 4;
        }
        else {
            space = size * i;
            switch (size) {
            case 1:
                buffer += i;
                break;
            case 2:
                while (i--) {
                    swaps((short *) buffer);
                    buffer += 2;
                }
                break;
            case 4:
                while (i--) {
                    swapl((int *) buffer);
                    buffer += 4;
                }
                break;
            }
            if (space & 3)
                buffer += 4 - (space & 3);
        }
    }
    return (*ProcRenderVector[stuff->renderReqType]) (client);
}

static int
SProcRenderFillRectangles(ClientPtr client)
{

    REQUEST(xRenderFillRectanglesReq);

    REQUEST_AT_LEAST_SIZE(xRenderFillRectanglesReq);
    swaps(&stuff->length);
    swapl(&stuff->dst);
    swaps(&stuff->color.red);
    swaps(&stuff->color.green);
    swaps(&stuff->color.blue);
    swaps(&stuff->color.alpha);
    SwapRestS(stuff);
    return (*ProcRenderVector[stuff->renderReqType]) (client);
}

static int
SProcRenderCreateCursor(ClientPtr client)
{

    REQUEST(xRenderCreateCursorReq);
    REQUEST_SIZE_MATCH(xRenderCreateCursorReq);

    swaps(&stuff->length);
    swapl(&stuff->cid);
    swapl(&stuff->src);
    swaps(&stuff->x);
    swaps(&stuff->y);
    return (*ProcRenderVector[stuff->renderReqType]) (client);
}

static int
SProcRenderSetPictureTransform(ClientPtr client)
{

    REQUEST(xRenderSetPictureTransformReq);
    REQUEST_SIZE_MATCH(xRenderSetPictureTransformReq);

    swaps(&stuff->length);
    swapl(&stuff->picture);
    swapl(&stuff->transform.matrix11);
    swapl(&stuff->transform.matrix12);
    swapl(&stuff->transform.matrix13);
    swapl(&stuff->transform.matrix21);
    swapl(&stuff->transform.matrix22);
    swapl(&stuff->transform.matrix23);
    swapl(&stuff->transform.matrix31);
    swapl(&stuff->transform.matrix32);
    swapl(&stuff->transform.matrix33);
    return (*ProcRenderVector[stuff->renderReqType]) (client);
}

static int
SProcRenderQueryFilters(ClientPtr client)
{

    REQUEST(xRenderQueryFiltersReq);
    REQUEST_SIZE_MATCH(xRenderQueryFiltersReq);

    swaps(&stuff->length);
    swapl(&stuff->drawable);
    return (*ProcRenderVector[stuff->renderReqType]) (client);
}

static int
SProcRenderSetPictureFilter(ClientPtr client)
{

    REQUEST(xRenderSetPictureFilterReq);
    REQUEST_AT_LEAST_SIZE(xRenderSetPictureFilterReq);

    swaps(&stuff->length);
    swapl(&stuff->picture);
    swaps(&stuff->nbytes);
    return (*ProcRenderVector[stuff->renderReqType]) (client);
}

static int
SProcRenderCreateAnimCursor(ClientPtr client)
{

    REQUEST(xRenderCreateAnimCursorReq);
    REQUEST_AT_LEAST_SIZE(xRenderCreateAnimCursorReq);

    swaps(&stuff->length);
    swapl(&stuff->cid);
    SwapRestL(stuff);
    return (*ProcRenderVector[stuff->renderReqType]) (client);
}

static int
SProcRenderAddTraps(ClientPtr client)
{

    REQUEST(xRenderAddTrapsReq);
    REQUEST_AT_LEAST_SIZE(xRenderAddTrapsReq);

    swaps(&stuff->length);
    swapl(&stuff->picture);
    swaps(&stuff->xOff);
    swaps(&stuff->yOff);
    SwapRestL(stuff);
    return (*ProcRenderVector[stuff->renderReqType]) (client);
}

static int
SProcRenderCreateSolidFill(ClientPtr client)
{

    REQUEST(xRenderCreateSolidFillReq);
    REQUEST_AT_LEAST_SIZE(xRenderCreateSolidFillReq);

    swaps(&stuff->length);
    swapl(&stuff->pid);
    swaps(&stuff->color.alpha);
    swaps(&stuff->color.red);
    swaps(&stuff->color.green);
    swaps(&stuff->color.blue);
    return (*ProcRenderVector[stuff->renderReqType]) (client);
}

static void
swapStops(void *stuff, int n)
{
    int i;

    CARD32 *stops;

    CARD16 *colors;

    stops = (CARD32 *) (stuff);
    for (i = 0; i < n; ++i) {
        swapl(stops);
        ++stops;
    }
    colors = (CARD16 *) (stops);
    for (i = 0; i < 4 * n; ++i) {
        swaps(colors);
        ++colors;
    }
}

static int
SProcRenderCreateLinearGradient(ClientPtr client)
{

    int len;

    REQUEST(xRenderCreateLinearGradientReq);
    REQUEST_AT_LEAST_SIZE(xRenderCreateLinearGradientReq);

    swaps(&stuff->length);
    swapl(&stuff->pid);
    swapl(&stuff->p1.x);
    swapl(&stuff->p1.y);
    swapl(&stuff->p2.x);
    swapl(&stuff->p2.y);
    swapl(&stuff->nStops);

    len = (client->req_len << 2) - sizeof(xRenderCreateLinearGradientReq);
    if (len != stuff->nStops * (sizeof(xFixed) + sizeof(xRenderColor)))
        return BadLength;

    swapStops(stuff + 1, stuff->nStops);

    return (*ProcRenderVector[stuff->renderReqType]) (client);
}

static int
SProcRenderCreateRadialGradient(ClientPtr client)
{

    int len;

    REQUEST(xRenderCreateRadialGradientReq);
    REQUEST_AT_LEAST_SIZE(xRenderCreateRadialGradientReq);

    swaps(&stuff->length);
    swapl(&stuff->pid);
    swapl(&stuff->inner.x);
    swapl(&stuff->inner.y);
    swapl(&stuff->outer.x);
    swapl(&stuff->outer.y);
    swapl(&stuff->inner_radius);
    swapl(&stuff->outer_radius);
    swapl(&stuff->nStops);

    len = (client->req_len << 2) - sizeof(xRenderCreateRadialGradientReq);
    if (len != stuff->nStops * (sizeof(xFixed) + sizeof(xRenderColor)))
        return BadLength;

    swapStops(stuff + 1, stuff->nStops);

    return (*ProcRenderVector[stuff->renderReqType]) (client);
}

static int
SProcRenderCreateConicalGradient(ClientPtr client)
{

    int len;

    REQUEST(xRenderCreateConicalGradientReq);
    REQUEST_AT_LEAST_SIZE(xRenderCreateConicalGradientReq);

    swaps(&stuff->length);
    swapl(&stuff->pid);
    swapl(&stuff->center.x);
    swapl(&stuff->center.y);
    swapl(&stuff->angle);
    swapl(&stuff->nStops);

    len = (client->req_len << 2) - sizeof(xRenderCreateConicalGradientReq);
    if (len != stuff->nStops * (sizeof(xFixed) + sizeof(xRenderColor)))
        return BadLength;

    swapStops(stuff + 1, stuff->nStops);

    return (*ProcRenderVector[stuff->renderReqType]) (client);
}

static int
SProcRenderDispatch(ClientPtr client)
{
    REQUEST(xReq);

    if (stuff->data < RenderNumberRequests)
        return (*SProcRenderVector[stuff->data]) (client);
    else
        return BadRequest;
}

