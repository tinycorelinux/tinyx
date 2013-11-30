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

#ifdef HAVE_CONFIG_H
#include <kdrive-config.h>
#endif
#include "kdrive.h"
#include "kkeymap.h"
#include <linux/keyboard.h>
#include <linux/kd.h>
#define XK_PUBLISHING
#include <X11/keysym.h>
#include <termios.h>
#include <sys/ioctl.h>

extern int LinuxConsoleFd;

static const KeySym linux_to_x[256] = {
	NoSymbol, NoSymbol, NoSymbol, NoSymbol,
	NoSymbol, NoSymbol, NoSymbol, NoSymbol,
	XK_BackSpace, XK_Tab, XK_Linefeed, NoSymbol,
	NoSymbol, NoSymbol, NoSymbol, NoSymbol,
	NoSymbol, NoSymbol, NoSymbol, NoSymbol,
	NoSymbol, NoSymbol, NoSymbol, NoSymbol,
	NoSymbol, NoSymbol, NoSymbol, XK_Escape,
	NoSymbol, NoSymbol, NoSymbol, NoSymbol,
	XK_space, XK_exclam, XK_quotedbl, XK_numbersign,
	XK_dollar, XK_percent, XK_ampersand, XK_apostrophe,
	XK_parenleft, XK_parenright, XK_asterisk, XK_plus,
	XK_comma, XK_minus, XK_period, XK_slash,
	XK_0, XK_1, XK_2, XK_3,
	XK_4, XK_5, XK_6, XK_7,
	XK_8, XK_9, XK_colon, XK_semicolon,
	XK_less, XK_equal, XK_greater, XK_question,
	XK_at, XK_A, XK_B, XK_C,
	XK_D, XK_E, XK_F, XK_G,
	XK_H, XK_I, XK_J, XK_K,
	XK_L, XK_M, XK_N, XK_O,
	XK_P, XK_Q, XK_R, XK_S,
	XK_T, XK_U, XK_V, XK_W,
	XK_X, XK_Y, XK_Z, XK_bracketleft,
	XK_backslash, XK_bracketright, XK_asciicircum, XK_underscore,
	XK_grave, XK_a, XK_b, XK_c,
	XK_d, XK_e, XK_f, XK_g,
	XK_h, XK_i, XK_j, XK_k,
	XK_l, XK_m, XK_n, XK_o,
	XK_p, XK_q, XK_r, XK_s,
	XK_t, XK_u, XK_v, XK_w,
	XK_x, XK_y, XK_z, XK_braceleft,
	XK_bar, XK_braceright, XK_asciitilde, XK_BackSpace,
	NoSymbol, NoSymbol, NoSymbol, NoSymbol,
	NoSymbol, NoSymbol, NoSymbol, NoSymbol,
	NoSymbol, NoSymbol, NoSymbol, NoSymbol,
	NoSymbol, NoSymbol, NoSymbol, NoSymbol,
	NoSymbol, NoSymbol, NoSymbol, NoSymbol,
	NoSymbol, NoSymbol, NoSymbol, NoSymbol,
	NoSymbol, NoSymbol, NoSymbol, NoSymbol,
	NoSymbol, NoSymbol, NoSymbol, NoSymbol,
	XK_nobreakspace, XK_exclamdown, XK_cent, XK_sterling,
	XK_currency, XK_yen, XK_brokenbar, XK_section,
	XK_diaeresis, XK_copyright, XK_ordfeminine, XK_guillemotleft,
	XK_notsign, XK_hyphen, XK_registered, XK_macron,
	XK_degree, XK_plusminus, XK_twosuperior, XK_threesuperior,
	XK_acute, XK_mu, XK_paragraph, XK_periodcentered,
	XK_cedilla, XK_onesuperior, XK_masculine, XK_guillemotright,
	XK_onequarter, XK_onehalf, XK_threequarters, XK_questiondown,
	XK_Agrave, XK_Aacute, XK_Acircumflex, XK_Atilde,
	XK_Adiaeresis, XK_Aring, XK_AE, XK_Ccedilla,
	XK_Egrave, XK_Eacute, XK_Ecircumflex, XK_Ediaeresis,
	XK_Igrave, XK_Iacute, XK_Icircumflex, XK_Idiaeresis,
	XK_ETH, XK_Ntilde, XK_Ograve, XK_Oacute,
	XK_Ocircumflex, XK_Otilde, XK_Odiaeresis, XK_multiply,
	XK_Ooblique, XK_Ugrave, XK_Uacute, XK_Ucircumflex,
	XK_Udiaeresis, XK_Yacute, XK_THORN, XK_ssharp,
	XK_agrave, XK_aacute, XK_acircumflex, XK_atilde,
	XK_adiaeresis, XK_aring, XK_ae, XK_ccedilla,
	XK_egrave, XK_eacute, XK_ecircumflex, XK_ediaeresis,
	XK_igrave, XK_iacute, XK_icircumflex, XK_idiaeresis,
	XK_eth, XK_ntilde, XK_ograve, XK_oacute,
	XK_ocircumflex, XK_otilde, XK_odiaeresis, XK_division,
	XK_oslash, XK_ugrave, XK_uacute, XK_ucircumflex,
	XK_udiaeresis, XK_yacute, XK_thorn, XK_ydiaeresis
};

static unsigned char tbl[KD_MAX_WIDTH] = {
	0,
	1 << KG_SHIFT,
	(1 << KG_ALTGR),
	(1 << KG_ALTGR) | (1 << KG_SHIFT)
};

static void readKernelMapping(void)
{
	KeySym *k;
	int i, j;
	struct kbentry kbe;
	int minKeyCode, maxKeyCode;
	int row;

	minKeyCode = NR_KEYS;
	maxKeyCode = 0;
	row = 0;
	for (i = 0; i < NR_KEYS && row < KD_MAX_LENGTH; ++i) {
		kbe.kb_index = i;

		k = kdKeymap + row * KD_MAX_WIDTH;

		for (j = 0; j < KD_MAX_WIDTH; ++j) {
			unsigned short kval;

			k[j] = NoSymbol;

			kbe.kb_table = tbl[j];
			kbe.kb_value = 0;
			if (ioctl(LinuxConsoleFd, KDGKBENT, &kbe))
				continue;

			kval = KVAL(kbe.kb_value);
			switch (KTYP(kbe.kb_value)) {
			case KT_LATIN:
			case KT_LETTER:
				k[j] = linux_to_x[kval];
				break;

			case KT_FN:
				if (kval <= 19)
					k[j] = XK_F1 + kval;
				else
					switch (kbe.kb_value) {
					case K_FIND:
						k[j] = XK_Home;	/* or XK_Find */
						break;
					case K_INSERT:
						k[j] = XK_Insert;
						break;
					case K_REMOVE:
						k[j] = XK_Delete;
						break;
					case K_SELECT:
						k[j] = XK_End;	/* or XK_Select */
						break;
					case K_PGUP:
						k[j] = XK_Prior;
						break;
					case K_PGDN:
						k[j] = XK_Next;
						break;
					case K_HELP:
						k[j] = XK_Help;
						break;
					case K_DO:
						k[j] = XK_Execute;
						break;
					case K_PAUSE:
						k[j] = XK_Pause;
						break;
					case K_MACRO:
						k[j] = XK_Menu;
						break;
					default:
						break;
					}
				break;

			case KT_SPEC:
				switch (kbe.kb_value) {
				case K_ENTER:
					k[j] = XK_Return;
					break;
				case K_BREAK:
					k[j] = XK_Break;
					break;
				case K_CAPS:
					k[j] = XK_Caps_Lock;
					break;
				case K_NUM:
					k[j] = XK_Num_Lock;
					break;
				case K_HOLD:
					k[j] = XK_Scroll_Lock;
					break;
				case K_COMPOSE:
					k[j] = XK_Multi_key;
					break;
				default:
					break;
				}
				break;

			case KT_PAD:
				switch (kbe.kb_value) {
				case K_PPLUS:
					k[j] = XK_KP_Add;
					break;
				case K_PMINUS:
					k[j] = XK_KP_Subtract;
					break;
				case K_PSTAR:
					k[j] = XK_KP_Multiply;
					break;
				case K_PSLASH:
					k[j] = XK_KP_Divide;
					break;
				case K_PENTER:
					k[j] = XK_KP_Enter;
					break;
				case K_PCOMMA:
					k[j] = XK_KP_Separator;
					break;
				case K_PDOT:
					k[j] = XK_KP_Decimal;
					break;
				case K_PPLUSMINUS:
					k[j] = XK_KP_Subtract;
					break;
				default:
					if (kval <= 9)
						k[j] = XK_KP_0 + kval;
					break;
				}
				break;

				/*
				 * KT_DEAD keys are for accelerated diacritical creation.
				 */
			case KT_DEAD:
				switch (kbe.kb_value) {
				case K_DGRAVE:
					k[j] = XK_dead_grave;
					break;
				case K_DACUTE:
					k[j] = XK_dead_acute;
					break;
				case K_DCIRCM:
					k[j] = XK_dead_circumflex;
					break;
				case K_DTILDE:
					k[j] = XK_dead_tilde;
					break;
				case K_DDIERE:
					k[j] = XK_dead_diaeresis;
					break;
				}
				break;

			case KT_CUR:
				switch (kbe.kb_value) {
				case K_DOWN:
					k[j] = XK_Down;
					break;
				case K_LEFT:
					k[j] = XK_Left;
					break;
				case K_RIGHT:
					k[j] = XK_Right;
					break;
				case K_UP:
					k[j] = XK_Up;
					break;
				}
				break;

			case KT_SHIFT:
				switch (kbe.kb_value) {
				case K_ALTGR:
					k[j] = XK_Mode_switch;
					break;
				case K_ALT:
					k[j] = (kbe.kb_index == 0x64 ?
						XK_Alt_R : XK_Alt_L);
					break;
				case K_CTRL:
					k[j] = (kbe.kb_index == 0x61 ?
						XK_Control_R : XK_Control_L);
					break;
				case K_CTRLL:
					k[j] = XK_Control_L;
					break;
				case K_CTRLR:
					k[j] = XK_Control_R;
					break;
				case K_SHIFT:
					k[j] = (kbe.kb_index == 0x36 ?
						XK_Shift_R : XK_Shift_L);
					break;
				case K_SHIFTL:
					k[j] = XK_Shift_L;
					break;
				case K_SHIFTR:
					k[j] = XK_Shift_R;
					break;
				default:
					break;
				}
				break;

				/*
				 * KT_ASCII keys accumulate a 3 digit decimal number that gets
				 * emitted when the shift state changes. We can't emulate that.
				 */
			case KT_ASCII:
				break;

			case KT_LOCK:
				if (kbe.kb_value == K_SHIFTLOCK)
					k[j] = XK_Shift_Lock;
				break;

#ifdef KT_X
			case KT_X:
				/* depends on new keyboard symbols in file linux/keyboard.h */
				if (kbe.kb_value == K_XMENU)
					k[j] = XK_Menu;
				if (kbe.kb_value == K_XTELEPHONE)
					k[j] = XK_telephone;
				break;
#endif
#ifdef KT_XF
			case KT_XF:
				/* special linux keysyms which map directly to XF86 keysyms */
				k[j] = (kbe.kb_value & 0xFF) + 0x1008FF00;
				break;
#endif

			default:
				break;
			}
			if (i < minKeyCode)
				minKeyCode = i;
			if (i > maxKeyCode)
				maxKeyCode = i;
		}

		if (minKeyCode == NR_KEYS)
			continue;

		if (k[3] == k[2])
			k[3] = NoSymbol;
		if (k[2] == k[1])
			k[2] = NoSymbol;
		if (k[1] == k[0])
			k[1] = NoSymbol;
		if (k[0] == k[2] && k[1] == k[3])
			k[2] = k[3] = NoSymbol;
		if (k[3] == k[0] && k[2] == k[1] && k[2] == NoSymbol)
			k[3] = NoSymbol;
		row++;
	}
	kdMinScanCode = minKeyCode;
	kdMaxScanCode = maxKeyCode;
}

static void LinuxKeyboardLoad(void)
{
	readKernelMapping();
}

static void LinuxKeyboardRead(int fd, void *closure)
{
	unsigned char buf[256], *b;
	int n;

	while ((n = read(fd, buf, sizeof(buf))) > 0) {
		b = buf;
		while (n--) {
			KdEnqueueKeyboardEvent(b[0] & 0x7f, b[0] & 0x80);
			b++;
		}
	}
}

static int LinuxKbdTrans;
static struct termios LinuxTermios;
static int LinuxKbdType;

static int LinuxKeyboardEnable(int fd, void *closure)
{
	struct termios nTty;
	unsigned char buf[256];
	int n;

	ioctl(fd, KDGKBMODE, &LinuxKbdTrans);
	tcgetattr(fd, &LinuxTermios);

	ioctl(fd, KDSKBMODE, K_MEDIUMRAW);
	nTty = LinuxTermios;
	nTty.c_iflag = (IGNPAR | IGNBRK) & (~PARMRK) & (~ISTRIP);
	nTty.c_oflag = 0;
	nTty.c_cflag = CREAD | CS8;
	nTty.c_lflag = 0;
	nTty.c_cc[VTIME] = 0;
	nTty.c_cc[VMIN] = 1;
	cfsetispeed(&nTty, 9600);
	cfsetospeed(&nTty, 9600);
	tcsetattr(fd, TCSANOW, &nTty);
	/*
	 * Flush any pending keystrokes
	 */
	while ((n = read(fd, buf, sizeof(buf))) > 0) ;
	return fd;
}

static void LinuxKeyboardDisable(int fd, void *closure)
{
	ioctl(LinuxConsoleFd, KDSKBMODE, LinuxKbdTrans);
	tcsetattr(LinuxConsoleFd, TCSANOW, &LinuxTermios);
}

static int LinuxKeyboardInit(void)
{
	if (!LinuxKbdType)
		LinuxKbdType = KdAllocInputType();

	KdRegisterFd(LinuxKbdType, LinuxConsoleFd, LinuxKeyboardRead, 0);
	LinuxKeyboardEnable(LinuxConsoleFd, 0);
	KdRegisterFdEnableDisable(LinuxConsoleFd,
				  LinuxKeyboardEnable, LinuxKeyboardDisable);
	return 1;
}

static void LinuxKeyboardFini(void)
{
	LinuxKeyboardDisable(LinuxConsoleFd, 0);
	KdUnregisterFds(LinuxKbdType, FALSE);
}

static void LinuxKeyboardLeds(int leds)
{
	ioctl(LinuxConsoleFd, KDSETLED, leds & 7);
}

static void LinuxKeyboardBell(int volume, int pitch, int duration)
{
	if (volume && pitch) {
		ioctl(LinuxConsoleFd, KDMKTONE,
		      ((1193190 / pitch) & 0xffff) |
		      (((unsigned long)duration * volume / 50) << 16));

	}
}

const KdKeyboardFuncs LinuxKeyboardFuncs = {
	LinuxKeyboardLoad,
	LinuxKeyboardInit,
	LinuxKeyboardLeds,
	LinuxKeyboardBell,
	LinuxKeyboardFini,
	3,
};
