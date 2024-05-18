#include "Client.h"

off_t get_file_size(const char* filename)
{
    struct stat st;

    if (stat(filename, &st) == 0) {
        return st.st_size;
    }
    else {
        perror("stat");
        return -1;
    }
}

void tags_cb(GstElement* playbin, gint stream, CustomData* data)
{
    gst_element_post_message(playbin,
        gst_message_new_application(GST_OBJECT(playbin),
            gst_structure_new_empty("tags-changed")));
}

void error_cb(GstBus* bus, GstMessage* msg, CustomData* data)
{
    GError* err;
    gchar* debug_info;

    gst_message_parse_error(msg, &err, &debug_info);
    g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
    g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
    g_clear_error(&err);
    g_free(debug_info);

    gst_element_set_state(data->playbin, GST_STATE_READY);
}

void eos_cb(GstBus* bus, GstMessage* msg, CustomData* data)
{
    g_print("End-Of-Stream reached.\n");
    gst_element_set_state(data->playbin, GST_STATE_READY);
}

void state_changed_cb(GstBus* bus, GstMessage* msg, CustomData* data)
{
    GstState old_state, new_state, pending_state;
    gst_message_parse_state_changed(msg, &old_state, &new_state, &pending_state);
    if (GST_MESSAGE_SRC(msg) == GST_OBJECT(data->playbin)) {
        data->state = new_state;
        g_print("State set to %s\n", gst_element_state_get_name(new_state));
        if (old_state == GST_STATE_READY && new_state == GST_STATE_PAUSED) {
            refresh_ui(data);
        }
    }

}

void buffering_cb(GstBus* bus, GstMessage* msg, CustomData* data)
{
    gint percent = 0;

    gst_message_parse_buffering(msg, &percent);
    g_print("Buffering (%3d%%)\r", percent);

    if (percent < 100 && data->state != GST_STATE_PAUSED)
        gst_element_set_state(data->playbin, GST_STATE_PAUSED);
    else if (percent == 100 && data->state != GST_STATE_PLAYING)
        gst_element_set_state(data->playbin, GST_STATE_PLAYING);
}

void got_location(GstObject* gstobject, GstObject* prop_object, GParamSpec* prop, gpointer _, CustomData* data)
{
    gchar* loc;
    g_object_get(G_OBJECT(prop_object), "temp-location", &loc, NULL);
    g_print("Temporary File: %s \n", loc);
    g_free(loc);
}
gboolean refresh_ui(CustomData* data)
{
    gint64 current = -1;

    if (data->state < GST_STATE_PAUSED)
        return TRUE;

    if (!GST_CLOCK_TIME_IS_VALID(data->duration)) {
        if (!gst_element_query_duration(data->playbin, GST_FORMAT_TIME, &data->duration)) {
            g_printerr("Could not query current duration.\n");
        }
        else {
            gtk_range_set_range(GTK_RANGE(data->slider), 0, (gdouble)data->duration / GST_SECOND);
        }
    }

    if (gst_element_query_position(data->playbin, GST_FORMAT_TIME, &current)) {
        g_signal_handler_block(data->slider, data->slider_update_signal_id);
        gtk_range_set_value(GTK_RANGE(data->slider), (gdouble)current / GST_SECOND);
        g_signal_handler_unblock(data->slider, data->slider_update_signal_id);
    }

    return TRUE;
}