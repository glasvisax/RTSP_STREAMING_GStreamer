#include "Server.h"

void ssrc_active_cb(GObject* session, GObject* source, GstRTSPMedia* media)
{
    GstStructure* stats;
    g_object_get(source, "stats", &stats, NULL);
    if (stats) {
        gchar* sstr = gst_structure_to_string(stats);
        g_print("Source stats:\nStructure: %s\n", sstr);
        g_free(sstr);
        gst_structure_free(stats);
    }
}

void sender_ssrc_active_cb(GObject* session, GObject* source, GstRTSPMedia* media)
{
    GstStructure* stats;
    g_object_get(source, "stats", &stats, NULL);
    if (stats) {
        gchar* sstr = gst_structure_to_string(stats);
        g_print("Sender stats:\nStructure: %s\n", sstr);
        g_free(sstr);
        gst_structure_free(stats);
    }
}

void media_prepared_cb(GstRTSPMedia* media)
{
    guint n_streams = gst_rtsp_media_n_streams(media);
    for (guint i = 0; i < n_streams; i++) {
        GstRTSPStream* stream = gst_rtsp_media_get_stream(media, i);
        if (!stream) continue;
        GObject* session = gst_rtsp_stream_get_rtpsession(stream);
        g_signal_connect(session, "on-ssrc-active", (GCallback)ssrc_active_cb, media);
        g_signal_connect(session, "on-sender-ssrc-active", (GCallback)sender_ssrc_active_cb, media);
    }
}

void media_configure_cb(GstRTSPMediaFactory* factory, GstRTSPMedia* media)
{
    g_signal_connect(media, "prepared", (GCallback)media_prepared_cb, factory);
}