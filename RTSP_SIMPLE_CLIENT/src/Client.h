#pragma once 
#include <sys/types.h>
#include <sys/stat.h>
#include <gst/gst.h>
#include <gtk/gtk.h>

typedef enum {
    GST_PLAY_FLAG_DOWNLOAD = (1 << 7)
} GstPlayFlags;

typedef struct _CustomData {
    GstElement* playbin;

    GtkWidget* slider;
    gulong slider_update_signal_id;

    GstState state;
    gint64 duration;

} CustomData;

off_t get_file_size(const char* filename);

void tags_cb(GstElement* playbin, gint stream, CustomData* data);

void error_cb(GstBus* bus, GstMessage* msg, CustomData* data);

void eos_cb(GstBus* bus, GstMessage* msg, CustomData* data);

void state_changed_cb(GstBus* bus, GstMessage* msg, CustomData* data);

void buffering_cb(GstBus* bus, GstMessage* msg, CustomData* data);

void got_location(GstObject* gstobject, GstObject* prop_object, GParamSpec* prop, gpointer _, CustomData* data);

gboolean refresh_ui(CustomData* data);
