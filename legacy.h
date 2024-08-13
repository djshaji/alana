#ifndef LEGACY_H
#define LEGACY_H
/*  ayyo    */
#include <gtk/gtk.h>
#define GdkWindow GdkSurface

struct GdkEventExpose {
  GdkEventType type;
  GdkWindow* window;
  gint8 send_event;
  GdkRectangle area;
  cairo_region_t* region;
  gint count;
};

struct GdkEventKey {
  GdkEventType type;
  GdkWindow* window;
  gint8 send_event;
  guint32 time;
  GdkModifierType* state;
  guint keyval;
  gint length;
  gchar* string;
  guint16 hardware_keycode;
  guint8 group;
  guint is_modifier : 1;
};

struct GdkEventButton {
  GdkEventType type;
  GdkWindow* window;
  gint8 send_event;
  guint32 time;
  gdouble x;
  gdouble y;
  gdouble* axes;
  GdkModifierType* state;
  guint button;
  GdkDevice* device;
  gdouble x_root;
  gdouble y_root;
};

struct GdkEventScroll {
  GdkEventType type;
  GdkWindow* window;
  gint8 send_event;
  guint32 time;
  gdouble x;
  gdouble y;
  GdkModifierType* state;
  GdkScrollDirection direction;
  GdkDevice* device;
  gdouble x_root;
  gdouble y_root;
  gdouble delta_x;
  gdouble delta_y;
  guint is_stop : 1;
} ;

struct GdkEventCrossing {
  GdkEventType type;
  GdkWindow* window;
  gint8 send_event;
  GdkWindow* subwindow;
  guint32 time;
  gdouble x;
  gdouble y;
  gdouble x_root;
  gdouble y_root;
  GdkCrossingMode mode;
  GdkNotifyType detail;
  gboolean focus;
  GdkModifierType* state;
};

struct GdkEventMotion {
  GdkEventType type;
  GdkWindow* window;
  gint8 send_event;
  guint32 time;
  gdouble x;
  gdouble y;
  gdouble* axes;
  GdkModifierType* state;
  gint16 is_hint;
  GdkDevice* device;
  gdouble x_root;
  gdouble y_root;
} ;

#endif // LEGACY_H
