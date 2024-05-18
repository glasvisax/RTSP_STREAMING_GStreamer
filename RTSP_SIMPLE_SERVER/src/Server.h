#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>

#pragma once 
// Колбэк для обработки активного SSRC
void ssrc_active_cb(GObject* session, GObject* source, GstRTSPMedia* media);

// Колбэк для обработки активного SSRC отправителя
void sender_ssrc_active_cb(GObject* session, GObject* source, GstRTSPMedia* media);

// Колбэк для обработки события готовности медиа
void media_prepared_cb(GstRTSPMedia* media);

// Колбэк для обработки конфигурации медиа
void media_configure_cb(GstRTSPMediaFactory* factory, GstRTSPMedia* media);

