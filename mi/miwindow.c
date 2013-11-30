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
#ifdef HAVE_DIX_CONFIG_H
#include <dix-config.h>
#endif

#include <X11/X.h>
#include "regionstr.h"
#include "region.h"
#include "mi.h"
#include "windowstr.h"
#include "scrnintstr.h"
#include "pixmapstr.h"
#include "mivalidate.h"

_X_EXPORT void
miClearToBackground(pWin, x, y, w, h, generateExposures)
    WindowPtr pWin;
    int x,y;
    int w,h;
    Bool generateExposures;
{
    BoxRec box;
    RegionRec	reg;
    RegionPtr pBSReg = NullRegion;
    ScreenPtr	pScreen;
    BoxPtr  extents;
    int	    x1, y1, x2, y2;

    /* compute everything using ints to avoid overflow */

    x1 = pWin->drawable.x + x;
    y1 = pWin->drawable.y + y;
    if (w)
        x2 = x1 + (int) w;
    else
        x2 = x1 + (int) pWin->drawable.width - (int) x;
    if (h)
        y2 = y1 + h;
    else
        y2 = y1 + (int) pWin->drawable.height - (int) y;

    extents = &pWin->clipList.extents;

    /* clip the resulting rectangle to the window clipList extents.  This
     * makes sure that the result will fit in a box, given that the
     * screen is < 32768 on a side.
     */

    if (x1 < extents->x1)
	x1 = extents->x1;
    if (x2 > extents->x2)
	x2 = extents->x2;
    if (y1 < extents->y1)
	y1 = extents->y1;
    if (y2 > extents->y2)
	y2 = extents->y2;

    if (x2 <= x1 || y2 <= y1)
    {
	x2 = x1 = 0;
	y2 = y1 = 0;
    }

    box.x1 = x1;
    box.x2 = x2;
    box.y1 = y1;
    box.y2 = y2;

    pScreen = pWin->drawable.pScreen;
    REGION_INIT(&reg, &box, 1);

    REGION_INTERSECT(&reg, &reg, &pWin->clipList);
    if (generateExposures)
	(*pScreen->WindowExposures)(pWin, &reg, pBSReg);
    else if (pWin->backgroundState != None)
        (*pScreen->PaintWindowBackground)(pWin, &reg, PW_BACKGROUND);
    REGION_UNINIT(&reg);
    if (pBSReg)
	REGION_DESTROY(pBSReg);
}

void
miMarkWindow(pWin)
    WindowPtr pWin;
{
    ValidatePtr val;

    if (pWin->valdata)
	return;
    val = (ValidatePtr)malloc(sizeof(ValidateRec));
    val->before.oldAbsCorner.x = pWin->drawable.x;
    val->before.oldAbsCorner.y = pWin->drawable.y;
    val->before.borderVisible = NullRegion;
    val->before.resized = FALSE;
    pWin->valdata = val;
}

Bool
miMarkOverlappedWindows(pWin, pFirst, ppLayerWin)
    WindowPtr pWin;
    WindowPtr pFirst;
    WindowPtr *ppLayerWin;
{
    BoxPtr box;
    WindowPtr pChild, pLast;
    Bool anyMarked = FALSE;
    MarkWindowProcPtr MarkWindow = pWin->drawable.pScreen->MarkWindow;

    /* single layered systems are easy */
    if (ppLayerWin) *ppLayerWin = pWin;

    if (pWin == pFirst)
    {
	/* Blindly mark pWin and all of its inferiors.	 This is a slight
	 * overkill if there are mapped windows that outside pWin's border,
	 * but it's better than wasting time on RectIn checks.
	 */
	pChild = pWin;
	while (1)
	{
	    if (pChild->viewable)
	    {
		if (REGION_BROKEN(&pChild->winSize))
		    SetWinSize (pChild);
		if (REGION_BROKEN(&pChild->borderSize))
		    SetBorderSize (pChild);
		(* MarkWindow)(pChild);
		if (pChild->firstChild)
		{
		    pChild = pChild->firstChild;
		    continue;
		}
	    }
	    while (!pChild->nextSib && (pChild != pWin))
		pChild = pChild->parent;
	    if (pChild == pWin)
		break;
	    pChild = pChild->nextSib;
	}
	anyMarked = TRUE;
	pFirst = pFirst->nextSib;
    }
    if ( (pChild = pFirst) )
    {
	box = REGION_EXTENTS(&pWin->borderSize);
	pLast = pChild->parent->lastChild;
	while (1)
	{
	    if (pChild->viewable)
	    {
		if (REGION_BROKEN(&pChild->winSize))
		    SetWinSize (pChild);
		if (REGION_BROKEN(&pChild->borderSize))
		    SetBorderSize (pChild);
		if (RECT_IN_REGION(&pChild->borderSize, box))
		{
		    (* MarkWindow)(pChild);
		    anyMarked = TRUE;
		    if (pChild->firstChild)
		    {
			pChild = pChild->firstChild;
			continue;
		    }
		}
	    }
	    while (!pChild->nextSib && (pChild != pLast))
		pChild = pChild->parent;
	    if (pChild == pLast)
		break;
	    pChild = pChild->nextSib;
	}
    }
    if (anyMarked)
	(* MarkWindow)(pWin->parent);
    return anyMarked;
}

/*****
 *  miHandleValidateExposures(pWin)
 *    starting at pWin, draw background in any windows that have exposure
 *    regions, translate the regions, restore any backing store,
 *    and then send any regions still exposed to the client
 *****/
_X_EXPORT void
miHandleValidateExposures(pWin)
    WindowPtr pWin;
{
    WindowPtr pChild;
    ValidatePtr val;
    WindowExposuresProcPtr WindowExposures;

    pChild = pWin;
    WindowExposures = pChild->drawable.pScreen->WindowExposures;
    while (1)
    {
	if ( (val = pChild->valdata) )
	{
	    if (REGION_NOTEMPTY(&val->after.borderExposed))
		(*pChild->drawable.pScreen->PaintWindowBorder)(pChild,
						    &val->after.borderExposed,
						    PW_BORDER);
	    REGION_UNINIT(&val->after.borderExposed);
	    (*WindowExposures)(pChild, &val->after.exposed, NullRegion);
	    REGION_UNINIT(&val->after.exposed);
	    free(val);
	    pChild->valdata = (ValidatePtr)NULL;
	    if (pChild->firstChild)
	    {
		pChild = pChild->firstChild;
		continue;
	    }
	}
	while (!pChild->nextSib && (pChild != pWin))
	    pChild = pChild->parent;
	if (pChild == pWin)
	    break;
	pChild = pChild->nextSib;
    }
}

void
miMoveWindow(pWin, x, y, pNextSib, kind)
    WindowPtr pWin;
    int x,y;
    WindowPtr pNextSib;
    VTKind kind;
{
    WindowPtr pParent;
    Bool WasViewable = (Bool)(pWin->viewable);
    short bw;
    RegionPtr oldRegion = NULL;
    DDXPointRec oldpt;
    Bool anyMarked = FALSE;
    ScreenPtr pScreen;
    WindowPtr windowToValidate;
    WindowPtr pLayerWin;

    /* if this is a root window, can't be moved */
    if (!(pParent = pWin->parent))
       return ;
    pScreen = pWin->drawable.pScreen;
    bw = wBorderWidth (pWin);

    oldpt.x = pWin->drawable.x;
    oldpt.y = pWin->drawable.y;
    if (WasViewable)
    {
	oldRegion = REGION_CREATE(NullBox, 1);
	REGION_COPY(oldRegion, &pWin->borderClip);
	anyMarked = (*pScreen->MarkOverlappedWindows)(pWin, pWin, &pLayerWin);
    }
    pWin->origin.x = x + (int)bw;
    pWin->origin.y = y + (int)bw;
    x = pWin->drawable.x = pParent->drawable.x + x + (int)bw;
    y = pWin->drawable.y = pParent->drawable.y + y + (int)bw;

    SetWinSize (pWin);
    SetBorderSize (pWin);

    (*pScreen->PositionWindow)(pWin, x, y);

    windowToValidate = MoveWindowInStack(pWin, pNextSib);

    ResizeChildrenWinSize(pWin, x - oldpt.x, y - oldpt.y, 0, 0);

    if (WasViewable)
    {
	if (pLayerWin == pWin)
	    anyMarked |= (*pScreen->MarkOverlappedWindows)
				(pWin, windowToValidate, (WindowPtr *)NULL);
	else
	    anyMarked |= (*pScreen->MarkOverlappedWindows)
				(pWin, pLayerWin, (WindowPtr *)NULL);


	if (anyMarked)
	{
	    (*pScreen->ValidateTree)(pLayerWin->parent, NullWindow, kind);
	    (* pWin->drawable.pScreen->CopyWindow)(pWin, oldpt, oldRegion);
	    REGION_DESTROY(oldRegion);
	    /* XXX need to retile border if ParentRelative origin */
	    (*pScreen->HandleExposures)(pLayerWin->parent);
	}
	if (anyMarked && pScreen->PostValidateTree)
	    (*pScreen->PostValidateTree)(pLayerWin->parent, NullWindow, kind);
    }
    if (pWin->realized)
	WindowsRestructured ();
}


/*
 * pValid is a region of the screen which has been
 * successfully copied -- recomputed exposed regions for affected windows
 */

static int
miRecomputeExposures (
    register WindowPtr	pWin,
    pointer		value) /* must conform to VisitWindowProcPtr */
{
    RegionPtr	pValid = (RegionPtr)value;

    if (pWin->valdata)
    {
	/*
	 * compute exposed regions of this window
	 */
	REGION_SUBTRACT(&pWin->valdata->after.exposed,
			&pWin->clipList, pValid);
	/*
	 * compute exposed regions of the border
	 */
	REGION_SUBTRACT(&pWin->valdata->after.borderExposed,
			     &pWin->borderClip, &pWin->winSize);
	REGION_SUBTRACT(&pWin->valdata->after.borderExposed,
			     &pWin->valdata->after.borderExposed, pValid);
	return WT_WALKCHILDREN;
    }
    return WT_NOMATCH;
}

void
miSlideAndSizeWindow(pWin, x, y, w, h, pSib)
    WindowPtr pWin;
    int x,y;
    unsigned int w, h;
    WindowPtr pSib;
{
    WindowPtr pParent;
    Bool WasViewable = (Bool)(pWin->viewable);
    unsigned short width = pWin->drawable.width,
		   height = pWin->drawable.height;
    short oldx = pWin->drawable.x,
	  oldy = pWin->drawable.y;
    int bw = wBorderWidth (pWin);
    short dw, dh;
    DDXPointRec oldpt;
    RegionPtr oldRegion = NULL;
    Bool anyMarked = FALSE;
    ScreenPtr pScreen;
    WindowPtr pFirstChange;
    WindowPtr pChild;
    RegionPtr	gravitate[StaticGravity + 1];
    unsigned g;
    int		nx, ny;		/* destination x,y */
    int		newx, newy;	/* new inner window position */
    RegionPtr	pRegion = NULL;
    RegionPtr	destClip;	/* portions of destination already written */
    RegionPtr	oldWinClip = NULL;	/* old clip list for window */
    RegionPtr	borderVisible = NullRegion; /* visible area of the border */
    RegionPtr	bsExposed = NullRegion;	    /* backing store exposures */
    Bool	shrunk = FALSE; /* shrunk in an inner dimension */
    Bool	moved = FALSE;	/* window position changed */
    WindowPtr  pLayerWin;

    /* if this is a root window, can't be resized */
    if (!(pParent = pWin->parent))
	return ;

    pScreen = pWin->drawable.pScreen;
    newx = pParent->drawable.x + x + bw;
    newy = pParent->drawable.y + y + bw;
    if (WasViewable)
    {
	anyMarked = FALSE;
	/*
	 * save the visible region of the window
	 */
	oldRegion = REGION_CREATE(NullBox, 1);
	REGION_COPY(oldRegion, &pWin->winSize);

	/*
	 * categorize child windows into regions to be moved
	 */
	for (g = 0; g <= StaticGravity; g++)
	    gravitate[g] = (RegionPtr) NULL;
	for (pChild = pWin->firstChild; pChild; pChild = pChild->nextSib)
	{
	    g = pChild->winGravity;
	    if (g != UnmapGravity)
	    {
		if (!gravitate[g])
		    gravitate[g] = REGION_CREATE(NullBox, 1);
		REGION_UNION(gravitate[g],
				   gravitate[g], &pChild->borderClip);
	    }
	    else
	    {
		UnmapWindow(pChild, TRUE);
		anyMarked = TRUE;
	    }
	}
	anyMarked |= (*pScreen->MarkOverlappedWindows)(pWin, pWin,
						       &pLayerWin);

	oldWinClip = NULL;
	if (pWin->bitGravity != ForgetGravity)
	{
	    oldWinClip = REGION_CREATE(NullBox, 1);
	    REGION_COPY(oldWinClip, &pWin->clipList);
	}
	/*
	 * if the window is changing size, borderExposed
	 * can't be computed correctly without some help.
	 */
	if (pWin->drawable.height > h || pWin->drawable.width > w)
	    shrunk = TRUE;

	if (newx != oldx || newy != oldy)
	    moved = TRUE;

	if ((pWin->drawable.height != h || pWin->drawable.width != w) &&
	    HasBorder (pWin))
	{
	    borderVisible = REGION_CREATE(NullBox, 1);
	    /* for tiled borders, we punt and draw the whole thing */
	    if (pWin->borderIsPixel || !moved)
	    {
		if (shrunk || moved)
		    REGION_SUBTRACT(borderVisible,
					  &pWin->borderClip,
					  &pWin->winSize);
		else
		    REGION_COPY(borderVisible,
					    &pWin->borderClip);
	    }
	}
    }
    pWin->origin.x = x + bw;
    pWin->origin.y = y + bw;
    pWin->drawable.height = h;
    pWin->drawable.width = w;

    x = pWin->drawable.x = newx;
    y = pWin->drawable.y = newy;

    SetWinSize (pWin);
    SetBorderSize (pWin);

    dw = (int)w - (int)width;
    dh = (int)h - (int)height;
    ResizeChildrenWinSize(pWin, x - oldx, y - oldy, dw, dh);

    /* let the hardware adjust background and border pixmaps, if any */
    (*pScreen->PositionWindow)(pWin, x, y);

    pFirstChange = MoveWindowInStack(pWin, pSib);

    if (WasViewable)
    {
	pRegion = REGION_CREATE(NullBox, 1);

	if (pLayerWin == pWin)
	    anyMarked |= (*pScreen->MarkOverlappedWindows)(pWin, pFirstChange,
						(WindowPtr *)NULL);
	else
	    anyMarked |= (*pScreen->MarkOverlappedWindows)(pWin, pLayerWin,
						(WindowPtr *)NULL);

	if (pWin->valdata)
	{
	    pWin->valdata->before.resized = TRUE;
	    pWin->valdata->before.borderVisible = borderVisible;
	}


	if (anyMarked)
	    (*pScreen->ValidateTree)(pLayerWin->parent, pFirstChange, VTOther);
	/*
	 * the entire window is trashed unless bitGravity
	 * recovers portions of it
	 */
	REGION_COPY(&pWin->valdata->after.exposed, &pWin->clipList);
    }

    GravityTranslate (x, y, oldx, oldy, dw, dh, pWin->bitGravity, &nx, &ny);

    if (WasViewable)
    {
	/* avoid the border */
	if (HasBorder (pWin))
	{
	    int	offx, offy, dx, dy;

	    /* kruft to avoid double translates for each gravity */
	    offx = 0;
	    offy = 0;
	    for (g = 0; g <= StaticGravity; g++)
	    {
		if (!gravitate[g])
		    continue;

		/* align winSize to gravitate[g].
		 * winSize is in new coordinates,
		 * gravitate[g] is still in old coordinates */
		GravityTranslate (x, y, oldx, oldy, dw, dh, g, &nx, &ny);

		dx = (oldx - nx) - offx;
		dy = (oldy - ny) - offy;
		if (dx || dy)
		{
		    REGION_TRANSLATE(&pWin->winSize, dx, dy);
		    offx += dx;
		    offy += dy;
		}
		REGION_INTERSECT(gravitate[g], gravitate[g],
				 &pWin->winSize);
	    }
	    /* get winSize back where it belongs */
	    if (offx || offy)
		REGION_TRANSLATE(&pWin->winSize, -offx, -offy);
	}
	/*
	 * add screen bits to the appropriate bucket
	 */

	if (oldWinClip)
	{
	    /*
	     * clip to new clipList
	     */
	    REGION_COPY(pRegion, oldWinClip);
	    REGION_TRANSLATE(pRegion, nx - oldx, ny - oldy);
	    REGION_INTERSECT(oldWinClip, pRegion, &pWin->clipList);
	    /*
	     * don't step on any gravity bits which will be copied after this
	     * region.	Note -- this assumes that the regions will be copied
	     * in gravity order.
	     */
	    for (g = pWin->bitGravity + 1; g <= StaticGravity; g++)
	    {
		if (gravitate[g])
		    REGION_SUBTRACT(oldWinClip, oldWinClip,
					gravitate[g]);
	    }
	    REGION_TRANSLATE(oldWinClip, oldx - nx, oldy - ny);
	    g = pWin->bitGravity;
	    if (!gravitate[g])
		gravitate[g] = oldWinClip;
	    else
	    {
		REGION_UNION(gravitate[g], gravitate[g], oldWinClip);
		REGION_DESTROY(oldWinClip);
	    }
	}

	/*
	 * move the bits on the screen
	 */

	destClip = NULL;

	for (g = 0; g <= StaticGravity; g++)
	{
	    if (!gravitate[g])
		continue;

	    GravityTranslate (x, y, oldx, oldy, dw, dh, g, &nx, &ny);

	    oldpt.x = oldx + (x - nx);
	    oldpt.y = oldy + (y - ny);

	    /* Note that gravitate[g] is *translated* by CopyWindow */

	    /* only copy the remaining useful bits */

	    REGION_INTERSECT(gravitate[g], gravitate[g], oldRegion);

	    /* clip to not overwrite already copied areas */

	    if (destClip) {
		REGION_TRANSLATE(destClip, oldpt.x - x, oldpt.y - y);
		REGION_SUBTRACT(gravitate[g], gravitate[g], destClip);
		REGION_TRANSLATE(destClip, x - oldpt.x, y - oldpt.y);
	    }

	    /* and move those bits */

	    if (oldpt.x != x || oldpt.y != y
		)
	    {
		(*pWin->drawable.pScreen->CopyWindow)(pWin, oldpt, gravitate[g]);
	    }

	    /* remove any overwritten bits from the remaining useful bits */

	    REGION_SUBTRACT(oldRegion, oldRegion, gravitate[g]);

	    /*
	     * recompute exposed regions of child windows
	     */

	    for (pChild = pWin->firstChild; pChild; pChild = pChild->nextSib)
	    {
		if (pChild->winGravity != g)
		    continue;
		REGION_INTERSECT(pRegion,
				       &pChild->borderClip, gravitate[g]);
		TraverseTree (pChild, miRecomputeExposures, (pointer)pRegion);
	    }

	    /*
	     * remove the successfully copied regions of the
	     * window from its exposed region
	     */

	    if (g == pWin->bitGravity)
		REGION_SUBTRACT(&pWin->valdata->after.exposed,
				     &pWin->valdata->after.exposed, gravitate[g]);
	    if (!destClip)
		destClip = gravitate[g];
	    else
	    {
		REGION_UNION(destClip, destClip, gravitate[g]);
		REGION_DESTROY(gravitate[g]);
	    }
	}

	REGION_DESTROY(oldRegion);
	REGION_DESTROY(pRegion);
	if (destClip)
	    REGION_DESTROY(destClip);
	if (bsExposed)
	{
	    RegionPtr	valExposed = NullRegion;

	    if (pWin->valdata)
		valExposed = &pWin->valdata->after.exposed;
	    (*pScreen->WindowExposures) (pWin, valExposed, bsExposed);
	    if (valExposed)
		REGION_EMPTY(valExposed);
	    REGION_DESTROY(bsExposed);
	}
	if (anyMarked)
	    (*pScreen->HandleExposures)(pLayerWin->parent);
	if (anyMarked && pScreen->PostValidateTree)
	    (*pScreen->PostValidateTree)(pLayerWin->parent, pFirstChange,
					  VTOther);
    }
    else if (bsExposed)
    {
	(*pScreen->WindowExposures) (pWin, NullRegion, bsExposed);
	REGION_DESTROY(bsExposed);
    }
    if (pWin->realized)
	WindowsRestructured ();
}

WindowPtr
miGetLayerWindow(pWin)
    WindowPtr pWin;
{
    return pWin->firstChild;
}

/******
 *
 * miSetShape
 *    The border/window shape has changed.  Recompute winSize/borderSize
 *    and send appropriate exposure events
 */

_X_EXPORT void
miSetShape(pWin)
    WindowPtr	pWin;
{
    Bool	WasViewable = (Bool)(pWin->viewable);
    ScreenPtr pScreen = pWin->drawable.pScreen;
    Bool	anyMarked = FALSE;
    WindowPtr   pLayerWin;

    if (WasViewable)
    {
	anyMarked = (*pScreen->MarkOverlappedWindows)(pWin, pWin,
						      &pLayerWin);
	if (pWin->valdata)
	{
	    if (HasBorder (pWin))
	    {
		RegionPtr	borderVisible;

		borderVisible = REGION_CREATE(NullBox, 1);
		REGION_SUBTRACT(borderVisible,
				      &pWin->borderClip, &pWin->winSize);
		pWin->valdata->before.borderVisible = borderVisible;
	    }
	    pWin->valdata->before.resized = TRUE;
	}
    }

    SetWinSize (pWin);
    SetBorderSize (pWin);

    ResizeChildrenWinSize(pWin, 0, 0, 0, 0);

    if (WasViewable)
    {
	anyMarked |= (*pScreen->MarkOverlappedWindows)(pWin, pWin,
						(WindowPtr *)NULL);


	if (anyMarked)
	    (*pScreen->ValidateTree)(pLayerWin->parent, NullWindow, VTOther);
    }

    if (WasViewable)
    {
	if (anyMarked)
	    (*pScreen->HandleExposures)(pLayerWin->parent);
	if (anyMarked && pScreen->PostValidateTree)
	    (*pScreen->PostValidateTree)(pLayerWin->parent, NullWindow, VTOther);
    }
    if (pWin->realized)
	WindowsRestructured ();
    CheckCursorConfinement(pWin);
}

/* Keeps the same inside(!) origin */

_X_EXPORT void
miChangeBorderWidth(pWin, width)
    WindowPtr pWin;
    unsigned int width;
{
    int oldwidth;
    Bool anyMarked = FALSE;
    ScreenPtr pScreen;
    Bool WasViewable = (Bool)(pWin->viewable);
    Bool HadBorder;
    WindowPtr  pLayerWin;

    oldwidth = wBorderWidth (pWin);
    if (oldwidth == width)
	return;
    HadBorder = HasBorder(pWin);
    pScreen = pWin->drawable.pScreen;
    if (WasViewable && width < oldwidth)
	anyMarked = (*pScreen->MarkOverlappedWindows)(pWin, pWin, &pLayerWin);

    pWin->borderWidth = width;
    SetBorderSize (pWin);

    if (WasViewable)
    {
	if (width > oldwidth)
	{
	    anyMarked = (*pScreen->MarkOverlappedWindows)(pWin, pWin,
							  &pLayerWin);
	    /*
	     * save the old border visible region to correctly compute
	     * borderExposed.
	     */
	    if (pWin->valdata && HadBorder)
	    {
		RegionPtr   borderVisible;
		borderVisible = REGION_CREATE(NULL, 1);
		REGION_SUBTRACT(borderVisible,
				      &pWin->borderClip, &pWin->winSize);
		pWin->valdata->before.borderVisible = borderVisible;
	    }
	}

	if (anyMarked)
	{
	    (*pScreen->ValidateTree)(pLayerWin->parent, pLayerWin, VTOther);
	    (*pScreen->HandleExposures)(pLayerWin->parent);
	}
	if (anyMarked && pScreen->PostValidateTree)
	    (*pScreen->PostValidateTree)(pLayerWin->parent, pLayerWin,
					  VTOther);
    }
    if (pWin->realized)
	WindowsRestructured ();
}

void
miMarkUnrealizedWindow(pChild, pWin, fromConfigure)
    WindowPtr pChild;
    WindowPtr pWin;
    Bool fromConfigure;
{
    if ((pChild != pWin) || fromConfigure)
    {
	REGION_EMPTY(&pChild->clipList);
	if (pChild->drawable.pScreen->ClipNotify)
	    (* pChild->drawable.pScreen->ClipNotify)(pChild, 0, 0);
	REGION_EMPTY(&pChild->borderClip);
    }
}

_X_EXPORT void
miSegregateChildren(WindowPtr pWin, RegionPtr pReg, int depth)
{
    WindowPtr pChild;

    for (pChild = pWin->firstChild; pChild; pChild = pChild->nextSib)
    {
	if (pChild->drawable.depth == depth)
	    REGION_UNION(pReg, pReg, &pChild->borderClip);

	if (pChild->firstChild)
	    miSegregateChildren(pChild, pReg, depth);
    }
}
