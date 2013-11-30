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

#ifndef REGIONSTRUCT_H
#define REGIONSTRUCT_H

typedef struct _Region RegionRec, *RegionPtr;

#include "miscstruct.h"

/* Return values from RectIn() */

#define rgnOUT 0
#define rgnIN  1
#define rgnPART 2

#define NullRegion ((RegionPtr)0)

/*
 *   clip region
 */

typedef struct _RegData {
    long	size;
    long 	numRects;
/*  BoxRec	rects[size];   in memory but not explicitly declared */
} RegDataRec, *RegDataPtr;

struct _Region {
    BoxRec 	extents;
    RegDataPtr	data;
};

extern BoxRec miEmptyBox;
extern RegDataRec miEmptyData;
extern RegDataRec miBrokenData;

#define REGION_NIL(reg) ((reg)->data && !(reg)->data->numRects)
/* not a region */
#define REGION_NAR(reg)	((reg)->data == &miBrokenData)
#define REGION_NUM_RECTS(reg) ((reg)->data ? (reg)->data->numRects : 1)
#define REGION_SIZE(reg) ((reg)->data ? (reg)->data->size : 0)
#define REGION_RECTS(reg) ((reg)->data ? (BoxPtr)((reg)->data + 1) \
			               : &(reg)->extents)
#define REGION_BOXPTR(reg) ((BoxPtr)((reg)->data + 1))
#define REGION_BOX(reg,i) (&REGION_BOXPTR(reg)[i])
#define REGION_TOP(reg) REGION_BOX(reg, (reg)->data->numRects)
#define REGION_END(reg) REGION_BOX(reg, (reg)->data->numRects - 1)
#define REGION_SZOF(n) (sizeof(RegDataRec) + ((n) * sizeof(BoxRec)))

#define REGION_CREATE(_rect, _size) \
    miRegionCreate(_rect, _size)

#define REGION_COPY(dst, src) \
    miRegionCopy(dst, src)

#define REGION_DESTROY(_pReg) \
    miRegionDestroy(_pReg)

#define REGION_INTERSECT(newReg, reg1, reg2) \
    miIntersect(newReg, reg1, reg2)

#define REGION_UNION(newReg, reg1, reg2) \
    miUnion(newReg, reg1, reg2)

#define REGION_SUBTRACT(newReg, reg1, reg2) \
    miSubtract(newReg, reg1, reg2)

#define REGION_INVERSE(newReg, reg1, invRect) \
    miInverse(newReg, reg1, invRect)

#define REGION_TRANSLATE(_pReg, _x, _y) \
    miTranslateRegion(_pReg, _x, _y)

#define RECT_IN_REGION(_pReg, prect) \
    miRectIn(_pReg, prect)

#define POINT_IN_REGION(_pReg, _x, _y, prect) \
    miPointInRegion(_pReg, _x, _y, prect)

#define REGION_APPEND(dstrgn, rgn) \
    miRegionAppend(dstrgn, rgn)

#define REGION_VALIDATE(badreg, pOverlap) \
    miRegionValidate(badreg, pOverlap)

#define BITMAP_TO_REGION(_pScreen, pPix) \
    (*(_pScreen)->BitmapToRegion)(pPix) /* no mi version?! */

#define RECTS_TO_REGION(nrects, prect, ctype) \
    miRectsToRegion(nrects, prect, ctype)

#define REGION_EQUAL(_pReg1, _pReg2) \
    miRegionEqual(_pReg1, _pReg2)

#define REGION_BREAK(_pReg) \
    miRegionBreak(_pReg)

static inline void REGION_INIT(RegionPtr _pReg, BoxPtr _rect, int _size) {

    if (_rect != NULL)
    {
        (_pReg)->extents = *(_rect);
        (_pReg)->data = (RegDataPtr)NULL;
    }
    else
    {
        (_pReg)->extents = miEmptyBox;
        if (((_size) > 1) && ((_pReg)->data =
                             (RegDataPtr)malloc(REGION_SZOF(_size))))
        {
            (_pReg)->data->size = (_size);
            (_pReg)->data->numRects = 0;
        }
        else
            (_pReg)->data = &miEmptyData;
    }
}

#define REGION_UNINIT(_pReg) \
{ \
    if ((_pReg)->data && (_pReg)->data->size && (_pReg)->data != (&miEmptyData)) { \
	free((_pReg)->data); \
    } \
    (_pReg)->data = NULL; \
}

#define REGION_RESET(_pReg, _pBox) \
{ \
    (_pReg)->extents = *(_pBox); \
    REGION_UNINIT(_pReg); \
    (_pReg)->data = (RegDataPtr)NULL; \
}

#define REGION_NOTEMPTY(_pReg) \
    !REGION_NIL(_pReg)

#define REGION_BROKEN(_pReg) \
    REGION_NAR(_pReg)

#define REGION_EMPTY(_pReg) \
{ \
    REGION_UNINIT(_pReg); \
    (_pReg)->extents.x2 = (_pReg)->extents.x1; \
    (_pReg)->extents.y2 = (_pReg)->extents.y1; \
    (_pReg)->data = &miEmptyData; \
}

#define REGION_EXTENTS(_pReg) \
    (&(_pReg)->extents)

#define REGION_NULL(_pReg) \
{ \
    (_pReg)->extents = miEmptyBox; \
    (_pReg)->data = &miEmptyData; \
}

/* moved from mi.h */

RegionPtr miRegionCreate(
    BoxPtr /*rect*/,
    int /*size*/);

void miRegionDestroy(
    RegionPtr /*pReg*/);

Bool miRegionCopy(
    RegionPtr /*dst*/,
    RegionPtr /*src*/);

Bool miIntersect(
    RegionPtr /*newReg*/,
    RegionPtr /*reg1*/,
    RegionPtr /*reg2*/);

Bool miUnion(
    RegionPtr /*newReg*/,
    RegionPtr /*reg1*/,
    RegionPtr /*reg2*/);

Bool miRegionAppend(
    RegionPtr /*dstrgn*/,
    RegionPtr /*rgn*/);

Bool miRegionValidate(
    RegionPtr /*badreg*/,
    Bool * /*pOverlap*/);

RegionPtr miRectsToRegion(
    int /*nrects*/,
    xRectanglePtr /*prect*/,
    int /*ctype*/);

Bool miSubtract(
    RegionPtr /*regD*/,
    RegionPtr /*regM*/,
    RegionPtr /*regS*/);

Bool miInverse(
    RegionPtr /*newReg*/,
    RegionPtr /*reg1*/,
    BoxPtr /*invRect*/);

int miRectIn(
    RegionPtr /*region*/,
    BoxPtr /*prect*/);

void miTranslateRegion(
    RegionPtr /*pReg*/,
    int /*x*/,
    int /*y*/);

Bool miRegionBreak(
    RegionPtr /*pReg*/);

Bool miPointInRegion(
    RegionPtr /*pReg*/,
    int /*x*/,
    int /*y*/,
    BoxPtr /*box*/);

#endif /* REGIONSTRUCT_H */
