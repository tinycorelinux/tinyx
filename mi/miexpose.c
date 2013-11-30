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
/*****************************************************************

Copyright (c) 1991, 1997 Digital Equipment Corporation, Maynard, Massachusetts.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software.

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
DIGITAL EQUIPMENT CORPORATION BE LIABLE FOR ANY CLAIM, DAMAGES, INCLUDING,
BUT NOT LIMITED TO CONSEQUENTIAL OR INCIDENTAL DAMAGES, OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of Digital Equipment Corporation
shall not be used in advertising or otherwise to promote the sale, use or other
dealings in this Software without prior written authorization from Digital
Equipment Corporation.

******************************************************************/


#ifdef HAVE_DIX_CONFIG_H
#include <dix-config.h>
#endif

#include <X11/X.h>
#include <X11/Xproto.h>
#include <X11/Xprotostr.h>

#include "misc.h"
#include "regionstr.h"
#include "scrnintstr.h"
#include "gcstruct.h"
#include "windowstr.h"
#include "pixmap.h"
#include "input.h"

#include "dixstruct.h"
#include "mi.h"
#include <X11/Xmd.h>

#include "globals.h"


/*
    machine-independent graphics exposure code.  any device that uses
the region package can call this.
*/

#ifndef RECTLIMIT
#define RECTLIMIT 25		/* pick a number, any number > 8 */
#endif

/* miHandleExposures
    generate a region for exposures for areas that were copied from obscured or
non-existent areas to non-obscured areas of the destination.  Paint the
background for the region, if the destination is a window.

NOTE:
     this should generally be called, even if graphicsExposures is false,
because this is where bits get recovered from backing store.

NOTE:
     added argument 'plane' is used to indicate how exposures from backing
store should be accomplished. If plane is 0 (i.e. no bit plane), CopyArea
should be used, else a CopyPlane of the indicated plane will be used. The
exposing is done by the backing store's GraphicsExpose function, of course.

*/

_X_EXPORT RegionPtr
miHandleExposures(pSrcDrawable, pDstDrawable,
		  pGC, srcx, srcy, width, height, dstx, dsty, plane)
    DrawablePtr	pSrcDrawable;
    DrawablePtr	pDstDrawable;
    GCPtr 			pGC;
    int 			srcx, srcy;
    int 			width, height;
    int 			dstx, dsty;
    unsigned long		plane;
{
    ScreenPtr pscr;
    RegionPtr prgnSrcClip;	/* drawable-relative source clip */
    RegionRec rgnSrcRec;
    RegionPtr prgnDstClip;	/* drawable-relative dest clip */
    RegionRec rgnDstRec;
    BoxRec srcBox;		/* unclipped source */
    RegionRec rgnExposed;	/* exposed region, calculated source-
				   relative, made dst relative to
				   intersect with visible parts of
				   dest and send events to client,
				   and then screen relative to paint
				   the window background
				*/
    WindowPtr pSrcWin;
    BoxRec expBox;
    Bool extents;

    /* This prevents warning about pscr not being used. */
    pGC->pScreen = pscr = pGC->pScreen;

    /* avoid work if we can */
    if (!pGC->graphicsExposures &&
	(pDstDrawable->type == DRAWABLE_PIXMAP) &&
	((pSrcDrawable->type == DRAWABLE_PIXMAP)))
	return NULL;

    srcBox.x1 = srcx;
    srcBox.y1 = srcy;
    srcBox.x2 = srcx+width;
    srcBox.y2 = srcy+height;

    if (pSrcDrawable->type != DRAWABLE_PIXMAP)
    {
	BoxRec TsrcBox;

	TsrcBox.x1 = srcx + pSrcDrawable->x;
	TsrcBox.y1 = srcy + pSrcDrawable->y;
	TsrcBox.x2 = TsrcBox.x1 + width;
	TsrcBox.y2 = TsrcBox.y1 + height;
	pSrcWin = (WindowPtr) pSrcDrawable;
	if (pGC->subWindowMode == IncludeInferiors)
 	{
	    prgnSrcClip = NotClippedByChildren (pSrcWin);
	    if ((RECT_IN_REGION(prgnSrcClip, &TsrcBox)) == rgnIN)
	    {
		REGION_DESTROY(prgnSrcClip);
		return NULL;
	    }
	}
 	else
 	{
	    if ((RECT_IN_REGION(&pSrcWin->clipList, &TsrcBox)) == rgnIN)
		return NULL;
	    prgnSrcClip = &rgnSrcRec;
	    REGION_NULL(prgnSrcClip);
	    REGION_COPY(prgnSrcClip, &pSrcWin->clipList);
	}
	REGION_TRANSLATE(prgnSrcClip,
				-pSrcDrawable->x, -pSrcDrawable->y);
    }
    else
    {
	BoxRec	box;

	if ((srcBox.x1 >= 0) && (srcBox.y1 >= 0) &&
	    (srcBox.x2 <= pSrcDrawable->width) &&
 	    (srcBox.y2 <= pSrcDrawable->height))
	    return NULL;

	box.x1 = 0;
	box.y1 = 0;
	box.x2 = pSrcDrawable->width;
	box.y2 = pSrcDrawable->height;
	prgnSrcClip = &rgnSrcRec;
	REGION_INIT(prgnSrcClip, &box, 1);
	pSrcWin = (WindowPtr)NULL;
    }

    if (pDstDrawable == pSrcDrawable)
    {
	prgnDstClip = prgnSrcClip;
    }
    else if (pDstDrawable->type != DRAWABLE_PIXMAP)
    {
	if (pGC->subWindowMode == IncludeInferiors)
	{
	    prgnDstClip = NotClippedByChildren((WindowPtr)pDstDrawable);
	}
	else
	{
	    prgnDstClip = &rgnDstRec;
	    REGION_NULL(prgnDstClip);
	    REGION_COPY(prgnDstClip,
				&((WindowPtr)pDstDrawable)->clipList);
	}
	REGION_TRANSLATE(prgnDstClip,
				 -pDstDrawable->x, -pDstDrawable->y);
    }
    else
    {
	BoxRec	box;

	box.x1 = 0;
	box.y1 = 0;
	box.x2 = pDstDrawable->width;
	box.y2 = pDstDrawable->height;
	prgnDstClip = &rgnDstRec;
	REGION_INIT(prgnDstClip, &box, 1);
    }

    /* drawable-relative source region */
    REGION_INIT(&rgnExposed, &srcBox, 1);

    /* now get the hidden parts of the source box*/
    REGION_SUBTRACT(&rgnExposed, &rgnExposed, prgnSrcClip);

    /* move them over the destination */
    REGION_TRANSLATE(&rgnExposed, dstx-srcx, dsty-srcy);

    /* intersect with visible areas of dest */
    REGION_INTERSECT(&rgnExposed, &rgnExposed, prgnDstClip);

    /*
     * If we have LOTS of rectangles, we decide to take the extents
     * and force an exposure on that.  This should require much less
     * work overall, on both client and server.  This is cheating, but
     * isn't prohibited by the protocol ("spontaneous combustion" :-)
     * for windows.
     */
    extents = pGC->graphicsExposures &&
	      (REGION_NUM_RECTS(&rgnExposed) > RECTLIMIT) &&
	      (pDstDrawable->type != DRAWABLE_PIXMAP);
    if (pSrcWin)
    {
	RegionPtr	region;
    	if (!(region = wClipShape (pSrcWin)))
    	    region = wBoundingShape (pSrcWin);
    	/*
     	 * If you try to CopyArea the extents of a shaped window, compacting the
     	 * exposed region will undo all our work!
     	 */
    	if (extents && pSrcWin && region &&
    	    (RECT_IN_REGION(region, &srcBox) != rgnIN))
	    	extents = FALSE;
    }
    if (extents)
    {
	expBox = *REGION_EXTENTS(&rgnExposed);
	REGION_RESET(&rgnExposed, &expBox);
    }
    if ((pDstDrawable->type != DRAWABLE_PIXMAP) &&
	(((WindowPtr)pDstDrawable)->backgroundState != None))
    {
	WindowPtr pWin = (WindowPtr)pDstDrawable;

	/* make the exposed area screen-relative */
	REGION_TRANSLATE(&rgnExposed,
				 pDstDrawable->x, pDstDrawable->y);

	if (extents)
	{
	    /* PaintWindowBackground doesn't clip, so we have to */
	    REGION_INTERSECT(&rgnExposed, &rgnExposed, &pWin->clipList);
	}
	(*pWin->drawable.pScreen->PaintWindowBackground)(
			(WindowPtr)pDstDrawable, &rgnExposed, PW_BACKGROUND);

	if (extents)
	{
	    REGION_RESET(&rgnExposed, &expBox);
	}
	else
	    REGION_TRANSLATE(&rgnExposed,
				     -pDstDrawable->x, -pDstDrawable->y);
    }
    if (prgnDstClip == &rgnDstRec)
    {
	REGION_UNINIT(prgnDstClip);
    }
    else if (prgnDstClip != prgnSrcClip)
    {
	REGION_DESTROY(prgnDstClip);
    }

    if (prgnSrcClip == &rgnSrcRec)
    {
	REGION_UNINIT(prgnSrcClip);
    }
    else
    {
	REGION_DESTROY(prgnSrcClip);
    }

    if (pGC->graphicsExposures)
    {
	/* don't look */
	RegionPtr exposed = REGION_CREATE(NullBox, 0);
	*exposed = rgnExposed;
	return exposed;
    }
    else
    {
	REGION_UNINIT(&rgnExposed);
	return NULL;
    }
}

/* send GraphicsExpose events, or a NoExpose event, based on the region */

_X_EXPORT void
miSendGraphicsExpose (client, pRgn, drawable, major, minor)
    ClientPtr	client;
    RegionPtr	pRgn;
    XID		drawable;
    int	major;
    int	minor;
{
    if (pRgn && !REGION_NIL(pRgn))
    {
        xEvent *pEvent;
	xEvent *pe;
	BoxPtr pBox;
	int i;
	int numRects;

	numRects = REGION_NUM_RECTS(pRgn);
	pBox = REGION_RECTS(pRgn);
	if(!(pEvent = (xEvent *)ALLOCATE_LOCAL(numRects * sizeof(xEvent))))
		return;
	pe = pEvent;

	for (i=1; i<=numRects; i++, pe++, pBox++)
	{
	    pe->u.u.type = GraphicsExpose;
	    pe->u.graphicsExposure.drawable = drawable;
	    pe->u.graphicsExposure.x = pBox->x1;
	    pe->u.graphicsExposure.y = pBox->y1;
	    pe->u.graphicsExposure.width = pBox->x2 - pBox->x1;
	    pe->u.graphicsExposure.height = pBox->y2 - pBox->y1;
	    pe->u.graphicsExposure.count = numRects - i;
	    pe->u.graphicsExposure.majorEvent = major;
	    pe->u.graphicsExposure.minorEvent = minor;
	}
	TryClientEvents(client, pEvent, numRects,
			    (Mask)0, NoEventMask, NullGrab);
	DEALLOCATE_LOCAL(pEvent);
    }
    else
    {
        xEvent event;
	event.u.u.type = NoExpose;
	event.u.noExposure.drawable = drawable;
	event.u.noExposure.majorEvent = major;
	event.u.noExposure.minorEvent = minor;
	TryClientEvents(client, &event, 1,
	    (Mask)0, NoEventMask, NullGrab);
    }
}


void
miSendExposures(pWin, pRgn, dx, dy)
    WindowPtr pWin;
    RegionPtr pRgn;
    int dx, dy;
{
    BoxPtr pBox;
    int numRects;
    xEvent *pEvent, *pe;
    int i;

    pBox = REGION_RECTS(pRgn);
    numRects = REGION_NUM_RECTS(pRgn);
    if(!(pEvent = (xEvent *) ALLOCATE_LOCAL(numRects * sizeof(xEvent))))
	return;

    for (i=numRects, pe = pEvent; --i >= 0; pe++, pBox++)
    {
	pe->u.u.type = Expose;
	pe->u.expose.window = pWin->drawable.id;
	pe->u.expose.x = pBox->x1 - dx;
	pe->u.expose.y = pBox->y1 - dy;
	pe->u.expose.width = pBox->x2 - pBox->x1;
	pe->u.expose.height = pBox->y2 - pBox->y1;
	pe->u.expose.count = i;
    }


    DeliverEvents(pWin, pEvent, numRects, NullWindow);

    DEALLOCATE_LOCAL(pEvent);
}

_X_EXPORT void
miWindowExposures(pWin, prgn, other_exposed)
    WindowPtr pWin;
    RegionPtr prgn, other_exposed;
{
    RegionPtr   exposures = prgn;
    if ((prgn && !REGION_NIL(prgn)) ||
	(exposures && !REGION_NIL(exposures)) || other_exposed)
    {
	RegionRec   expRec;
	int	    clientInterested;

	/*
	 * Restore from backing-store FIRST.
	 */
	clientInterested = (pWin->eventMask|wOtherEventMasks(pWin)) & ExposureMask;
	if (other_exposed)
	{
	    if (exposures)
	    {
		REGION_UNION(other_exposed,
						  exposures,
					          other_exposed);
		if (exposures != prgn)
		    REGION_DESTROY(exposures);
	    }
	    exposures = other_exposed;
	}
	if (clientInterested && exposures && (REGION_NUM_RECTS(exposures) > RECTLIMIT))
	{
	    /*
	     * If we have LOTS of rectangles, we decide to take the extents
	     * and force an exposure on that.  This should require much less
	     * work overall, on both client and server.  This is cheating, but
	     * isn't prohibited by the protocol ("spontaneous combustion" :-).
	     */
	    BoxRec box;

	    box = *REGION_EXTENTS(exposures);
	    if (exposures == prgn) {
		exposures = &expRec;
		REGION_INIT(exposures, &box, 1);
		REGION_RESET(prgn, &box);
	    } else {
		REGION_RESET(exposures, &box);
		REGION_UNION(prgn, prgn, exposures);
	    }
	    /* PaintWindowBackground doesn't clip, so we have to */
	    REGION_INTERSECT(prgn, prgn, &pWin->clipList);
	}
	if (prgn && !REGION_NIL(prgn))
	    (*pWin->drawable.pScreen->PaintWindowBackground)(pWin, prgn, PW_BACKGROUND);
	if (clientInterested && exposures && !REGION_NIL(exposures))
	    miSendExposures(pWin, exposures,
			    pWin->drawable.x, pWin->drawable.y);
	if (exposures == &expRec)
	{
	    REGION_UNINIT(exposures);
	}
	else if (exposures && exposures != prgn && exposures != other_exposed)
	    REGION_DESTROY(exposures);
	if (prgn)
	    REGION_EMPTY(prgn);
    }
    else if (exposures && exposures != prgn)
	REGION_DESTROY(exposures);
}


/*
    this code is highly unlikely.  it is not haile selassie.

    there is some hair here.  we can't just use the window's
clip region as it is, because if we are painting the border,
the border is not in the client area and so we will be excluded
when we validate the GC, and if we are painting a parent-relative
background, the area we want to paint is in some other window.
since we trust the code calling us to tell us to paint only areas
that are really ours, we will temporarily give the window a
clipList the size of the whole screen and an origin at (0,0).
this more or less assumes that ddX code will do translation
based on the window's absolute position, and that ValidateGC will
look at clipList, and that no other fields from the
window will be used.  it's not possible to just draw
in the root because it may be a different depth.

to get the tile to align correctly we set the GC's tile origin to
be the (x,y) of the window's upper left corner, after which we
get the right bits when drawing into the root.

because the clip_mask is being set to None, we may call DoChangeGC with
fPointer set true, thus we no longer need to install the background or
border tile in the resource table.
*/

/* MICLEARDRAWABLE -- sets the entire drawable to the background color of
 * the GC.  Useful when we have a scratch drawable and need to initialize
 * it. */
_X_EXPORT void
miClearDrawable(pDraw, pGC)
    DrawablePtr	pDraw;
    GCPtr	pGC;
{
    XID fg = pGC->fgPixel;
    XID bg = pGC->bgPixel;
    xRectangle rect;

    rect.x = 0;
    rect.y = 0;
    rect.width = pDraw->width;
    rect.height = pDraw->height;
    DoChangeGC(pGC, GCForeground, &bg, 0);
    ValidateGC(pDraw, pGC);
    (*pGC->ops->PolyFillRect)(pDraw, pGC, 1, &rect);
    DoChangeGC(pGC, GCForeground, &fg, 0);
    ValidateGC(pDraw, pGC);
}
