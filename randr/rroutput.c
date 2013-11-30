/*
 * Copyright © 2006 Keith Packard
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation, and
 * that the name of the copyright holders not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  The copyright holders make no representations
 * about the suitability of this software for any purpose.  It is provided "as
 * is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 */

#include "randrstr.h"

RESTYPE RROutputType;

/*
 * Notify the output of some change
 */
void
RROutputChanged(RROutputPtr output, Bool configChanged)
{
    ScreenPtr pScreen = output->pScreen;

    output->changed = TRUE;
    if (pScreen) {
        rrScrPriv(pScreen);
        pScrPriv->changed = TRUE;
        if (configChanged)
            pScrPriv->configChanged = TRUE;
    }
}

/*
 * Create an output
 */

RROutputPtr
RROutputCreate(const char *name, int nameLength, void *devPrivate)
{
    RROutputPtr output;

    if (!RRInit())
        return NULL;
    output = malloc(sizeof(RROutputRec) + nameLength + 1);
    if (!output)
        return NULL;
    output->id = FakeClientID(0);
    output->pScreen = NULL;
    output->name = (char *) (output + 1);
    output->nameLength = nameLength;
    memcpy(output->name, name, nameLength);
    output->name[nameLength] = '\0';
    output->connection = RR_UnknownConnection;
    output->subpixelOrder = SubPixelUnknown;
    output->mmWidth = 0;
    output->mmHeight = 0;
    output->crtc = NULL;
    output->numCrtcs = 0;
    output->crtcs = NULL;
    output->numClones = 0;
    output->clones = NULL;
    output->numModes = 0;
    output->numPreferred = 0;
    output->modes = NULL;
    output->properties = NULL;
    output->changed = FALSE;
    output->devPrivate = devPrivate;

    if (!AddResource(output->id, RROutputType, (pointer) output))
        return NULL;

    return output;
}

/*
 * Attach an Output to a screen. This is done as a separate step
 * so that an xf86-based driver can create Outputs in PreInit
 * before the Screen has been created
 */

Bool
RROutputAttachScreen(RROutputPtr output, ScreenPtr pScreen)
{
    rrScrPriv(pScreen);
    RROutputPtr *outputs;

    if (pScrPriv->numOutputs)
        outputs = realloc(pScrPriv->outputs,
                           (pScrPriv->numOutputs + 1) * sizeof(RROutputPtr));
    else
        outputs = malloc(sizeof(RROutputPtr));
    if (!outputs)
        return FALSE;
    output->pScreen = pScreen;
    pScrPriv->outputs = outputs;
    pScrPriv->outputs[pScrPriv->numOutputs++] = output;
    RROutputChanged(output, FALSE);
    return TRUE;
}

/*
 * Notify extension that output parameters have been changed
 */
Bool
RROutputSetClones(RROutputPtr output, RROutputPtr * clones, int numClones)
{
    RROutputPtr *newClones;

    int i;

    if (numClones == output->numClones) {
        for (i = 0; i < numClones; i++)
            if (output->clones[i] != clones[i])
                break;
        if (i == numClones)
            return TRUE;
    }
    if (numClones) {
        newClones = malloc(numClones * sizeof(RROutputPtr));
        if (!newClones)
            return FALSE;

        memcpy(newClones, clones, numClones * sizeof(RROutputPtr));
    }
    else
        newClones = NULL;
    if (output->clones)
        free(output->clones);
    output->clones = newClones;
    output->numClones = numClones;
    RROutputChanged(output, TRUE);
    return TRUE;
}

Bool
RROutputSetModes(RROutputPtr output,
                 RRModePtr * modes, int numModes, int numPreferred)
{
    RRModePtr *newModes;

    int i;

    if (numModes == output->numModes && numPreferred == output->numPreferred) {
        for (i = 0; i < numModes; i++)
            if (output->modes[i] != modes[i])
                break;
        if (i == numModes) {
            for (i = 0; i < numModes; i++)
                RRModeDestroy(modes[i]);
            return TRUE;
        }
    }

    if (numModes) {
        newModes = malloc(numModes * sizeof(RRModePtr));
        if (!newModes)
            return FALSE;

        memcpy(newModes, modes, numModes * sizeof(RRModePtr));
    }
    else
        newModes = NULL;
    if (output->modes) {
        for (i = 0; i < output->numModes; i++)
            RRModeDestroy(output->modes[i]);
        free(output->modes);
    }
    output->modes = newModes;
    output->numModes = numModes;
    output->numPreferred = numPreferred;
    RROutputChanged(output, TRUE);
    return TRUE;
}

Bool
RROutputSetCrtcs(RROutputPtr output, RRCrtcPtr * crtcs, int numCrtcs)
{
    RRCrtcPtr *newCrtcs;

    int i;

    if (numCrtcs == output->numCrtcs) {
        for (i = 0; i < numCrtcs; i++)
            if (output->crtcs[i] != crtcs[i])
                break;
        if (i == numCrtcs)
            return TRUE;
    }
    if (numCrtcs) {
        newCrtcs = malloc(numCrtcs * sizeof(RRCrtcPtr));
        if (!newCrtcs)
            return FALSE;

        memcpy(newCrtcs, crtcs, numCrtcs * sizeof(RRCrtcPtr));
    }
    else
        newCrtcs = NULL;

    free(output->crtcs);
    output->crtcs = newCrtcs;
    output->numCrtcs = numCrtcs;
    RROutputChanged(output, TRUE);
    return TRUE;
}

void
RROutputSetCrtc(RROutputPtr output, RRCrtcPtr crtc)
{
    if (output->crtc == crtc)
        return;
    output->crtc = crtc;
    RROutputChanged(output, FALSE);
}

Bool
RROutputSetConnection(RROutputPtr output, CARD8 connection)
{
    if (output->connection == connection)
        return TRUE;
    output->connection = connection;
    RROutputChanged(output, TRUE);
    return TRUE;
}

Bool
RROutputSetSubpixelOrder(RROutputPtr output, int subpixelOrder)
{
    if (output->subpixelOrder == subpixelOrder)
        return TRUE;

    output->subpixelOrder = subpixelOrder;
    RROutputChanged(output, FALSE);
    return TRUE;
}

Bool
RROutputSetPhysicalSize(RROutputPtr output, int mmWidth, int mmHeight)
{
    if (output->mmWidth == mmWidth && output->mmHeight == mmHeight)
        return TRUE;
    output->mmWidth = mmWidth;
    output->mmHeight = mmHeight;
    RROutputChanged(output, FALSE);
    return TRUE;
}

void
RRDeliverOutputEvent(ClientPtr client, WindowPtr pWin, RROutputPtr output)
{
    ScreenPtr pScreen = pWin->drawable.pScreen;

    rrScrPriv(pScreen);
    xRROutputChangeNotifyEvent oe;

    RRCrtcPtr crtc = output->crtc;

    RRModePtr mode = crtc ? crtc->mode : 0;

    oe.type = RRNotify + RREventBase;
    oe.subCode = RRNotify_OutputChange;
    oe.sequenceNumber = client->sequence;
    oe.timestamp = pScrPriv->lastSetTime.milliseconds;
    oe.configTimestamp = pScrPriv->lastConfigTime.milliseconds;
    oe.window = pWin->drawable.id;
    oe.output = output->id;
    if (crtc) {
        oe.crtc = crtc->id;
        oe.mode = mode ? mode->mode.id : None;
        oe.rotation = crtc->rotation;
    }
    else {
        oe.crtc = None;
        oe.mode = None;
        oe.rotation = RR_Rotate_0;
    }
    oe.connection = output->connection;
    oe.subpixelOrder = output->subpixelOrder;
    WriteEventsToClient(client, 1, (xEvent *) &oe);
}

/*
 * Destroy a Output at shutdown
 */
void
RROutputDestroy(RROutputPtr crtc)
{
    FreeResource(crtc->id, 0);
}

static int
RROutputDestroyResource(pointer value, XID pid)
{
    RROutputPtr output = (RROutputPtr) value;

    ScreenPtr pScreen = output->pScreen;

    if (pScreen) {
        rrScrPriv(pScreen);
        int i;

        for (i = 0; i < pScrPriv->numOutputs; i++) {
            if (pScrPriv->outputs[i] == output) {
                memmove(pScrPriv->outputs + i, pScrPriv->outputs + i + 1,
                        (pScrPriv->numOutputs - (i + 1)) * sizeof(RROutputPtr));
                --pScrPriv->numOutputs;
                break;
            }
        }
    }
    if (output->modes)
        free(output->modes);
    if (output->crtcs)
        free(output->crtcs);
    if (output->clones)
        free(output->clones);
    RRDeleteAllOutputProperties(output);
    free(output);
    return 1;
}

/*
 * Initialize output type
 */
Bool
RROutputInit(void)
{
    RROutputType = CreateNewResourceType(RROutputDestroyResource);
    if (!RROutputType)
        return FALSE;
#ifdef XResExtension
    RegisterResourceName(RROutputType, "OUTPUT");
#endif
    return TRUE;
}

#define OutputInfoExtra	(SIZEOF(xRRGetOutputInfoReply) - 32)

int
ProcRRGetOutputInfo(ClientPtr client)
{
    REQUEST(xRRGetOutputInfoReq);;
    xRRGetOutputInfoReply rep;

    RROutputPtr output;

    CARD8 *extra;

    unsigned long extraLen;

    ScreenPtr pScreen;

    rrScrPrivPtr pScrPriv;

    RRCrtc *crtcs;

    RRMode *modes;

    RROutput *clones;

    char *name;

    int i;

    REQUEST_SIZE_MATCH(xRRGetOutputInfoReq);
    output = LookupOutput(client, stuff->output, SecurityReadAccess);

    if (!output)
        return RRErrorBase + BadRROutput;

    pScreen = output->pScreen;
    pScrPriv = rrGetScrPriv(pScreen);

    rep.type = X_Reply;
    rep.sequenceNumber = client->sequence;
    rep.length = OutputInfoExtra >> 2;
    rep.timestamp = pScrPriv->lastSetTime.milliseconds;
    rep.crtc = output->crtc ? output->crtc->id : None;
    rep.mmWidth = output->mmWidth;
    rep.mmHeight = output->mmHeight;
    rep.connection = output->connection;
    rep.subpixelOrder = output->subpixelOrder;
    rep.nCrtcs = output->numCrtcs;
    rep.nModes = output->numModes;
    rep.nPreferred = output->numPreferred;
    rep.nClones = output->numClones;
    rep.nameLength = output->nameLength;

    extraLen = ((output->numCrtcs +
                 output->numModes +
                 output->numClones + ((rep.nameLength + 3) >> 2)) << 2);

    if (extraLen) {
        rep.length += extraLen >> 2;
        extra = malloc(extraLen);
        if (!extra)
            return BadAlloc;
    }
    else
        extra = NULL;

    crtcs = (RRCrtc *) extra;
    modes = (RRMode *) (crtcs + output->numCrtcs);
    clones = (RROutput *) (modes + output->numModes);
    name = (char *) (clones + output->numClones);

    for (i = 0; i < output->numCrtcs; i++) {
        crtcs[i] = output->crtcs[i]->id;
        if (client->swapped)
            swapl(&crtcs[i]);
    }
    for (i = 0; i < output->numModes; i++) {
        modes[i] = output->modes[i]->mode.id;
        if (client->swapped)
            swapl(&modes[i]);
    }
    for (i = 0; i < output->numClones; i++) {
        clones[i] = output->clones[i]->id;
        if (client->swapped)
            swapl(&clones[i]);
    }
    memcpy(name, output->name, output->nameLength);
    if (client->swapped) {
        swaps(&rep.sequenceNumber);
        swapl(&rep.length);
        swapl(&rep.timestamp);
        swapl(&rep.crtc);
        swapl(&rep.mmWidth);
        swapl(&rep.mmHeight);
        swaps(&rep.nCrtcs);
        swaps(&rep.nModes);
        swaps(&rep.nClones);
        swaps(&rep.nameLength);
    }
    WriteToClient(client, sizeof(xRRGetOutputInfoReply), (char *) &rep);
    if (extraLen) {
        WriteToClient(client, extraLen, (char *) extra);
        free(extra);
    }

    return client->noClientException;
}
