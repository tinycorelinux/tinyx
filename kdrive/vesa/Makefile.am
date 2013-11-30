AM_CPPFLAGS = 					\
	@KDRIVE_INCS@				\
	@KDRIVE_CFLAGS@

noinst_LIBRARIES = libvesa.a

bin_PROGRAMS = Xvesa

libvesa_a_SOURCES = \
	vesa.c		\
	vesa.h		\
	vbe.c		\
	vbe.h		\
	vga.c		\
	vga.h		\
	vm86.c		\
	vm86.h

Xvesa_SOURCES = \
	vesainit.c

Xvesa_LDADD = \
	libvesa.a 				\
	@KDRIVE_LIBS@				\
	@XSERVER_LIBS@

Xvesa_DEPENDENCIES = \
	libvesa.a @KDRIVE_LIBS@

install-exec-hook:
if INSTALL_SETUID
	chown root $(DESTDIR)$(bindir)/Xvesa
	chmod u+s $(DESTDIR)$(bindir)/Xvesa
endif
