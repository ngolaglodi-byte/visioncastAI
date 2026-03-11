#pragma once
#include "LibMWCapture\MWCapture.h"
#include "ring_buffer\ring_buffer.h"
#include "mw_capture.h"
class CMWCapturePro : public CMWCapture {
public:
    CMWCapturePro();
    virtual ~CMWCapturePro();
    virtual bool start_capture(bool capture_video, bool capture_audio);
	virtual bool set_mirror_and_reverse(bool is_mirror, bool is_reverse);

    DWORD capture_by_input();
    DWORD capture_by_timer();
    DWORD audio_capture();
private:
    virtual void prev_frame_capture_process();
    bool check();
    bool                            m_video_capturing;
    bool                            m_audio_capturing;                  
    bool                            m_capture_video;
    bool                            m_capture_audio;

    BOOLEAN							m_bottom_up;//false

    DWORD							m_process_switchs;//0
    int								m_parital_notify;//0
    HOSD							m_OSD_image;//NULL
    RECT *					        m_p_OSD_rects;//NULL
    int								m_OSD_rects_num;//0
    SHORT							m_contrast;//100
    SHORT							m_brightness;//0
    SHORT							m_saturation;//100
    SHORT							m_hue;//0
    MWCAP_VIDEO_DEINTERLACE_MODE			m_deinterlace_mode;//MWCAP_VIDEO_DEINTERLACE_BLEND
    MWCAP_VIDEO_ASPECT_RATIO_CONVERT_MODE	m_aspect_ratio_convert_mode;//MWCAP_VIDEO_ASPECT_RATIO_IGNORE
    RECT *							        m_p_rect_src;//NULL
    RECT *							        m_p_rect_dest;//NULL
    int										m_aspect_x;//0
    int										m_aspect_y;//0
    MWCAP_VIDEO_COLOR_FORMAT				m_color_format;//MWCAP_VIDEO_COLOR_FORMAT_UNKNOWN
    MWCAP_VIDEO_QUANTIZATION_RANGE			m_quant_range;//MWCAP_VIDEO_QUANTIZATION_UNKNOWN
    MWCAP_VIDEO_SATURATION_RANGE			m_sat_range;//MWCAP_VIDEO_SATURATION_UNKNOWN


    HANDLE	m_video_thread;
    HANDLE	m_audio_thread;
};