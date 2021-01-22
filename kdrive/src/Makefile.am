AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = 					\
	@KDRIVE_INCS@				\
	@KDRIVE_CFLAGS@

AM_CFLAGS =	-DHAVE_DIX_CONFIG_H

noinst_LIBRARIES = libkdrive.a libkdrivestubs.a

libkdrive_a_SOURCES =	\
	kcmap.c		\
	kdrive.c	\
	kdrive.h	\
	kinfo.c		\
	kinput.c	\
	kkeymap.h	\
	kmap.c		\
	kmode.c		\
	kshadow.c	\
        $(top_srcdir)/mi/miinitext.c

libkdrivestubs_a_SOURCES = \
        $(top_srcdir)/fb/fbcmap.c
