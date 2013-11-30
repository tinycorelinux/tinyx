/*
 *
 * Copyright © 1999 Keith Packard
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

#include <kdrive-config.h>
#include "kdrive.h"

#include <errno.h>
#include <unistd.h>
#include <sys/mman.h>
#ifdef HAVE_ASM_MTRR_H
#include <asm/mtrr.h>
#endif

#include <sys/ioctl.h>

void *KdMapDevice(CARD32 addr, CARD32 size)
{
#ifdef linux
	void *a;

	int fd;

#ifdef __arm__
	fd = open("/dev/mem", O_RDWR | O_SYNC);
#else
	fd = open("/dev/mem", O_RDWR);
#endif
	if (fd < 0)
		FatalError("KdMapDevice: failed to open /dev/mem (%s)\n",
			   strerror(errno));

	a = mmap((caddr_t) 0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd,
		 addr);
	close(fd);
	if ((long)a == -1)
		FatalError("KdMapDevice: failed to map frame buffer (%s)\n",
			   strerror(errno));
	return a;
#endif
}

void KdUnmapDevice(void *addr, CARD32 size)
{
#ifdef linux
	munmap(addr, size);
#endif
}

#ifdef HAVE_ASM_MTRR_H
static int mtrr;
#endif

void KdSetMappedMode(CARD32 addr, CARD32 size, int mode)
{
#ifdef HAVE_ASM_MTRR_H
	struct mtrr_sentry sentry;

	unsigned long base, bound;

	unsigned int type = MTRR_TYPE_WRBACK;

	if (addr < 0x100000)
		return;
	if (!mtrr)
		mtrr = open("/proc/mtrr", 2);
	if (mtrr > 0) {
		unsigned long nsize;

		base = addr & ~((1 << 22) - 1);
		bound = ((addr + size) + ((1 << 22) - 1)) & ~((1 << 22) - 1);
		nsize = 1;
		while (nsize < (bound - base))
			nsize <<= 1;
		switch (mode) {
		case KD_MAPPED_MODE_REGISTERS:
			type = MTRR_TYPE_UNCACHABLE;
			break;
		case KD_MAPPED_MODE_FRAMEBUFFER:
			type = MTRR_TYPE_WRCOMB;
			break;
		}
		sentry.base = base;
		sentry.size = nsize;
		sentry.type = type;

		if (ioctl(mtrr, MTRRIOC_ADD_ENTRY, &sentry) < 0)
			ErrorF("MTRRIOC_ADD_ENTRY failed 0x%lx 0x%lx %d (errno %d)\n",
			     base, bound - base, type, errno);
	}
#endif
}

void KdResetMappedMode(CARD32 addr, CARD32 size, int mode)
{
#ifdef HAVE_ASM_MTRR_H
	struct mtrr_sentry sentry;

	unsigned long base, bound;

	unsigned int type = MTRR_TYPE_WRBACK;

	if (addr < 0x100000)
		return;
	if (!mtrr)
		mtrr = open("/proc/mtrr", 2);
	if (mtrr > 0) {
		unsigned long nsize;

		base = addr & ~((1 << 22) - 1);
		bound = ((addr + size) + ((1 << 22) - 1)) & ~((1 << 22) - 1);
		nsize = 1;
		while (nsize < (bound - base))
			nsize <<= 1;
		switch (mode) {
		case KD_MAPPED_MODE_REGISTERS:
			type = MTRR_TYPE_UNCACHABLE;
			break;
		case KD_MAPPED_MODE_FRAMEBUFFER:
			type = MTRR_TYPE_WRCOMB;
			break;
		}
		sentry.base = base;
		sentry.size = nsize;
		sentry.type = type;

		if (ioctl(mtrr, MTRRIOC_DEL_ENTRY, &sentry) < 0)
			ErrorF("MTRRIOC_DEL_ENTRY failed 0x%lx 0x%lx %d (errno %d)\n",
			     base, bound - base, type, errno);
	}
#endif
}
