// clang-format off
// SPDX-FileCopyrightText: © 2019-2020, 2022 Alexandros Theodotou <alex@zrythm.org>
// SPDX-License-Identifier: LicenseRef-ZrythmLicense
// clang-format on

/**
 * \file
 *
 * Cairo utilities.
 */

#ifndef __UTILS_CAIRO_H__
#define __UTILS_CAIRO_H__

#include "gtk_wrapper.h"

typedef struct Dictionary Dictionary;

/**
 * @addtogroup utils
 *
 * @{
 */

/**
 * Caches for cairo.
 */
typedef struct CairoCaches
{
  /**
   * Icon surface dictionary:
   *   icon name: cairo_surface_t
   */
  Dictionary * icon_surface_dict;
} CairoCaches;

/**
 * Default font for drawing pango text.
 */
#define Z_CAIRO_FONT "Bold 9"

/**
 * Padding to leave from the top/left edges when
 * drawing text.
 */
#define Z_CAIRO_TEXT_PADDING 2

#if 0
void
z_cairo_draw_selection_with_color (
  cairo_t * cr,
  GdkRGBA * color,
  double    start_x,
  double    start_y,
  double    offset_x,
  double    offset_y);

void
z_cairo_draw_selection (
  cairo_t * cr,
  double    start_x,
  double    start_y,
  double    offset_x,
  double    offset_y);
#endif

void
z_cairo_draw_horizontal_line (
  cairo_t * cr,
  double    y,
  double    from_x,
  double    to_x,
  double    line_width,
  double    alpha);

void
z_cairo_draw_vertical_line (
  cairo_t * cr,
  double    x,
  double    from_y,
  double    to_y,
  double    line_width);

/**
 * @param aspect Aspect ratio.
 * @param corner_radius Corner curvature radius.
 */
static inline void
z_cairo_rounded_rectangle (
  cairo_t * cr,
  double    x,
  double    y,
  double    width,
  double    height,
  double    aspect,
  double    corner_radius)
{
  double radius = corner_radius / aspect;
  double degrees = G_PI / 180.0;

  cairo_new_sub_path (cr);
  cairo_arc (
    cr, x + width - radius, y + radius, radius, -90 * degrees, 0 * degrees);
  cairo_arc (
    cr, x + width - radius, y + height - radius, radius, 0 * degrees,
    90 * degrees);
  cairo_arc (
    cr, x + radius, y + height - radius, radius, 90 * degrees, 180 * degrees);
  cairo_arc (cr, x + radius, y + radius, radius, 180 * degrees, 270 * degrees);
  cairo_close_path (cr);
}

#define z_cairo_get_text_extents_for_widget( \
  _widget, _layout, _text, _width, _height) \
  _z_cairo_get_text_extents_for_widget ( \
    (GtkWidget *) _widget, _layout, _text, _width, _height)

/**
 * Gets the width of the given text in pixels
 * for the given widget.
 *
 * @param widget The widget to derive a PangoLayout
 *   from.
 * @param text The text to draw.
 * @param width The width to fill in.
 * @param height The height to fill in.
 */
void
_z_cairo_get_text_extents_for_widget (
  GtkWidget *   widget,
  PangoLayout * layout,
  const char *  text,
  int *         width,
  int *         height);

/**
 * Draw text with default padding.
 */
#define z_cairo_draw_text(cr, widget, layout, text) \
  z_cairo_draw_text_full ( \
    cr, widget, layout, text, Z_CAIRO_TEXT_PADDING, Z_CAIRO_TEXT_PADDING)

/**
 * Draws the given text using the given font
 * starting at the given position.
 */
void
z_cairo_draw_text_full (
  cairo_t *     cr,
  GtkWidget *   widget,
  PangoLayout * layout,
  const char *  text,
  int           start_x,
  int           start_y);

/**
 * Draws a diamond shape.
 */
static inline void
z_cairo_diamond (cairo_t * cr, double x, double y, double width, double height)
{
  cairo_move_to (cr, x, height / 2);
  cairo_line_to (cr, width / 2, y);
  cairo_line_to (cr, width, height / 2);
  cairo_line_to (cr, width / 2, height);
  cairo_line_to (cr, x, height / 2);
  cairo_close_path (cr);
}

/**
 * Returns a surface for the icon name.
 */
cairo_surface_t *
z_cairo_get_surface_from_icon_name (const char * icon_name, int size, int scale);

/**
 * Creates a PangoLayout to be cached in widgets
 * based on the given settings.
 */
PangoLayout *
z_cairo_create_pango_layout_from_string (
  GtkWidget *        widget,
  const char *       font,
  PangoEllipsizeMode ellipsize_mode,
  int                ellipsize_padding);

/**
 * Creates a PangoLayout to be cached in widgets
 * based on the given settings.
 */
PangoLayout *
z_cairo_create_pango_layout_from_description (
  GtkWidget *            widget,
  PangoFontDescription * descr,
  PangoEllipsizeMode     ellipsize_mode,
  int                    ellipsize_padding);

/**
 * Creates a PangoLayout with default settings.
 */
PangoLayout *
z_cairo_create_default_pango_layout (GtkWidget * widget);

/**
 * Resets a surface and cairo_t with a new surface
 * and cairo_t based on the given rectangle and
 * cairo_t.
 *
 * To be used inside draw calls of widgets that
 * use caching.
 *
 * @param width New surface width.
 * @param height New surface height.
 */
void
z_cairo_reset_caches (
  cairo_t **         cr_cache,
  cairo_surface_t ** surface_cache,
  int                width,
  int                height,
  cairo_t *          new_cr);

CairoCaches *
z_cairo_caches_new (void);

void
z_cairo_caches_free (CairoCaches * self);

/**
 * @}
 */
GdkPixbuf *
z_gdk_pixbuf_new_from_icon_name (
  const char * icon_name,
  int          width,
  int          height,
  int          scale,
  GError **    error) ;
  
  GtkIconTheme *
z_gtk_icon_theme_get_default (void) ;

GdkTexture *
z_gdk_texture_new_from_icon_name (
  const char * icon_name,
  int          width,
  int          height,
  int          scale) ;
#endif
