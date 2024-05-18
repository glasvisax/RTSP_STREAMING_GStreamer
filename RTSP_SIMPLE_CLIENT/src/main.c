#include <string.h>
#include <gst/video/videooverlay.h>
#include <gdk/gdk.h>
#include <gdk/gdkwin32.h>
#include <assert.h>
#include <windows.h>
#include "Client.h"

static void realize_cb(GtkWidget* widget, CustomData* data)
{
    GdkWindow* window = gtk_widget_get_window(widget);
    guintptr window_handle;

    if (!gdk_window_ensure_native(window))
        g_error("Couldn't create native window needed for GstXOverlay!");

    window_handle = (guintptr)GDK_WINDOW_HWND(window);

    gst_video_overlay_set_window_handle(GST_VIDEO_OVERLAY(data->playbin), window_handle);
}

static void play_cb(GtkButton* button, CustomData* data)
{
    gst_element_set_state(data->playbin, GST_STATE_PLAYING);
}

static void pause_cb(GtkButton* button, CustomData* data)
{
    gst_element_set_state(data->playbin, GST_STATE_PAUSED);
}

static void stop_cb(GtkButton* button, CustomData* data)
{
    gst_element_set_state(data->playbin, GST_STATE_READY);
}

static void delete_event_cb(GtkWidget* widget, GdkEvent* event, CustomData* data)
{
    stop_cb(NULL, data);
    gtk_main_quit();
}

static gboolean draw_cb(GtkWidget* widget, cairo_t* cr, CustomData* data)
{
    if (data->state < GST_STATE_PAUSED) {
        GtkAllocation allocation;

        gtk_widget_get_allocation(widget, &allocation);
        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_rectangle(cr, 0, 0, allocation.width, allocation.height);
        cairo_fill(cr);
    }

    return FALSE;
}

static void slider_cb(GtkRange* range, CustomData* data)
{
    gdouble value = gtk_range_get_value(GTK_RANGE(data->slider));
    gst_element_seek_simple(data->playbin, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT,
        (gint64)(value * GST_SECOND));
}

static void create_ui(CustomData* data)
{
    GtkWidget* main_window;  
    GtkWidget* video_window;
    GtkWidget* main_box;     
    GtkWidget* controls;     
    GtkWidget* play_button, * pause_button, * stop_button; 

    main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(G_OBJECT(main_window), "delete-event", G_CALLBACK(delete_event_cb), data);

    video_window = gtk_drawing_area_new();
    gtk_widget_set_double_buffered(video_window, FALSE);
    g_signal_connect(video_window, "realize", G_CALLBACK(realize_cb), data);
    g_signal_connect(video_window, "draw", G_CALLBACK(draw_cb), data);

    play_button = gtk_button_new_from_icon_name("media-playback-start", GTK_ICON_SIZE_SMALL_TOOLBAR);
    g_signal_connect(G_OBJECT(play_button), "clicked", G_CALLBACK(play_cb), data);

    pause_button = gtk_button_new_from_icon_name("media-playback-pause", GTK_ICON_SIZE_SMALL_TOOLBAR);
    g_signal_connect(G_OBJECT(pause_button), "clicked", G_CALLBACK(pause_cb), data);

    stop_button = gtk_button_new_from_icon_name("media-playback-stop", GTK_ICON_SIZE_SMALL_TOOLBAR);
    g_signal_connect(G_OBJECT(stop_button), "clicked", G_CALLBACK(stop_cb), data);

    data->slider = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 100, 0.5);
    gtk_scale_set_draw_value(GTK_SCALE(data->slider), 0);
    data->slider_update_signal_id = g_signal_connect(G_OBJECT(data->slider), "value-changed", G_CALLBACK(slider_cb), data);

    controls = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5); 
    gtk_box_pack_start(GTK_BOX(controls), play_button, FALSE, FALSE, 2);
    gtk_box_pack_start(GTK_BOX(controls), pause_button, FALSE, FALSE, 2);
    gtk_box_pack_start(GTK_BOX(controls), stop_button, FALSE, FALSE, 2);
    gtk_box_pack_start(GTK_BOX(controls), data->slider, TRUE, TRUE, 2);

    main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5); 
    gtk_box_pack_start(GTK_BOX(main_box), video_window, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(main_box), controls, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(main_window), main_box);
    gtk_window_set_default_size(GTK_WINDOW(main_window), 1024, 768);

    gtk_widget_show_all(main_window);
    gtk_window_set_resizable(GTK_WINDOW(main_window), FALSE);
    gtk_window_set_title(GTK_WINDOW(main_window), "Stream App");
}


int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,int nShowCmd) 
{
    CustomData data;
    GstStateChangeReturn ret;
    GstBus* bus;
    guint flags;
    int argc;
    char* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    
    if (argc < 2) {
        assert(FALSE && "To use need to obtain rtsp address");
    }

    gtk_init(&argc, &argv);
    gst_init(&argc, &argv);

    data.duration = GST_CLOCK_TIME_NONE;
    data.playbin = gst_element_factory_make("playbin", "playbin");

    if (!data.playbin) {
        g_printerr("Not all elements could be created.\n");
        return -1;
    }

    g_object_get(data.playbin, "flags", &flags, NULL);
    flags |= GST_PLAY_FLAG_DOWNLOAD;
    g_object_set(data.playbin, "flags", flags, NULL);

    g_object_set(data.playbin, "uri", "rtsp://127.0.0.1:8554/stream", NULL);

    g_signal_connect(G_OBJECT(data.playbin), "video-tags-changed", (GCallback)tags_cb, &data);
    g_signal_connect(G_OBJECT(data.playbin), "audio-tags-changed", (GCallback)tags_cb, &data);
    g_signal_connect(G_OBJECT(data.playbin), "text-tags-changed", (GCallback)tags_cb, &data);

    create_ui(&data);

    bus = gst_element_get_bus(data.playbin);
    gst_bus_add_signal_watch(bus);
    g_signal_connect(G_OBJECT(bus), "message::error", (GCallback)error_cb, &data);
    g_signal_connect(G_OBJECT(bus), "message::eos", (GCallback)eos_cb, &data);
    g_signal_connect(G_OBJECT(bus), "message::state-changed", (GCallback)state_changed_cb, &data);
    g_signal_connect(G_OBJECT(bus), "message::buffering", (GCallback)buffering_cb, &data);
    //g_signal_connect(data.playbin, "deep-notify::temp-location", (GCallback)got_location, &data);

    gst_object_unref(bus);

    ret = gst_element_set_state(data.playbin, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr("Unable to set the pipeline to the playing state.\n");
        gst_object_unref(data.playbin);
        return -1;
    } 

    g_timeout_add_seconds(1, (GSourceFunc)refresh_ui, &data);

    gtk_main();

    gst_element_set_state(data.playbin, GST_STATE_NULL);
    gst_object_unref(data.playbin);

    return 0;
}