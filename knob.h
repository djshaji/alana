// SPDX-FileCopyrightText: © 2019-2022 Alexandros Theodotou <alex@zrythm.org>
// SPDX-License-Identifier: LicenseRef-ZrythmLicense

/**
 * \file
 *
 * Basic knob widget, taken from Ardour.
 */

#ifndef __GUI_WIDGETS_KNOB_H__
#define __GUI_WIDGETS_KNOB_H__

#include <gtk/gtk.h>
#include "types.h"

#define KNOB_WIDGET_TYPE (knob_widget_get_type ())
G_DECLARE_FINAL_TYPE (KnobWidget, knob_widget, Z, KNOB_WIDGET, GtkWidget)

/**
 * @addtogroup widgets
 *
 * @{
 */

/**
 * Type of knob.
 */
enum class KnobType
{
  KNOB_TYPE_NORMAL,

  /** Port connection multiplier. */
  KNOB_TYPE_PORT_MULTIPLIER,
};

typedef struct Port Port;

typedef struct _KnobWidget
{
  GtkWidget parent_instance;

  KnobType type;

  /** Getter for the actual value. */
  GenericFloatGetter getter;

  /** Getter for the default actual value. */
  GenericFloatGetter default_getter;

  /** Getter for a snapped value (used if
   * passed). */
  GenericFloatGetter snapped_getter;

  /** Setter. */
  GenericFloatSetter setter;

  /** Float setter for drag begin. */
  GenericFloatSetter init_setter;

  /** Float setter for drag end. */
  GenericFloatSetter end_setter;

  /** Value to show when hovering (optional). */
  GenericStringCopyGetter hover_str_getter;

  /** Object to call get/set with. */
  void * object;

  /** Size in px. */
  int size;
  /** Whether hovering or not. */
  bool hover;
  /** Zero point 0.0-1.0. */
  float zero;
  /** Draw arc around the knob or not. */
  bool arc;
  /** Bevel. */
  int bevel;
  /** Flat or 3D. */
  bool flat;
  /** Min value (eg, 1). */
  float min;
  /** Max value (eg, 180). */
  float max;

  /** Color away from zero point. */
  GdkRGBA start_color;
  /** Color closer to zero point. */
  GdkRGBA end_color;

  /** Used in drag gesture. */
  GtkGestureDrag * drag;
  /** Used in gesture drag. */
  double last_x;
  /** Used in gesture drag. */
  double last_y;

  bool drag_updated;

  PangoLayout * layout;

  /** Last drawn real val. */
  float last_real_val;

  /** To be optionally used for context menus. */
  GtkPopoverMenu * popover_menu;
} KnobWidget;

/**
 * Creates a knob widget with the given options and
 * binds it to the given value.
 *
 * @param get_val Getter function.
 * @param get_default_val Getter function for
 *   default value.
 * @param set_val Setter function.
 * @param object Object to call get/set with.
 * @param dest Port destination multiplier index, if
 *   type is Port, otherwise ignored.
 */
KnobWidget *
_knob_widget_new (
  GenericFloatGetter get_val,
  GenericFloatGetter get_default_val,
  GenericFloatSetter set_val,
  void *             object,
  KnobType           type,
  float              min,
  float              max,
  int                size,
  float              zero);

#define knob_widget_new_simple( \
  getter, default_getter, setter, obj, min, max, size, zero) \
  _knob_widget_new ( \
    (GenericFloatGetter) getter, (GenericFloatGetter) default_getter, \
    (GenericFloatSetter) setter, (void *) obj, KnobType::KNOB_TYPE_NORMAL, \
    min, max, size, zero)

/**
 * @param conn PortConnection pointer.
 */
#define knob_widget_new_port(conn, size) \
  _knob_widget_new ( \
    NULL, NULL, (void *) conn, KnobType::KNOB_TYPE_PORT_MULTIPLIER, 0.f, 1.f, \
    size, 0.f)

/**
 * @}
 */

#endif
