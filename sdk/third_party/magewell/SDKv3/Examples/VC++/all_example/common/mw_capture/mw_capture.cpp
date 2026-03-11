#include "mw_capture.h"
#include "mw_capture_pro.h"
#include "mw_capture_eco.h"
#include "mw_capture_usb.h"

CMWCapture::CMWCapture()
{
    m_p_video_buffer = NULL;
    m_p_audio_buffer = NULL;

	m_is_start = false;
    m_frame_duration = 0;
    m_width = 0;
    m_height = 0;
    m_mw_fourcc = MWFOURCC_YUY2;
    m_audio_channel_num = 2;
    m_audio_bit_per_sample = 16;

	m_is_mirror = -1;
	m_is_reverse = -1;

    m_signal_frame_duration = 0;
    m_audio_sample_rate = 48000;
    m_channel_handle = NULL;
    m_device_family = MW_FAMILY_ID_PRO_CAPTURE;
    m_device_index = -1;
    m_use_common_api = false;
    m_user_video_buffer = false;
    m_user_audio_buffer = false;
    m_prev_frame_count = 0;
    m_prev_time = 0;
    m_fps = 0;
    m_capture_frame_count = 0;
}

CMWCapture::~CMWCapture()
{
    if (!m_user_video_buffer && m_p_video_buffer) {
        delete m_p_video_buffer;
        m_p_video_buffer = NULL;
    }
    if (!m_user_audio_buffer && m_p_audio_buffer) {
        delete m_p_audio_buffer;
        m_p_audio_buffer = NULL;
    }
    if (m_channel_handle) {
        MWCloseChannel(m_channel_handle);
        m_channel_handle = NULL;
    }

}

void CMWCapture::init()
{
    MWCaptureInitInstance();
}

void CMWCapture::exit()
{
    MWCaptureExitInstance();
}

bool CMWCapture::refresh_devices()
{
    if (MWRefreshDevice() != MW_SUCCEEDED) {
        return false;
    }
    return true;
}

int CMWCapture::get_channel_count()
{
    return MWGetChannelCount();
}

bool CMWCapture::get_channel_info_by_index(int index, MWCAP_CHANNEL_INFO *p_channel_info)
{
    if (MW_SUCCEEDED != MWGetChannelInfoByIndex(index, p_channel_info)) {
        return false;
    }
    return true;
}
void pin_device_name(MWCAP_CHANNEL_INFO channel_info, char *p_device_name)
{
	if (NULL == p_device_name) {
		return;
	}
	if (strstr(channel_info.szProductName, "Quad") || strstr(channel_info.szProductName, "Dual")) {
		sprintf(p_device_name, "%d-%d %s", channel_info.byBoardIndex, channel_info.byChannelIndex, channel_info.szProductName);
	}
	else {
		sprintf(p_device_name, "%d %s", channel_info.byBoardIndex, channel_info.szProductName);
	}
}
bool CMWCapture::get_device_name_by_index(int index, char *p_device_name)
{
	MWCAP_CHANNEL_INFO channel_info;
	if (MW_SUCCEEDED != MWGetChannelInfoByIndex(index, &channel_info)) {
		printf("MWGetChannelInfoByIndex fail\n");
		return false;
	}
	pin_device_name(channel_info, p_device_name);
	return true;
}
CMWCapture *CMWCapture::mw_capture_factory(int index)
{
    MWCAP_CHANNEL_INFO mci;
    CMWCapture *p_mw_capture = NULL;
    MWGetChannelInfoByIndex(index, &mci);
    if (mci.wFamilyID == MW_FAMILY_ID_PRO_CAPTURE) {
        p_mw_capture = (CMWCapture*)(new CMWCapturePro());
    }
    else if (mci.wFamilyID == MW_FAMILY_ID_ECO_CAPTURE) {
        p_mw_capture = (CMWCapture*)(new CMWCaptureEco());
    }
    else if (mci.wFamilyID == MW_FAMILY_ID_USB_CAPTURE) {
        p_mw_capture = (CMWCapture*)(new CMWCaptureUsb());
    }
    p_mw_capture->set_device(index);
    return p_mw_capture;
}

bool CMWCapture::set_device(int magewell_device_index)
{
	if (m_channel_handle) {
		if (m_device_index == magewell_device_index) {
			return true;
		}
		printf("have set_device(%d)\n",m_device_index);
		return false;
	}
    WCHAR path[128] = { 0 };
    MWGetDevicePath(magewell_device_index, path);
    m_channel_handle = MWOpenChannelByPath(path);
    if (NULL == m_channel_handle) {
        printf("open fail\n");
        return false;
    }
	MWCAP_CHANNEL_INFO info;
	MWGetChannelInfo(m_channel_handle, &info);
	m_device_family = (MW_FAMILY_ID)info.wFamilyID;
    m_device_index = magewell_device_index;
    return true;
}
void CMWCapture::check_signal()
{
    MWCAP_VIDEO_SIGNAL_STATUS	video_signal_status;
    MWGetVideoSignalStatus(m_channel_handle, &video_signal_status);
    switch (video_signal_status.state)
    {
    case MWCAP_VIDEO_SIGNAL_NONE:
        printf("Input signal status: NONE\n");
        break;
    case MWCAP_VIDEO_SIGNAL_UNSUPPORTED:
        printf("Input signal status: Unsupported\n");
        break;
    case MWCAP_VIDEO_SIGNAL_LOCKING:
        printf("Input signal status: Locking\n");
        break;
    case MWCAP_VIDEO_SIGNAL_LOCKED:
        printf("Input signal status: Locked\n");
        break;
    }

    if (MWCAP_VIDEO_SIGNAL_LOCKED == video_signal_status.state) {
        printf("Input signal resolution: %d x %d\n", video_signal_status.cx, video_signal_status.cy);
        double fps = (video_signal_status.bInterlaced == TRUE) ? (double)20000000LL / video_signal_status.dwFrameDuration : (double)10000000LL / video_signal_status.dwFrameDuration;
        printf("Input signal fps: %.2f\n", fps);
        printf("Input signal interlaced: %d\n", video_signal_status.bInterlaced);
        printf("Input signal frame segmented: %d\n", video_signal_status.bSegmentedFrame);
        m_signal_frame_duration = video_signal_status.dwFrameDuration;
    }
    if ((0 == m_width) || (0 == m_height)) {
        if (MWCAP_VIDEO_SIGNAL_LOCKED == video_signal_status.state) {
            m_width = video_signal_status.cx;
            m_height = video_signal_status.cy;
        }
        else {
            m_width = 1920;
            m_height = 1080;
        }
    }
    if ((0 == m_frame_duration) && (MWCAP_VIDEO_SIGNAL_LOCKED != video_signal_status.state)) {
        m_frame_duration = 40;
    }
    if (0 == m_mw_fourcc) {
        m_mw_fourcc = MWFOURCC_YUY2;
    }
    m_frame_duration = m_frame_duration * 10000;

    MWCAP_AUDIO_SIGNAL_STATUS	audio_signal_status;
    MWGetAudioSignalStatus(m_channel_handle, &audio_signal_status);
    if (!m_use_common_api && audio_signal_status.wChannelValid) {
        m_audio_sample_rate = audio_signal_status.dwSampleRate;
    }

    if (m_use_common_api) {
        AUDIO_FORMAT_INFO *p_format = NULL;
        int count = 0;
        bool have_format = false;
        if (!MWGetAudioCaptureSupportFormat(m_channel_handle, MWCAP_AUDIO_CAPTURE_NODE_DEFAULT, NULL, &count)) {
            return;
        }
        if (count) {
            p_format = (AUDIO_FORMAT_INFO *)malloc(count*sizeof(AUDIO_FORMAT_INFO));
        }
        else {
            return;
        }
        if (!MWGetAudioCaptureSupportFormat(m_channel_handle, MWCAP_AUDIO_CAPTURE_NODE_DEFAULT, p_format, &count)) {
            if (p_format) {
                free(p_format);
            }
            return;
        }
        for (int i = 0; i < count; i++) {
            if (m_audio_sample_rate == p_format[i].nSamplerate) {
                have_format = true;
                break;
            }
        }
        if (!have_format && count) {
            m_audio_sample_rate = p_format[0].nSamplerate;
        }
        if (p_format) {
            free(p_format);
        }
    }
}
bool CMWCapture::check_video_buffer()
{
    if ((!m_user_video_buffer) && (NULL == m_p_video_buffer)) {
        m_p_video_buffer = new CRingBuffer();
        if (NULL == m_p_video_buffer) {
            return false;
        }
        DWORD stride = FOURCC_CalcMinStride(m_mw_fourcc, m_width, 2);
        DWORD frame_size = FOURCC_CalcImageSize(m_mw_fourcc, m_width, m_height, stride);
        return m_p_video_buffer->set_property(5, frame_size);
    }
    return true;
}
bool CMWCapture::check_audio_buffer()
{
    if ((!m_user_audio_buffer) && (NULL == m_p_audio_buffer)) {
        m_p_audio_buffer = new CRingBuffer();
        if (NULL == m_p_audio_buffer) {
            return false;
        }
        return m_p_audio_buffer->set_property(500, m_audio_channel_num * MWCAP_AUDIO_SAMPLES_PER_FRAME * (m_audio_bit_per_sample / 8));
    }
    return true;
}
bool CMWCapture::start_capture(bool video, bool audio)
{
    return true;
}
bool CMWCapture::set_mirror_and_reverse(bool is_mirror, bool is_reverse)
{
	return true;
}
bool CMWCapture::get_mirror_and_reverse(bool *p_is_mirror, bool *p_is_reverse)
{
	if ((m_is_mirror < 0) || (m_is_reverse < 0)) {
		printf("please start capture\n");
		return false;
	}
	if (p_is_mirror) {
		*p_is_mirror = m_is_mirror == 1;
	}
	if (p_is_reverse) {
		*p_is_reverse = m_is_reverse == 1;
	}
	return true;
}
float CMWCapture::get_capture_fps()
{
    if ((m_capture_frame_count - m_prev_frame_count) < 30) {
        return m_fps;
    }
    long long now_time = GetTickCount();
    int diff = now_time - m_prev_time;
    if (diff <= 0) {
        return m_fps;
    }
    m_fps = (m_capture_frame_count - m_prev_frame_count)*1000.0 / diff;
    m_prev_time = now_time;
    m_prev_frame_count = m_capture_frame_count;
    return m_fps;
}


bool CMWCapture::set_resolution(int width, int height)
{
	if (m_is_start) {
		printf("have start\n");
		return false;
	}
    m_width = width;
    m_height = height;
    return true;
}
bool CMWCapture::set_mw_fourcc(DWORD mw_fourcc)
{
	if (m_is_start) {
		printf("have start\n");
		return false;
	}
    m_mw_fourcc = mw_fourcc;
    return true;
}
bool CMWCapture::set_audio_channels(int channel_num)
{
	if (m_is_start) {
		printf("have start\n");
		return false;
	}
    m_audio_channel_num = channel_num;
    return true;
}
bool CMWCapture::set_audio_sample_rate(int sample_rate)
{
	if (m_is_start) {
		printf("have start\n");
		return false;
	}
    m_audio_sample_rate = sample_rate;
    return true;
}

bool CMWCapture::get_resolution(int *p_width, int *p_height)
{
	if (!m_is_start) {
		printf("not start\n");
		return false;
	}
    *p_width = m_width;
    *p_height = m_height;
    return true;
}
bool CMWCapture::get_mw_fourcc(DWORD *p_mw_fourcc)
{
	if (!m_is_start) {
		printf("not start\n");
		return false;
	}
    *p_mw_fourcc = m_mw_fourcc;
    return true;
}
bool CMWCapture::get_audio_channels(int *p_channel_num)
{
	if (!m_is_start) {
		printf("not start\n");
		return false;
	}
    *p_channel_num = m_audio_channel_num;
    return true;
}
bool CMWCapture::get_audio_sample_rate(int *p_sample_rate)
{
	if (!m_is_start) {
		printf("not start\n");
		return false;
	}
    *p_sample_rate = m_audio_sample_rate;
    return true;
}
bool CMWCapture::get_audio_bit_per_sample(int *p_bit_per_sample)
{
	if (!m_is_start) {
		printf("not start\n");
		return false;
	}
    *p_bit_per_sample = m_audio_bit_per_sample;
    return true;
}

bool CMWCapture::get_device_name(char *p_device_name)
{
	if (NULL == m_channel_handle) {
		printf("not open\n");
		return false;
	}
	MWCAP_CHANNEL_INFO channel_info;
	if (MW_SUCCEEDED != MWGetChannelInfo(m_channel_handle, &channel_info)) {
		printf("MWGetChannelInfo fail\n");
		return false;
	}
	pin_device_name(channel_info, p_device_name);
	return true;
}