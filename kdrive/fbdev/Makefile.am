AM_CPPFLAGS = 					\
	@KDRIVE_INCS@				\
	@KDRIVE_CFLAGS@

noinst_LIBRARIES = libfbdev.a

bin_PROGRAMS = Xfbdev

libfbdev_a_SOURCES =	\
	fbdev.c		\
	fbdev.h

Xfbdev_SOURCES = \
	fbinit.c

Xfbdev_LDADD = 						\
	libfbdev.a					\
	@KDRIVE_LIBS@					\
	@XSERVER_LIBS@					

Xfbdev_DEPENDENCIES =					\
	libfbdev.a @KDRIVE_LIBS@

install-exec-hook:
if INSTALL_SETUID
	chown root $(DESTDIR)$(bindir)/Xfbdev
	chmod u+s $(DESTDIR)$(bindir)/Xfbdev
endif
