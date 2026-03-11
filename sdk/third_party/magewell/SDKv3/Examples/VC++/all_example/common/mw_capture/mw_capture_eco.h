#pragma once
#include "LibMWCapture\MWCapture.h"
#include "ring_buffer\ring_buffer.h"
#include "mw_capture.h"
class CMWCaptureEco:public CMWCapture {
public:
    CMWCaptureEco();
    virtual ~CMWCaptureEco();
    virtual bool start_capture(bool capture_video, bool capture_audio);
	virtual bool set_mirror_and_reverse(bool is_mirror, bool is_reverse);
    DWORD eco_capture();
    DWORD audio_capture();
private:
    bool check();
    bool                            m_video_capturing;
    bool                            m_audio_capturing;
    HANDLE	m_video_thread;
    HANDLE	m_audio_thread;
};