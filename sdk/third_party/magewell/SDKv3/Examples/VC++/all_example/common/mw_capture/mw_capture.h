#pragma once
#include "LibMWCapture\MWCapture.h"
#include "ring_buffer\ring_buffer.h"
class CMWCapture {
public:
    CMWCapture();
    virtual ~CMWCapture();
    
    static void init();
    static void exit();
    static bool refresh_devices();
    static int get_channel_count();
    static bool get_channel_info_by_index(int index, MWCAP_CHANNEL_INFO *p_channel_info);
	static bool get_device_name_by_index(int index, char *p_device_name);
	
    static CMWCapture* mw_capture_factory(int index);

    virtual bool set_device(int magewell_device_index);
    virtual bool start_capture(bool capture_video, bool capture_audio);
	virtual bool set_mirror_and_reverse(bool is_mirror, bool is_reverse);

    float get_capture_fps();

    bool set_resolution(int width, int height);
    bool set_mw_fourcc(DWORD mw_fourcc);
    bool set_audio_channels(int channel_num);
    bool set_audio_sample_rate(int sample_rate);
	bool get_device_name(char *p_device_name);
    bool get_resolution(int *p_width, int *p_height);
    bool get_mw_fourcc(DWORD *p_mw_fourcc);
    bool get_audio_channels(int *p_channel_num);
    bool get_audio_sample_rate(int *p_sample_rate);
    bool get_audio_bit_per_sample(int *p_bit_per_sample);
	bool get_mirror_and_reverse(bool *p_is_mirror, bool *p_is_reverse);
    CRingBuffer *m_p_video_buffer;
    CRingBuffer *m_p_audio_buffer;
protected:
	bool                            m_is_start;
    int								m_width;
    int								m_height;
    DWORD                           m_mw_fourcc;
	int                             m_is_mirror;
	int                             m_is_reverse;

    int                             m_audio_sample_rate;
    int                             m_audio_channel_num;
    int                             m_audio_bit_per_sample;


    void check_signal();
    bool check_video_buffer();
    bool check_audio_buffer();
    int                             m_signal_frame_duration;
    HCHANNEL                        m_channel_handle;
    MW_FAMILY_ID                    m_device_family;
    int                             m_device_index;
    bool                            m_use_common_api;
    bool                            m_user_video_buffer;
    bool                            m_user_audio_buffer;
    int                             m_prev_frame_count;
    long long                       m_prev_time;
    float                           m_fps;
public:
    int                             m_frame_duration;
    int                             m_capture_frame_count;
};