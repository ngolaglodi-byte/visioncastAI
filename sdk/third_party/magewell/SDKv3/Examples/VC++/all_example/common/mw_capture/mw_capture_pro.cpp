#include "mw_capture_pro.h"


CMWCapturePro::CMWCapturePro()
{
    m_video_capturing = false;
    m_audio_capturing = false;
    m_capture_video = true;
    m_capture_audio = true;

    m_bottom_up = false;//
    m_process_switchs = 0;//
    m_parital_notify = 0;//0
    m_OSD_image = NULL;//NULL
    m_p_OSD_rects = NULL;//NULL
    m_OSD_rects_num = 0;//0
    m_contrast = 100;//100
    m_brightness = 0;//0
    m_saturation = 100;//100
    m_hue = 0;//0
    m_deinterlace_mode = MWCAP_VIDEO_DEINTERLACE_BLEND;//
    m_aspect_ratio_convert_mode = MWCAP_VIDEO_ASPECT_RATIO_IGNORE;//
    m_p_rect_src = NULL;//NULL
    m_p_rect_dest = NULL;//NULL
    m_aspect_x = 0;//0
    m_aspect_y = 0;//0
    m_color_format = MWCAP_VIDEO_COLOR_FORMAT_UNKNOWN;//
    m_quant_range = MWCAP_VIDEO_QUANTIZATION_UNKNOWN;
    m_sat_range = MWCAP_VIDEO_SATURATION_UNKNOWN;

    m_video_thread = NULL;
    m_audio_thread = NULL;
    printf("create pro capture\n");
}

CMWCapturePro::~CMWCapturePro()
{
    if (m_video_thread) {
        m_video_capturing = false;
        WaitForSingleObject(m_video_thread, INFINITE);
    }
    if (m_audio_thread) {
        m_audio_capturing = false;
        WaitForSingleObject(m_audio_thread, INFINITE);
    }
    if (m_OSD_image) {
        LONG ret;
        MWCloseImage(m_channel_handle, m_OSD_image, &ret);
    }
    if (m_p_OSD_rects) {
        free(m_p_OSD_rects);
    }
    if (m_p_rect_src) {
        free(m_p_rect_src);
    }
    if (m_p_rect_dest) {
        free(m_p_rect_dest);
    }
    printf("destory pro capture\n");
}

DWORD WINAPI video_capture_pro(LPVOID p_param)
{
    CMWCapturePro *p_class = (CMWCapturePro *)p_param;

    if (p_class->m_frame_duration) {
        return p_class->capture_by_timer();
    }
    return p_class->capture_by_input();
}

DWORD WINAPI audio_capture_pro(LPVOID p_param)
{
    CMWCapturePro *p_class = (CMWCapturePro *)p_param;
    return p_class->audio_capture();
}
bool CMWCapturePro::check()
{
    if (NULL == m_channel_handle) {
        set_device(0);
        if (NULL == m_channel_handle) {
            return false;
        }
    }
	if (MW_FAMILY_ID_PRO_CAPTURE != m_device_family) {
		printf("the device is not pro device\n");
		return false;
	}
    check_signal();
    return true;
}
bool CMWCapturePro::set_mirror_and_reverse(bool is_mirror, bool is_reverse)
{
	if (is_mirror) {
		printf("pro device not support mirror\n");
		return false;
	}
	m_bottom_up = m_is_reverse = is_reverse ? 1 : 0;
	m_is_mirror = 0;
	return true;
}
bool CMWCapturePro::start_capture(bool video, bool audio)
{
    if (!check()) {
        return false;
    }
	if (m_is_reverse < 0) {
		m_is_reverse = m_bottom_up;
	}
	m_is_mirror = 0;
    if (video) {
        if (!check_video_buffer()) {
            printf("chenck video buffer fail\n");
            return false;
        }
        m_video_capturing = true;
        if (MW_FAMILY_ID_PRO_CAPTURE == m_device_family) {
            m_video_thread = CreateThread(NULL, 0, video_capture_pro, (LPVOID)this, 0, NULL);
        }
        if (NULL == m_video_thread) {
            printf("capture video fail\n");
            return false;
        }
    }

    if (audio) {
        if (!check_audio_buffer()) {
            printf("chenck audio buffer fail\n");
            return false;
        }
        m_audio_capturing = true;
        if ((MW_FAMILY_ID_PRO_CAPTURE == m_device_family) || (MW_FAMILY_ID_ECO_CAPTURE == m_device_family)) {
            m_audio_thread = CreateThread(NULL, 0, audio_capture_pro, (LPVOID)this, 0, NULL);
        }

        if (NULL == m_audio_thread) {
            printf("capture audio fail\n");
            return false;
        }
    }
	m_is_start = true;
    return true;
}

void CMWCapturePro::prev_frame_capture_process()
{
}
DWORD CMWCapturePro::capture_by_timer()
{
    printf("capture video by timer in\n");
    HANDLE capture_event = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (NULL == capture_event) {
        printf("create event fail\n");
        return 1;
    }
    if (MWStartVideoCapture(m_channel_handle, capture_event) != MW_SUCCEEDED) {
        printf("start video capture fail\n");
        CloseHandle(capture_event);
        return 1;
    }

    MWCAP_VIDEO_BUFFER_INFO video_buffer_info;
    MWCAP_VIDEO_FRAME_INFO video_frame_info;

    HANDLE timer_event = CreateEvent(NULL, FALSE, FALSE, NULL);
    HTIMER timer = MWRegisterTimer(m_channel_handle, timer_event);
    if (timer == NULL) {
        printf("register notify fail\n");
        goto end_and_free;
    }
    DWORD stride = FOURCC_CalcMinStride(m_mw_fourcc, m_width, 2);
    DWORD frame_size = FOURCC_CalcImageSize(m_mw_fourcc, m_width, m_height, stride);
    st_frame_t *p_frame;
    for (int i = 0; p_frame = m_p_video_buffer->get_buffer_by_index(i); i++) {
        MWPinVideoBuffer(m_channel_handle, p_frame->p_buffer, frame_size);
    }
    LONGLONG wait_tm = 0LL;
    MWGetDeviceTime(m_channel_handle, &wait_tm);
    while (m_video_capturing) {
        wait_tm += m_frame_duration;
        if (MWScheduleTimer(m_channel_handle, timer, wait_tm) != MW_SUCCEEDED) {
            continue;
        }
        if (WaitForSingleObject(timer_event, 100)) {
            continue;
        }
        if (MWGetVideoBufferInfo(m_channel_handle, &video_buffer_info) != MW_SUCCEEDED) {
            continue;
        }

        if (MWGetVideoFrameInfo(m_channel_handle, video_buffer_info.iNewestBufferedFullFrame, &video_frame_info) != MW_SUCCEEDED){
            continue;
		}

        if (NULL == p_frame) {
            p_frame = m_p_video_buffer->get_buffer_to_fill();
        }

        if (NULL == p_frame) {
            continue;
        }
        prev_frame_capture_process();
        MWCaptureVideoFrameToVirtualAddressEx(m_channel_handle, MWCAP_VIDEO_FRAME_ID_NEWEST_BUFFERED, (LPBYTE)p_frame->p_buffer,
            frame_size, stride, m_bottom_up, NULL, m_mw_fourcc, m_width, m_height, m_process_switchs, m_parital_notify, m_OSD_image, m_p_OSD_rects, m_OSD_rects_num,
            m_contrast, m_brightness, m_saturation, m_hue,
            m_deinterlace_mode, m_aspect_ratio_convert_mode, m_p_rect_src, m_p_rect_dest,
            m_aspect_x, m_aspect_y, m_color_format, m_quant_range, m_sat_range);
        if (WaitForSingleObject(capture_event, 100)) {
            continue;
        }
        p_frame->ts = video_frame_info.allFieldStartTimes[0]/10000;
        m_p_video_buffer->buffer_filled();
        m_capture_frame_count++;
        p_frame = NULL;
        MWCAP_VIDEO_CAPTURE_STATUS capture_status;
        MWGetVideoCaptureStatus(m_channel_handle, &capture_status);
    }
    for (int i = 0; p_frame = m_p_video_buffer->get_buffer_by_index(i); i++) {
        MWUnpinVideoBuffer(m_channel_handle, p_frame->p_buffer);
    }
    printf("capture video by timer out\n");
end_and_free:
    MWStopVideoCapture(m_channel_handle);
    if (timer) {
        MWUnregisterTimer(m_channel_handle, timer);
    }
    if (timer_event) {
        CloseHandle(timer_event);
    }
    if (capture_event) {
        CloseHandle(capture_event);
    }
    return 0;
}

DWORD CMWCapturePro::capture_by_input()
{
    printf("capture video by input in\n");
    HANDLE capture_event = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (NULL == capture_event) {
        printf("create event fail\n");
        return 1;
    }
    if (MWStartVideoCapture(m_channel_handle, capture_event) != MW_SUCCEEDED) {
        printf("start video capture fail\n");
        CloseHandle(capture_event);
        return 1;
    }

    MWCAP_VIDEO_BUFFER_INFO video_buffer_info;
    //MWGetVideoBufferInfo(m_channel_handle, &video_buffer_info);

    MWCAP_VIDEO_FRAME_INFO video_frame_info;
    //MWGetVideoFrameInfo(m_channel_handle, video_buffer_info.iNewestBufferedFullFrame, &video_frame_info);

    HANDLE notify_event = CreateEvent(NULL, FALSE, FALSE, NULL);
    HNOTIFY notify = MWRegisterNotify(m_channel_handle, notify_event, MWCAP_NOTIFY_VIDEO_FRAME_BUFFERED | MWCAP_NOTIFY_VIDEO_SIGNAL_CHANGE);
    if (notify == NULL) {
        printf("register notify fail\n");
        goto end_and_free;
    }
    DWORD stride = FOURCC_CalcMinStride(m_mw_fourcc, m_width, 2);
    DWORD frame_size = FOURCC_CalcImageSize(m_mw_fourcc, m_width, m_height, stride);
    st_frame_t *p_frame;
    for (int i = 0; p_frame = m_p_video_buffer->get_buffer_by_index(i);i++) {
        MWPinVideoBuffer(m_channel_handle, p_frame->p_buffer, frame_size);
    }
    bool have_signal = true;
    DWORD event_wait_time = 100;
    while (m_video_capturing){
        if (WaitForSingleObject(notify_event, event_wait_time)) {
            if (have_signal) {
                continue;
            }
        }
        else {
            ULONGLONG notify_status = 0;
            if (MWGetNotifyStatus(m_channel_handle, notify, &notify_status) != MW_SUCCEEDED) {
                continue;
            }

			if (MWGetVideoBufferInfo(m_channel_handle, &video_buffer_info) != MW_SUCCEEDED) {
				continue;
			}
 
			if (MWGetVideoFrameInfo(m_channel_handle, video_buffer_info.iNewestBufferedFullFrame, &video_frame_info) != MW_SUCCEEDED){
				continue;
			}
            if (notify_status & MWCAP_NOTIFY_VIDEO_SIGNAL_CHANGE) {
                MWCAP_VIDEO_SIGNAL_STATUS video_signal_status;
                MWGetVideoSignalStatus(m_channel_handle, &video_signal_status);
                if (MWCAP_VIDEO_SIGNAL_LOCKED == video_signal_status.state) {
                    have_signal = true;
                    event_wait_time = 100;
                }
                else {
                    have_signal = false;
                    event_wait_time = 25;
                }
                continue;
            }else if (!(notify_status & MWCAP_NOTIFY_VIDEO_FRAME_BUFFERED)) {
                continue;
            }
        }

        if (NULL == p_frame) {
            p_frame = m_p_video_buffer->get_buffer_to_fill();
        }

        if (NULL == p_frame) {
            continue;
        }
        prev_frame_capture_process();
        MWCaptureVideoFrameToVirtualAddressEx(m_channel_handle, MWCAP_VIDEO_FRAME_ID_NEWEST_BUFFERED, (LPBYTE)p_frame->p_buffer,
            frame_size, stride, m_bottom_up, NULL, m_mw_fourcc, m_width, m_height, m_process_switchs, m_parital_notify, m_OSD_image, m_p_OSD_rects, m_OSD_rects_num,
            m_contrast, m_brightness, m_saturation, m_hue,
            m_deinterlace_mode, m_aspect_ratio_convert_mode, m_p_rect_src, m_p_rect_dest,
            m_aspect_x, m_aspect_y, m_color_format, m_quant_range, m_sat_range);
        if (WaitForSingleObject(capture_event, 100)) {
            continue;
        }
        p_frame->ts = video_frame_info.allFieldStartTimes[0]/10000;
        m_p_video_buffer->buffer_filled();
        m_capture_frame_count++;
        p_frame = NULL;
        MWCAP_VIDEO_CAPTURE_STATUS capture_status;
        MWGetVideoCaptureStatus(m_channel_handle, &capture_status);
    }
    for (int i = 0; p_frame = m_p_video_buffer->get_buffer_by_index(i); i++) {
        MWUnpinVideoBuffer(m_channel_handle, p_frame->p_buffer);
    }
    printf("capture video by input out\n");

end_and_free:
    if (notify) {
        MWUnregisterNotify(m_channel_handle, notify);
    }
    MWStopVideoCapture(m_channel_handle);
    if (notify_event) {
        CloseHandle(notify_event);
    }
    if (capture_event) {
        CloseHandle(capture_event);
    }

    return 0;
}
DWORD CMWCapturePro::audio_capture()
{
    printf("audio capture in\n");
    HANDLE capture_event = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (NULL == capture_event) {
        printf("create event fail\n");
        return 1;
    }
    if (MWStartAudioCapture(m_channel_handle) != MW_SUCCEEDED) {
        CloseHandle(capture_event);
        printf("start audio capture fail\n");
        return 1;
    }
    HNOTIFY notify = MWRegisterNotify(m_channel_handle, capture_event, MWCAP_NOTIFY_AUDIO_SIGNAL_CHANGE | MWCAP_NOTIFY_AUDIO_FRAME_BUFFERED);
    if (notify == NULL) {
        printf("register notify fail\n");
        goto end_and_free;
    }
    while (m_audio_capturing) {
        if (WaitForSingleObject(capture_event, 100)) {
            continue;
        }
        ULONGLONG notify_status = 0LL;
        MWGetNotifyStatus(m_channel_handle, notify, &notify_status);

        if (notify_status & MWCAP_NOTIFY_AUDIO_FRAME_BUFFERED) {
            do {
                MWCAP_AUDIO_CAPTURE_FRAME audio_frame;
                if (MWCaptureAudioFrame(m_channel_handle, &audio_frame) != MW_SUCCEEDED) {
                    break;
                }
                st_frame_t *p_frame = m_p_audio_buffer->get_buffer_to_fill();
                if (NULL == p_frame) {
                    continue;
                }
                int byte_per_sample = m_audio_bit_per_sample / 8;
                int offset = (sizeof(DWORD) - byte_per_sample)*8;
                p_frame->ts = audio_frame.llTimestamp/10000;
                p_frame->frame_len = m_audio_channel_num * MWCAP_AUDIO_SAMPLES_PER_FRAME * byte_per_sample;
                DWORD *p_channel_data[MWCAP_AUDIO_MAX_NUM_CHANNELS] = { audio_frame.adwSamples, audio_frame.adwSamples+4, audio_frame.adwSamples+1, 
                    audio_frame.adwSamples+5, audio_frame.adwSamples+2, audio_frame.adwSamples+6, audio_frame.adwSamples+3, audio_frame.adwSamples+7};
                unsigned char *p_audio = p_frame->p_buffer;
                for (int i = 0; i < MWCAP_AUDIO_SAMPLES_PER_FRAME; i++) {
                    for (int j = 0; j < m_audio_channel_num; j++) {
                        DWORD date = *p_channel_data[j] >> offset;//audio_frame.adwSamples[read_pos[j]];
                        p_channel_data[j] += 8;
                        memcpy(p_audio, &date, byte_per_sample);
                        p_audio += byte_per_sample;
                    }
                }
                m_p_audio_buffer->buffer_filled();
            } while (m_audio_capturing);
        }
    }
    printf("audio capture out\n");
    MWUnregisterNotify(m_channel_handle, notify);
end_and_free:
    MWStopAudioCapture(m_channel_handle);
    CloseHandle(capture_event);
    return 1;
}