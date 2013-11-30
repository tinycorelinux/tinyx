noinst_LTLIBRARIES = libos.la

AM_CFLAGS = $(DIX_CFLAGS)

XDMCP_SOURCES = xdmcp.c
STRLCAT_SOURCES = strlcat.c strlcpy.c
XORG_SOURCES = log.c

libos_la_SOURCES = 	\
	WaitFor.c	\
	access.c	\
	auth.c		\
	connection.c	\
	io.c		\
	mitauth.c	\
	oscolor.c	\
	osdep.h		\
	osinit.c	\
	utils.c		\
	xdmauth.c	\
	xstrans.c	\
	xauembed.c	\
	$(XORG_SOURCES)

if XDMCP
libos_la_SOURCES += $(XDMCP_SOURCES)
endif

if NEED_STRLCAT
libos_la_SOURCES += $(STRLCAT_SOURCES)
endif

EXTRA_DIST = $(XDMCP_SOURCES) $(STRLCAT_SOURCES)
