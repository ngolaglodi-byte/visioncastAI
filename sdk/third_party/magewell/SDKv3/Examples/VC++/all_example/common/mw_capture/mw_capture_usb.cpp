#include "mw_capture_usb.h"


CMWCaptureUsb::CMWCaptureUsb()
{
    m_video_handle = NULL;
    m_audio_handle = NULL;
    m_use_common_api = true;
    printf("create usb capture\n");
}
CMWCaptureUsb::~CMWCaptureUsb()
{
    if (m_video_handle) {
        MWDestoryVideoCapture(m_video_handle);
    }
    if (m_audio_handle) {
        MWDestoryAudioCapture(m_audio_handle);
    }
    printf("destory usb capture\n");
}

bool CMWCaptureUsb::check()
{
    if (NULL == m_channel_handle) {
        set_device(0);
        if (NULL == m_channel_handle) {
            return false;
        }
    }
    check_signal();
    return true;
}
void video_callback(BYTE *p_buffer, int frame_len, UINT64 ts, void* p_param)
{
    CMWCaptureUsb *p_class = (CMWCaptureUsb *)p_param;
    st_frame_t *p_frame = p_class->m_p_video_buffer->get_buffer_to_fill();
    p_class->m_capture_frame_count++;
    if (NULL == p_frame) {
        return;
    }
    memcpy(p_frame->p_buffer, p_buffer, frame_len);
    p_frame->ts = GetTickCount();
    p_class->m_p_video_buffer->buffer_filled();
}
void audio_callback(const BYTE *p_buffer, int frame_len, UINT64 ts, void* p_param)
{
    CMWCaptureUsb *p_class = (CMWCaptureUsb *)p_param;
    int have_copy_len = 0;
    while (have_copy_len < frame_len){
        st_frame_t *p_frame = p_class->m_p_audio_buffer->get_buffer_to_fill();
        p_frame->ts = GetTickCount();
        if (NULL == p_frame) {
            return;
        }
        if (frame_len - have_copy_len >= p_frame->buffer_len - p_frame->frame_len) {
            memcpy(p_frame->p_buffer + p_frame->frame_len, p_buffer + have_copy_len, p_frame->buffer_len - p_frame->frame_len);
            have_copy_len += p_frame->buffer_len - p_frame->frame_len;
            p_frame->frame_len = p_frame->buffer_len;
            p_class->m_p_audio_buffer->buffer_filled();
        }
        else {
            memcpy(p_frame->p_buffer + p_frame->frame_len, p_buffer + have_copy_len, frame_len - have_copy_len);
            p_frame->frame_len += frame_len - have_copy_len;
            return;
        }
    }
}

bool CMWCaptureUsb::set_mirror_and_reverse(bool is_mirror, bool is_reverse)
{
	if (NULL == m_channel_handle) {
		printf("please set_device\n");
		return false;
	}
	if (MW_FAMILY_ID_USB_CAPTURE != m_device_family) {
		printf("just usb device can set set_mirror_and_reverse use common api\n");
		return false;
	}
	m_is_mirror = is_mirror?1:0;
	m_is_reverse = is_reverse?1:0;
	return true;
}

bool CMWCaptureUsb::start_capture(bool video, bool audio)
{
    if (!check()) {
        return false;
    }
	if (MW_FAMILY_ID_USB_CAPTURE == m_device_family) {
		MWCAP_VIDEO_PROCESS_SETTINGS					process_settings;
		MWGetVideoCaptureProcessSettings(m_channel_handle, &process_settings);
		bool is_mirror = process_settings.dwProcessSwitchs & MWCAP_VIDEO_PROCESS_MIRROR;
		bool is_reverse = process_settings.dwProcessSwitchs & MWCAP_VIDEO_PROCESS_FLIP;
		if (-1 == m_is_mirror) {
			m_is_mirror = is_mirror;
		}
		else {
			is_mirror = m_is_mirror == 1;
		}
		if ((MWFOURCC_BGR24 == m_mw_fourcc) || (MWFOURCC_BGRA == m_mw_fourcc)) {
			if (-1 == m_is_reverse) {
				m_is_reverse = !is_reverse;
			}
			else {
				is_reverse = m_is_reverse == 0;
			}
		}
		else {
			if (-1 == m_is_reverse) {
				m_is_reverse = is_reverse;
			}
			else {
				is_reverse = m_is_reverse == 1;
			}	
		}
		process_settings.dwProcessSwitchs = (is_mirror ? MWCAP_VIDEO_PROCESS_MIRROR : 0) | (is_reverse ? MWCAP_VIDEO_PROCESS_FLIP : 0);
		MWSetVideoCaptureProcessSettings(m_channel_handle, &process_settings);
	}
	else if((MWFOURCC_BGR24 == m_mw_fourcc) || (MWFOURCC_BGRA == m_mw_fourcc)){
		m_is_mirror = 0;
		m_is_reverse = 1;
	}
	else {
		m_is_mirror = 0;
		m_is_reverse = 0;
	}

    if (video) {
        if (!check_video_buffer()) {
            printf("chenck video buffer fail\n");
            return false;
        }
        m_video_handle = MWCreateVideoCapture(m_channel_handle, m_width, m_height, m_mw_fourcc,m_frame_duration? m_frame_duration :m_signal_frame_duration, video_callback, this);
        if (NULL == m_video_handle) {
            printf("capture video fail\n");
            return false;
        }
    }

    if (audio) {
        if (!check_audio_buffer()) {
            printf("chenck audio buffer fail\n");
            return false;
        }
        m_audio_handle = MWCreateAudioCapture(m_channel_handle, MWCAP_AUDIO_CAPTURE_NODE_DEFAULT, m_audio_sample_rate, m_audio_bit_per_sample, m_audio_channel_num, audio_callback, this);

        if (NULL == m_audio_handle) {
            printf("capture audio fail\n");
            return false;
        }
    }
	m_is_start = true;
    return true;
}

