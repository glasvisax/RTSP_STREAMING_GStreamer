#include "Server.h"
#include <assert.h>

static char* PORT = "8554";
static int LATENCY = 200;
static gboolean use_tcp = FALSE;

// Структура для хранения объектов, связанных с RTSP сервером
typedef struct _RTSPServerData {
    GMainLoop* loop;
    GstRTSPServer* server;
    GstRTSPMountPoints* mounts;
    GstRTSPMediaFactory* factory;
} RTSPServerData;

int main(int argc, char* argv[])
{

    if (argc == 3 && g_strcmp0(argv[2], "--tcp") == 0) {
        use_tcp = TRUE;
    }
    else {
        if (argc < 2) {
            g_printerr("Usage: %s <MP4 filename> [--tcp]\n", argv[0]);
            assert(FALSE);
        }
    }

    gst_init(&argc, &argv);
    gst_debug_set_default_threshold(GST_LEVEL_ERROR);

    // Создание и инициализация структуры RTSPServerData
    RTSPServerData server_data;
    server_data.loop = g_main_loop_new(NULL, FALSE);
    server_data.server = gst_rtsp_server_new();
    g_object_set(server_data.server, "service", PORT, NULL);
    server_data.mounts = gst_rtsp_server_get_mount_points(server_data.server);
    gchar* str = g_strdup_printf("( filesrc location=\"%s\" ! qtdemux name=d d. ! queue ! rtph264pay pt=96 name=pay0 d. ! queue ! rtpmp4apay pt=97 name=pay1 )", argv[1]);
    server_data.factory = gst_rtsp_media_factory_new();
    gst_rtsp_media_factory_set_launch(server_data.factory, str);

    // Установка задержки и указание часов
    GstClock* clock = gst_system_clock_obtain();
    gst_rtsp_media_factory_set_clock(server_data.factory, clock);
    gst_rtsp_media_factory_set_latency(server_data.factory, LATENCY);

    // Поддержка выбора протокола TCP/UDP
    if (use_tcp) {
        gst_rtsp_media_factory_set_protocols(server_data.factory, GST_RTSP_LOWER_TRANS_TCP);
    }

    g_signal_connect(server_data.factory, "media-configure", (GCallback)media_configure_cb, server_data.factory);
    gst_rtsp_mount_points_add_factory(server_data.mounts, "/stream", server_data.factory);
    ;
    gst_rtsp_server_attach(server_data.server, NULL);
    g_print("Stream ready at rtsp://127.0.0.1:%s/stream\n", PORT);

    g_main_loop_run(server_data.loop);

    // Освобождение ресурсов
    g_free(str);
    g_object_unref(server_data.mounts);
    g_main_loop_unref(server_data.loop);
    g_object_unref(server_data.factory);
    g_object_unref(server_data.server);

    return 0;
}