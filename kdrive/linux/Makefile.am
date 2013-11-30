AM_CPPFLAGS = 					\
	@KDRIVE_INCS@				\
	@KDRIVE_CFLAGS@

AM_CFLAGS = -DHAVE_DIX_CONFIG_H

noinst_LIBRARIES = liblinux.a

KDRIVE_HW_SOURCES =	\
	keyboard.c	\
	linux.c

liblinux_a_SOURCES = 	\
	mouse.c		\
	$(KDRIVE_HW_SOURCES)

liblinux_a_DEPENDENCIES = \
	keyboard.c	\
	linux.c		\
	mouse.c
