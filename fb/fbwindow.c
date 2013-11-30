/*
 *
 * Copyright © 1998 Keith Packard
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

#ifdef HAVE_DIX_CONFIG_H
#include <dix-config.h>
#endif

#include <stdlib.h>

#include "fb.h"


Bool
fbCreateWindow(WindowPtr pWin)
{
    pWin->devPrivates[fbWinPrivateIndex].ptr =
        (pointer) fbGetScreenPixmap(pWin->drawable.pScreen);
    if (pWin->drawable.bitsPerPixel == 32)
        pWin->drawable.bitsPerPixel =
            fbGetScreenPrivate(pWin->drawable.pScreen)->win32bpp;
    return TRUE;
}

Bool
fbDestroyWindow(WindowPtr pWin)
{
    return TRUE;
}

Bool
fbMapWindow(WindowPtr pWindow)
{
    return TRUE;
}

Bool
fbPositionWindow(WindowPtr pWin, int x, int y)
{
    return TRUE;
}

Bool
fbUnmapWindow(WindowPtr pWindow)
{
    return TRUE;
}

void
fbCopyWindowProc(DrawablePtr pSrcDrawable,
                 DrawablePtr pDstDrawable,
                 GCPtr pGC,
                 BoxPtr pbox,
                 int nbox,
                 int dx,
                 int dy,
                 Bool reverse, Bool upsidedown, Pixel bitplane, void *closure)
{
    FbBits *src;

    FbStride srcStride;

    int srcBpp;

    int srcXoff, srcYoff;

    FbBits *dst;

    FbStride dstStride;

    int dstBpp;

    int dstXoff, dstYoff;

    fbGetDrawable(pSrcDrawable, src, srcStride, srcBpp, srcXoff, srcYoff);
    fbGetDrawable(pDstDrawable, dst, dstStride, dstBpp, dstXoff, dstYoff);

    while (nbox--) {
        fbBlt(src + (pbox->y1 + dy + srcYoff) * srcStride,
              srcStride,
              (pbox->x1 + dx + srcXoff) * srcBpp,
              dst + (pbox->y1 + dstYoff) * dstStride,
              dstStride,
              (pbox->x1 + dstXoff) * dstBpp,
              (pbox->x2 - pbox->x1) * dstBpp,
              (pbox->y2 - pbox->y1),
              GXcopy, FB_ALLONES, dstBpp, reverse, upsidedown);
        pbox++;
    }
}

void
fbCopyWindow(WindowPtr pWin, DDXPointRec ptOldOrg, RegionPtr prgnSrc)
{
    RegionRec rgnDst;

    int dx, dy;

    PixmapPtr pPixmap = fbGetWindowPixmap(pWin);

    DrawablePtr pDrawable = &pPixmap->drawable;

    dx = ptOldOrg.x - pWin->drawable.x;
    dy = ptOldOrg.y - pWin->drawable.y;
    REGION_TRANSLATE(prgnSrc, -dx, -dy);

    REGION_NULL(&rgnDst);

    REGION_INTERSECT(&rgnDst, &pWin->borderClip,
                     prgnSrc);


    fbCopyRegion(pDrawable, pDrawable,
                 0, &rgnDst, dx, dy, fbCopyWindowProc, 0, 0);

    REGION_UNINIT(&rgnDst);
    fbValidateDrawable(&pWin->drawable);
}

Bool
fbChangeWindowAttributes(WindowPtr pWin, unsigned long mask)
{
    PixmapPtr pPixmap;

    if (mask & CWBackPixmap) {
        if (pWin->backgroundState == BackgroundPixmap) {
            pPixmap = pWin->background.pixmap;
            if (pPixmap->drawable.bitsPerPixel != pWin->drawable.bitsPerPixel) {
                pPixmap = fb24_32ReformatTile(pPixmap,
                                              pWin->drawable.bitsPerPixel);
                if (pPixmap) {
                    (*pWin->drawable.pScreen->DestroyPixmap) (pWin->background.
                                                              pixmap);
                    pWin->background.pixmap = pPixmap;
                }
            }
            if (FbEvenTile(pPixmap->drawable.width *
                           pPixmap->drawable.bitsPerPixel))
                fbPadPixmap(pPixmap);
        }
    }
    if (mask & CWBorderPixmap) {
        if (pWin->borderIsPixel == FALSE) {
            pPixmap = pWin->border.pixmap;
            if (pPixmap->drawable.bitsPerPixel != pWin->drawable.bitsPerPixel) {
                pPixmap = fb24_32ReformatTile(pPixmap,
                                              pWin->drawable.bitsPerPixel);
                if (pPixmap) {
                    (*pWin->drawable.pScreen->DestroyPixmap) (pWin->border.
                                                              pixmap);
                    pWin->border.pixmap = pPixmap;
                }
            }
            if (FbEvenTile(pPixmap->drawable.width *
                           pPixmap->drawable.bitsPerPixel))
                fbPadPixmap(pPixmap);
        }
    }
    return TRUE;
}

void
fbFillRegionSolid(DrawablePtr pDrawable,
                  RegionPtr pRegion, FbBits and, FbBits xor)
{
    FbBits *dst;

    FbStride dstStride;

    int dstBpp;

    int dstXoff, dstYoff;

    int n = REGION_NUM_RECTS(pRegion);

    BoxPtr pbox = REGION_RECTS(pRegion);


    fbGetDrawable(pDrawable, dst, dstStride, dstBpp, dstXoff, dstYoff);

    while (n--) {
            fbSolid(dst + (pbox->y1 + dstYoff) * dstStride,
                    dstStride,
                    (pbox->x1 + dstXoff) * dstBpp,
                    dstBpp,
                    (pbox->x2 - pbox->x1) * dstBpp,
                    pbox->y2 - pbox->y1, and, xor);
        fbValidateDrawable(pDrawable);
        pbox++;
    }
}


void
fbFillRegionTiled(DrawablePtr pDrawable, RegionPtr pRegion, PixmapPtr pTile)
{
    FbBits *dst;

    FbStride dstStride;

    int dstBpp;

    int dstXoff, dstYoff;

    FbBits *tile;

    FbStride tileStride;

    int tileBpp;

    int tileXoff _X_UNUSED, tileYoff _X_UNUSED;     /* XXX assumed to be zero */

    int tileWidth, tileHeight;

    int n = REGION_NUM_RECTS(pRegion);

    BoxPtr pbox = REGION_RECTS(pRegion);

    int xRot = pDrawable->x;

    int yRot = pDrawable->y;

    fbGetDrawable(pDrawable, dst, dstStride, dstBpp, dstXoff, dstYoff);
    fbGetDrawable(&pTile->drawable, tile, tileStride, tileBpp, tileXoff,
                  tileYoff);
    tileWidth = pTile->drawable.width;
    tileHeight = pTile->drawable.height;
    xRot += dstXoff;
    yRot += dstYoff;

    while (n--) {
        fbTile(dst + (pbox->y1 + dstYoff) * dstStride,
               dstStride,
               (pbox->x1 + dstXoff) * dstBpp,
               (pbox->x2 - pbox->x1) * dstBpp,
               pbox->y2 - pbox->y1,
               tile,
               tileStride,
               tileWidth * dstBpp,
               tileHeight,
               GXcopy,
               FB_ALLONES, dstBpp, xRot * dstBpp, yRot - (pbox->y1 + dstYoff));
        pbox++;
    }
}

void
fbPaintWindow(WindowPtr pWin, RegionPtr pRegion, int what)
{
    WindowPtr pBgWin;

    switch (what) {
    case PW_BACKGROUND:
        switch (pWin->backgroundState) {
        case None:
            break;
        case ParentRelative:
            do {
                pWin = pWin->parent;
            } while (pWin->backgroundState == ParentRelative);
            (*pWin->drawable.pScreen->PaintWindowBackground) (pWin, pRegion,
                                                              what);
            break;
        case BackgroundPixmap:
            fbFillRegionTiled(&pWin->drawable,
                              pRegion, pWin->background.pixmap);
            break;
        case BackgroundPixel:
            fbFillRegionSolid(&pWin->drawable,
                              pRegion,
                              0,
                              fbReplicatePixel(pWin->background.pixel,
                                               pWin->drawable.bitsPerPixel));
            break;
        }
        break;
    case PW_BORDER:
        if (pWin->borderIsPixel) {
            fbFillRegionSolid(&pWin->drawable,
                              pRegion,
                              0,
                              fbReplicatePixel(pWin->border.pixel,
                                               pWin->drawable.bitsPerPixel));
        }
        else {
            for (pBgWin = pWin;
                 pBgWin->backgroundState == ParentRelative;
                 pBgWin = pBgWin->parent);

            fbFillRegionTiled(&pBgWin->drawable, pRegion, pWin->border.pixmap);
        }
        break;
    }
    fbValidateDrawable(&pWin->drawable);
}
