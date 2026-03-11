#include "mw_capture_eco.h"


CMWCaptureEco::CMWCaptureEco()
{
    m_video_capturing = false;
    m_audio_capturing = false;
    m_video_thread = NULL;
    m_audio_thread = NULL;
    printf("create eco capture\n");
}
CMWCaptureEco::~CMWCaptureEco()
{
    if (m_video_thread) {
        m_video_capturing = false;
        WaitForSingleObject(m_video_thread, INFINITE);
    }
    if (m_audio_thread) {
        m_audio_capturing = false;
        WaitForSingleObject(m_audio_thread, INFINITE);
    }
    printf("destory eco capture\n");
}

DWORD WINAPI video_capture_eco(LPVOID p_param)
{
    CMWCaptureEco *p_class = (CMWCaptureEco *)p_param;
    return p_class->eco_capture();
}
DWORD WINAPI audio_capture_eco(LPVOID p_param)
{
    CMWCaptureEco *p_class = (CMWCaptureEco *)p_param;
    return p_class->audio_capture();
}
bool CMWCaptureEco::check()
{
    if (NULL == m_channel_handle) {
        set_device(0);
        if (NULL == m_channel_handle) {
            return false;
        }
    }
	if (MW_FAMILY_ID_ECO_CAPTURE != m_device_family) {
		printf("the device is not eco device\n");
		return false;
	}
    check_signal();
    return true;
}
bool CMWCaptureEco::set_mirror_and_reverse(bool is_mirror, bool is_reverse)
{
	if (is_mirror) {
		printf("eco device not support mirror\n");
		return false;
	}
	m_is_reverse = is_reverse ? 1 : 0;
	m_is_mirror = 0;
	return true;
}
bool CMWCaptureEco::start_capture(bool video, bool audio)
{
    if (!check()) {
        return false;
    }
	if (m_is_reverse < 0) {
		m_is_reverse = 0;
	}
	m_is_mirror = 0;
    if (video) {
        if (!check_video_buffer()) {
            printf("chenck video buffer fail\n");
            return false;
        }
        m_video_capturing = true;
        m_video_thread = CreateThread(NULL, 0, video_capture_eco, (LPVOID)this, 0, NULL);
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
        m_audio_thread = CreateThread(NULL, 0, audio_capture_eco, (LPVOID)this, 0, NULL);
        if (NULL == m_audio_thread) {
            printf("capture audio fail\n");
            return false;
        }
    }
	m_is_start = true;
    return true;
}
DWORD CMWCaptureEco::eco_capture()
{
    printf("eco capture video in\n");
    HANDLE capture_event = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (NULL == capture_event) {
        printf("create event fail\n");
        return 1;
    }
    MWCAP_VIDEO_ECO_CAPTURE_OPEN ecoCaptureOpen = {
        capture_event,
        m_mw_fourcc,
        m_width,
        m_height,
        m_frame_duration ? m_frame_duration : -1
    };
    if (MWStartVideoEcoCapture(m_channel_handle, &ecoCaptureOpen) != MW_SUCCEEDED) {
        printf("start video eco capture fail\n");
        CloseHandle(capture_event);
        return 1;
    }
    DWORD stride = FOURCC_CalcMinStride(m_mw_fourcc, m_width, 2);
    DWORD frameSize = FOURCC_CalcImageSize(m_mw_fourcc, m_width, m_height, stride);

    MWCAP_VIDEO_ECO_CAPTURE_FRAME eco_frame[4] = {0};
    for (int i = 0; i < 4; i++) {
        eco_frame[i].pvFrame = (MWCAP_PTR64)malloc(frameSize);
        if (NULL == eco_frame[i].pvFrame) {
            goto end_and_free;
        }
        eco_frame[i].cbFrame = frameSize;
        eco_frame[i].cbStride = stride;
		eco_frame[i].bBottomUp = m_is_reverse;
        eco_frame[i].pvContext = &eco_frame[i];
        if (MWCaptureSetVideoEcoFrame(m_channel_handle, &eco_frame[i]) != MW_SUCCEEDED) {
            goto end_and_free;
        }
    }
    while (m_video_capturing) {
        if (WaitForSingleObject(capture_event, 100)) {
            continue;
        }
        while (m_video_capturing) {
            MWCAP_VIDEO_ECO_CAPTURE_STATUS capture_status;

            if (MWGetVideoEcoCaptureStatus(m_channel_handle, &capture_status) != MW_SUCCEEDED || capture_status.pvFrame == NULL) {
                break;
            }
            st_frame_t *p_frame = m_p_video_buffer->get_buffer_to_fill();
            if (p_frame) {
                memcpy(p_frame->p_buffer, capture_status.pvFrame, frameSize);
                p_frame->ts = capture_status.llTimestamp / 10000;
                m_p_video_buffer->buffer_filled();
            }
            m_capture_frame_count++;
            if (MWCaptureSetVideoEcoFrame(m_channel_handle, (MWCAP_VIDEO_ECO_CAPTURE_FRAME*)capture_status.pvContext) != MW_SUCCEEDED) {
                break;
            }
        }
    }
    printf("eco capture video out\n");
    MWStopVideoEcoCapture(m_channel_handle);
    if (capture_event) {
        CloseHandle(capture_event);
    }
end_and_free:
    for (int i = 0; i < 4; i++) {
        if (eco_frame[i].pvFrame) {
            free(eco_frame[i].pvFrame);
        }
        else {
            break;
        }
    }
    return 0;
}

DWORD CMWCaptureEco::audio_capture()
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