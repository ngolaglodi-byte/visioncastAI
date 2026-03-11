#pragma once
#include "LibMWCapture\MWCapture.h"
#include "ring_buffer\ring_buffer.h"
#include "mw_capture.h"
class CMWCaptureUsb : public CMWCapture {
public:
    CMWCaptureUsb();
    virtual ~CMWCaptureUsb();
    virtual bool start_capture(bool capture_video, bool capture_audio);
	virtual bool set_mirror_and_reverse(bool is_mirror, bool is_reverse);
private:
    HANDLE  m_video_handle;
    HANDLE  m_audio_handle;
private:
    bool check();
};