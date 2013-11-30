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

static int
SProcRRQueryVersion(ClientPtr client)
{

    REQUEST(xRRQueryVersionReq);

    swaps(&stuff->length);
    swapl(&stuff->majorVersion);
    swapl(&stuff->minorVersion);
    return (*ProcRandrVector[stuff->randrReqType]) (client);
}

static int
SProcRRGetScreenInfo(ClientPtr client)
{

    REQUEST(xRRGetScreenInfoReq);

    swaps(&stuff->length);
    swapl(&stuff->window);
    return (*ProcRandrVector[stuff->randrReqType]) (client);
}

static int
SProcRRSetScreenConfig(ClientPtr client)
{

    REQUEST(xRRSetScreenConfigReq);

    if (RRClientKnowsRates(client)) {
        REQUEST_SIZE_MATCH(xRRSetScreenConfigReq);
        swaps(&stuff->rate);
    }
    else {
        REQUEST_SIZE_MATCH(xRR1_0SetScreenConfigReq);
    }

    swaps(&stuff->length);
    swapl(&stuff->drawable);
    swapl(&stuff->timestamp);
    swaps(&stuff->sizeID);
    swaps(&stuff->rotation);
    return (*ProcRandrVector[stuff->randrReqType]) (client);
}

static int
SProcRRSelectInput(ClientPtr client)
{

    REQUEST(xRRSelectInputReq);

    swaps(&stuff->length);
    swapl(&stuff->window);
    swaps(&stuff->enable);
    return (*ProcRandrVector[stuff->randrReqType]) (client);
}

static int
SProcRRGetScreenSizeRange(ClientPtr client)
{
    REQUEST(xRRGetScreenSizeRangeReq);

    REQUEST_SIZE_MATCH(xRRGetScreenSizeRangeReq);
    (void) stuff;
    return BadImplementation;
}

static int
SProcRRSetScreenSize(ClientPtr client)
{
    REQUEST(xRRSetScreenSizeReq);

    REQUEST_SIZE_MATCH(xRRSetScreenSizeReq);
    (void) stuff;
    return BadImplementation;
}

static int
SProcRRGetScreenResources(ClientPtr client)
{
    REQUEST(xRRGetScreenResourcesReq);

    REQUEST_SIZE_MATCH(xRRGetScreenResourcesReq);
    (void) stuff;
    return BadImplementation;
}

static int
SProcRRGetOutputInfo(ClientPtr client)
{
    REQUEST(xRRGetOutputInfoReq);;

    REQUEST_SIZE_MATCH(xRRGetOutputInfoReq);
    (void) stuff;
    return BadImplementation;
}

static int
SProcRRListOutputProperties(ClientPtr client)
{
    REQUEST(xRRListOutputPropertiesReq);

    REQUEST_SIZE_MATCH(xRRListOutputPropertiesReq);
    (void) stuff;
    return BadImplementation;
}

static int
SProcRRQueryOutputProperty(ClientPtr client)
{
    REQUEST(xRRQueryOutputPropertyReq);

    REQUEST_SIZE_MATCH(xRRQueryOutputPropertyReq);
    (void) stuff;
    return BadImplementation;
}

static int
SProcRRConfigureOutputProperty(ClientPtr client)
{
    REQUEST(xRRConfigureOutputPropertyReq);

    REQUEST_SIZE_MATCH(xRRConfigureOutputPropertyReq);
    (void) stuff;
    return BadImplementation;
}

static int
SProcRRChangeOutputProperty(ClientPtr client)
{
    REQUEST(xRRChangeOutputPropertyReq);

    REQUEST_SIZE_MATCH(xRRChangeOutputPropertyReq);
    (void) stuff;
    return BadImplementation;
}

static int
SProcRRDeleteOutputProperty(ClientPtr client)
{
    REQUEST(xRRDeleteOutputPropertyReq);

    REQUEST_SIZE_MATCH(xRRDeleteOutputPropertyReq);
    (void) stuff;
    return BadImplementation;
}

static int
SProcRRGetOutputProperty(ClientPtr client)
{
    REQUEST(xRRGetOutputPropertyReq);

    REQUEST_SIZE_MATCH(xRRGetOutputPropertyReq);
    (void) stuff;
    return BadImplementation;
}

static int
SProcRRCreateMode(ClientPtr client)
{
    REQUEST(xRRCreateModeReq);

    REQUEST_SIZE_MATCH(xRRCreateModeReq);
    (void) stuff;
    return BadImplementation;
}

static int
SProcRRDestroyMode(ClientPtr client)
{
    REQUEST(xRRDestroyModeReq);

    REQUEST_SIZE_MATCH(xRRDestroyModeReq);
    (void) stuff;
    return BadImplementation;
}

static int
SProcRRAddOutputMode(ClientPtr client)
{
    REQUEST(xRRAddOutputModeReq);

    REQUEST_SIZE_MATCH(xRRAddOutputModeReq);
    (void) stuff;
    return BadImplementation;
}

static int
SProcRRDeleteOutputMode(ClientPtr client)
{
    REQUEST(xRRDeleteOutputModeReq);

    REQUEST_SIZE_MATCH(xRRDeleteOutputModeReq);
    (void) stuff;
    return BadImplementation;
}

static int
SProcRRGetCrtcInfo(ClientPtr client)
{
    REQUEST(xRRGetCrtcInfoReq);

    REQUEST_SIZE_MATCH(xRRGetCrtcInfoReq);
    (void) stuff;
    return BadImplementation;
}

static int
SProcRRSetCrtcConfig(ClientPtr client)
{
    REQUEST(xRRSetCrtcConfigReq);

    REQUEST_SIZE_MATCH(xRRSetCrtcConfigReq);
    (void) stuff;
    return BadImplementation;
}

static int
SProcRRGetCrtcGammaSize(ClientPtr client)
{
    REQUEST(xRRGetCrtcGammaSizeReq);

    REQUEST_SIZE_MATCH(xRRGetCrtcGammaSizeReq);
    (void) stuff;
    return BadImplementation;
}

static int
SProcRRGetCrtcGamma(ClientPtr client)
{
    REQUEST(xRRGetCrtcGammaReq);

    REQUEST_SIZE_MATCH(xRRGetCrtcGammaReq);
    (void) stuff;
    return BadImplementation;
}

static int
SProcRRSetCrtcGamma(ClientPtr client)
{
    REQUEST(xRRSetCrtcGammaReq);

    REQUEST_SIZE_MATCH(xRRSetCrtcGammaReq);
    (void) stuff;
    return BadImplementation;
}

int (*SProcRandrVector[RRNumberRequests]) (ClientPtr) = {
    SProcRRQueryVersion,        /* 0 */
/* we skip 1 to make old clients fail pretty immediately */
        NULL,                   /* 1 SProcRandrOldGetScreenInfo */
/* V1.0 apps share the same set screen config request id */
        SProcRRSetScreenConfig, /* 2 */
        NULL,                   /* 3 SProcRandrOldScreenChangeSelectInput */
/* 3 used to be ScreenChangeSelectInput; deprecated */
        SProcRRSelectInput,     /* 4 */
        SProcRRGetScreenInfo,   /* 5 */
/* V1.2 additions */
        SProcRRGetScreenSizeRange,      /* 6 */
        SProcRRSetScreenSize,   /* 7 */
        SProcRRGetScreenResources,      /* 8 */
        SProcRRGetOutputInfo,   /* 9 */
        SProcRRListOutputProperties,    /* 10 */
        SProcRRQueryOutputProperty,     /* 11 */
        SProcRRConfigureOutputProperty, /* 12 */
        SProcRRChangeOutputProperty,    /* 13 */
        SProcRRDeleteOutputProperty,    /* 14 */
        SProcRRGetOutputProperty,       /* 15 */
        SProcRRCreateMode,      /* 16 */
        SProcRRDestroyMode,     /* 17 */
        SProcRRAddOutputMode,   /* 18 */
        SProcRRDeleteOutputMode,        /* 19 */
        SProcRRGetCrtcInfo,     /* 20 */
        SProcRRSetCrtcConfig,   /* 21 */
        SProcRRGetCrtcGammaSize,        /* 22 */
        SProcRRGetCrtcGamma,    /* 23 */
        SProcRRSetCrtcGamma,    /* 24 */
};
