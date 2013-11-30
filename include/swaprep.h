/************************************************************

Copyright 1996 by Thomas E. Dickey <dickey@clark.net>

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of the above listed
copyright holder(s) not be used in advertising or publicity pertaining
to distribution of the software without specific, written prior
permission.

THE ABOVE LISTED COPYRIGHT HOLDER(S) DISCLAIM ALL WARRANTIES WITH REGARD
TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS, IN NO EVENT SHALL THE ABOVE LISTED COPYRIGHT HOLDER(S) BE
LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

********************************************************/

#ifndef SWAPREP_H
#define SWAPREP_H 1

void Swap32Write(
    ClientPtr /* pClient */,
    int /* size */,
    CARD32 * /* pbuf */);

void CopySwap32Write(
    ClientPtr /* pClient */,
    int /* size */,
    CARD32 * /* pbuf */);

void CopySwap16Write(
    ClientPtr /* pClient */,
    int /* size */,
    short * /* pbuf */);

void SGenericReply(
    ClientPtr /* pClient */,
    int /* size */,
    xGenericReply * /* pRep */);

void SGetWindowAttributesReply(
    ClientPtr /* pClient */,
    int /* size */,
    xGetWindowAttributesReply * /* pRep */);

void SGetGeometryReply(
    ClientPtr /* pClient */,
    int /* size */,
    xGetGeometryReply * /* pRep */);

void SQueryTreeReply(
    ClientPtr /* pClient */,
    int /* size */,
    xQueryTreeReply * /* pRep */);

void SInternAtomReply(
    ClientPtr /* pClient */,
    int /* size */,
    xInternAtomReply * /* pRep */);

void SGetAtomNameReply(
    ClientPtr /* pClient */,
    int /* size */,
    xGetAtomNameReply * /* pRep */);

void SGetPropertyReply(
    ClientPtr /* pClient */,
    int /* size */,
    xGetPropertyReply * /* pRep */);

void SListPropertiesReply(
    ClientPtr /* pClient */,
    int /* size */,
    xListPropertiesReply * /* pRep */);

void SGetSelectionOwnerReply(
    ClientPtr /* pClient */,
    int /* size */,
    xGetSelectionOwnerReply * /* pRep */);

void SQueryPointerReply(
    ClientPtr /* pClient */,
    int /* size */,
    xQueryPointerReply * /* pRep */);

void SwapTimecoord(
    xTimecoord * /* pCoord */);

void SwapTimeCoordWrite(
    ClientPtr /* pClient */,
    int /* size */,
    xTimecoord * /* pRep */);

void SGetMotionEventsReply(
    ClientPtr /* pClient */,
    int /* size */,
    xGetMotionEventsReply * /* pRep */);

void STranslateCoordsReply(
    ClientPtr /* pClient */,
    int /* size */,
    xTranslateCoordsReply * /* pRep */);

void SGetInputFocusReply(
    ClientPtr /* pClient */,
    int /* size */,
    xGetInputFocusReply * /* pRep */);

void SQueryKeymapReply(
    ClientPtr /* pClient */,
    int /* size */,
    xQueryKeymapReply * /* pRep */);

void SQueryFontReply(
    ClientPtr /* pClient */,
    int /* size */,
    xQueryFontReply * /* pRep */);

void SQueryTextExtentsReply(
    ClientPtr /* pClient */,
    int /* size */,
    xQueryTextExtentsReply * /* pRep */);

void SListFontsReply(
    ClientPtr /* pClient */,
    int /* size */,
    xListFontsReply * /* pRep */);

void SListFontsWithInfoReply(
    ClientPtr /* pClient */,
    int /* size */,
    xListFontsWithInfoReply * /* pRep */);

void SGetFontPathReply(
    ClientPtr /* pClient */,
    int /* size */,
    xGetFontPathReply * /* pRep */);

void SGetImageReply(
    ClientPtr /* pClient */,
    int /* size */,
    xGetImageReply * /* pRep */);

void SListInstalledColormapsReply(
    ClientPtr /* pClient */,
    int /* size */,
    xListInstalledColormapsReply * /* pRep */);

void SAllocColorReply(
    ClientPtr /* pClient */,
    int /* size */,
    xAllocColorReply * /* pRep */);

void SAllocNamedColorReply(
    ClientPtr /* pClient */,
    int /* size */,
    xAllocNamedColorReply * /* pRep */);

void SAllocColorCellsReply(
    ClientPtr /* pClient */,
    int /* size */,
    xAllocColorCellsReply * /* pRep */);

void SAllocColorPlanesReply(
    ClientPtr /* pClient */,
    int /* size */,
    xAllocColorPlanesReply * /* pRep */);

void SwapRGB(
    xrgb * /* prgb */);

void SQColorsExtend(
    ClientPtr /* pClient */,
    int /* size */,
    xrgb * /* prgb */);

void SQueryColorsReply(
    ClientPtr /* pClient */,
    int /* size */,
    xQueryColorsReply * /* pRep */);

void SLookupColorReply(
    ClientPtr /* pClient */,
    int /* size */,
    xLookupColorReply * /* pRep */);

void SQueryBestSizeReply(
    ClientPtr /* pClient */,
    int /* size */,
    xQueryBestSizeReply * /* pRep */);

void SListExtensionsReply(
    ClientPtr /* pClient */,
    int /* size */,
    xListExtensionsReply * /* pRep */);

void SGetKeyboardMappingReply(
    ClientPtr /* pClient */,
    int /* size */,
    xGetKeyboardMappingReply * /* pRep */);

void SGetPointerMappingReply(
    ClientPtr /* pClient */,
    int /* size */,
    xGetPointerMappingReply * /* pRep */);

void SGetModifierMappingReply(
    ClientPtr /* pClient */,
    int /* size */,
    xGetModifierMappingReply * /* pRep */);

void SGetKeyboardControlReply(
    ClientPtr /* pClient */,
    int /* size */,
    xGetKeyboardControlReply * /* pRep */);

void SGetPointerControlReply(
    ClientPtr /* pClient */,
    int /* size */,
    xGetPointerControlReply * /* pRep */);

void SGetScreenSaverReply(
    ClientPtr /* pClient */,
    int /* size */,
    xGetScreenSaverReply * /* pRep */);

void SLHostsExtend(
    ClientPtr /* pClient */,
    int /* size */,
    char * /* buf */);

void SListHostsReply(
    ClientPtr /* pClient */,
    int /* size */,
    xListHostsReply * /* pRep */);

void SErrorEvent(
    xError * /* from */,
    xError * /* to */);

void SwapConnSetupInfo(
    char * /* pInfo */,
    char * /* pInfoTBase */);

void WriteSConnectionInfo(
    ClientPtr /* pClient */,
    unsigned long /* size */,
    char * /* pInfo */);

void SwapConnSetup(
    xConnSetup * /* pConnSetup */,
    xConnSetup * /* pConnSetupT */);

void SwapWinRoot(
    xWindowRoot * /* pRoot */,
    xWindowRoot * /* pRootT */);

void SwapVisual(
    xVisualType * /* pVis */,
    xVisualType * /* pVisT */);

void SwapConnSetupPrefix(
    xConnSetupPrefix * /* pcspFrom */,
    xConnSetupPrefix * /* pcspTo */);

void WriteSConnSetupPrefix(
    ClientPtr /* pClient */,
    xConnSetupPrefix * /* pcsp */);

#undef SWAPREP_PROC
#define SWAPREP_PROC(func) void func(xEvent * /* from */, xEvent * /* to */)

SWAPREP_PROC(SCirculateEvent);
SWAPREP_PROC(SClientMessageEvent);
SWAPREP_PROC(SColormapEvent);
SWAPREP_PROC(SConfigureNotifyEvent);
SWAPREP_PROC(SConfigureRequestEvent);
SWAPREP_PROC(SCreateNotifyEvent);
SWAPREP_PROC(SDestroyNotifyEvent);
SWAPREP_PROC(SEnterLeaveEvent);
SWAPREP_PROC(SExposeEvent);
SWAPREP_PROC(SFocusEvent);
SWAPREP_PROC(SGraphicsExposureEvent);
SWAPREP_PROC(SGravityEvent);
SWAPREP_PROC(SKeyButtonPtrEvent);
SWAPREP_PROC(SKeymapNotifyEvent);
SWAPREP_PROC(SMapNotifyEvent);
SWAPREP_PROC(SMapRequestEvent);
SWAPREP_PROC(SMappingEvent);
SWAPREP_PROC(SNoExposureEvent);
SWAPREP_PROC(SPropertyEvent);
SWAPREP_PROC(SReparentEvent);
SWAPREP_PROC(SResizeRequestEvent);
SWAPREP_PROC(SSelectionClearEvent);
SWAPREP_PROC(SSelectionNotifyEvent);
SWAPREP_PROC(SSelectionRequestEvent);
SWAPREP_PROC(SUnmapNotifyEvent);
SWAPREP_PROC(SVisibilityEvent);

#undef SWAPREP_PROC

#endif /* SWAPREP_H */
