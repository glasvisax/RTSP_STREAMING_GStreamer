#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>

#pragma once 
// ������ ��� ��������� ��������� SSRC
void ssrc_active_cb(GObject* session, GObject* source, GstRTSPMedia* media);

// ������ ��� ��������� ��������� SSRC �����������
void sender_ssrc_active_cb(GObject* session, GObject* source, GstRTSPMedia* media);

// ������ ��� ��������� ������� ���������� �����
void media_prepared_cb(GstRTSPMedia* media);

// ������ ��� ��������� ������������ �����
void media_configure_cb(GstRTSPMediaFactory* factory, GstRTSPMedia* media);

