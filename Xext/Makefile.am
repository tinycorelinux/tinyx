# libXext.la:        includes all extensions and should be linked into Xvfb,
#                    Xnest, Xdmx and Xprt
# libXextbuiltin.la: includes those extensions that are built directly into
#                    Xorg by default
# libXextmodule.la:  includes those extensions that are built into a module
#                    that Xorg loads
noinst_LTLIBRARIES = libXext.la

AM_CFLAGS = $(DIX_CFLAGS)

# Sources always included in libXextbuiltin.la & libXext.la
BUILTIN_SRCS =			\
	shape.c			\
	sleepuntil.c		\
	sleepuntil.h		\
	xtest.c

# Sources always included in libXextmodule.la & libXext.la
MODULE_SRCS =			\
	bigreq.c		\
	shape.c			\
	sync.c			\
	xcmisc.c

# Optional sources included if extension enabled by configure.ac rules

# MIT Shared Memory extension
MITSHM_SRCS = shm.c shmint.h
BUILTIN_SRCS += $(MITSHM_SRCS)

# XResource extension: lets clients get data about per-client resource usage
RES_SRCS = xres.c
if RES
MODULE_SRCS  += $(RES_SRCS)
endif

# MIT ScreenSaver extension
SCREENSAVER_SRCS = saver.c
if SCREENSAVER
MODULE_SRCS  += $(SCREENSAVER_SRCS)
endif

# XF86 Big Font extension
BIGFONT_SRCS = xf86bigfont.c xf86bigfontsrv.h
if XF86BIGFONT
BUILTIN_SRCS += $(BIGFONT_SRCS)
endif

# DPMS extension
DPMS_SRCS = dpms.c dpmsproc.h
if DPMSExtension
MODULE_SRCS += $(DPMS_SRCS)
endif

# Now take all of the above, mix well, bake for 10 minutes and get libXext*.la

libXext_la_SOURCES =		$(BUILTIN_SRCS) $(MODULE_SRCS)

EXTRA_DIST = \
	$(MITSHM_SRCS) \
	$(RES_SRCS) \
	$(SCREENSAVER_SRCS) \
	$(BIGFONT_SRCS) \
	$(DPMS_SRCS)

