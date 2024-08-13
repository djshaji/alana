// SPDX-FileCopyrightText: © 2024 Alexandros Theodotou <alex@zrythm.org>
// SPDX-License-Identifier: CC0-1.0

#include <gtk/gtk.h>

#define HAVE_X11
#ifdef HAVE_X11
/* hack to drop dumb typedefs and macros defined by X11 */
#  define Region RegionForX11
#  define None NoneForX11
#  include <gdk/x11/gdkx.h>
#  undef Region
#  undef None
#endif

#ifdef _WIN32
#  include <gdk/win32/gdkwin32.h>
#endif
