#include "dsound_player.h"

CDSoundPlayer::CDSoundPlayer(void) :
    m_running(false),
    m_thread(NULL),
    m_dsound(NULL),
    m_dsound_buffer(NULL),
    m_event_notify(NULL),
    m_dsound_notify(NULL),
    m_frame_size(0),
    m_buffer_size(0),
    m_write_pos(0),
    m_nFrameSeq(0),
    m_cEmptyCount(0),
    m_p_frame(NULL)
{
    printf("dsound create\n");
}

CDSoundPlayer::~CDSoundPlayer(void)
{
    stop();
    if (NULL != m_dsound_buffer) {
        m_dsound_buffer->Release();
        m_dsound_buffer = NULL;
    }
    if (NULL != m_dsound_notify) {
        m_dsound_notify->Release();
        m_dsound_notify = NULL;
    }
    if (m_event_notify) {
        CloseHandle(m_event_notify);
        m_event_notify = NULL;
    }
    printf("dsound destory\n");
}
#define FRAME_BUFFER_NUM 10
#define SAMPLE_PER_FRAME 192
bool CDSoundPlayer::set_param(int sample_rate, int channels)
{
    if (NULL == m_dsound) {
        GUID guid = GUID_NULL;
        if (DirectSoundCreate(&guid, &m_dsound, NULL) < 0) {
            printf("DirectSoundCreate fail\n");
            return false;
        }
    }

    memset(&m_wfx, 0, sizeof(m_wfx));
    m_wfx.wFormatTag = WAVE_FORMAT_PCM;
    m_wfx.nChannels = channels;
    m_wfx.wBitsPerSample = 16;
    m_wfx.nSamplesPerSec = sample_rate;
    m_wfx.nAvgBytesPerSec = sample_rate * channels * m_wfx.wBitsPerSample / 8;
    m_wfx.nBlockAlign = channels * m_wfx.wBitsPerSample / 8;
    m_wfx.cbSize = 0;

    m_frame_size = SAMPLE_PER_FRAME * channels * m_wfx.wBitsPerSample / 8;
    m_buffer_size = FRAME_BUFFER_NUM * m_frame_size;

    DSBUFFERDESC dsbd;
    ZeroMemory(&dsbd, sizeof(dsbd));
    dsbd.dwSize = sizeof(dsbd);
    dsbd.dwBufferBytes = m_buffer_size;
    dsbd.lpwfxFormat = &m_wfx;
    dsbd.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPOSITIONNOTIFY
        | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_GLOBALFOCUS;

    HWND hwnd = GetForegroundWindow();
    if (NULL == hwnd) {
        hwnd = GetDesktopWindow();
    }

    m_event_notify = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (NULL == m_event_notify) {
        printf("create evnet fail\n");
        return FALSE;
    }

    DSBPOSITIONNOTIFY pos_notify[FRAME_BUFFER_NUM];
    for (int i = 0; i < FRAME_BUFFER_NUM; i++) {
        pos_notify[i].dwOffset = i * m_frame_size + (m_frame_size - 1);
        pos_notify[i].hEventNotify = m_event_notify;
    }

    if (FAILED(m_dsound->SetCooperativeLevel(hwnd, DSSCL_PRIORITY))
        || FAILED(m_dsound->CreateSoundBuffer(&dsbd, &m_dsound_buffer, NULL))
        || FAILED(m_dsound_buffer->QueryInterface(IID_IDirectSoundNotify, (LPVOID *)&m_dsound_notify))
        || FAILED(m_dsound_notify->SetNotificationPositions(FRAME_BUFFER_NUM, pos_notify))
        || FAILED(m_dsound_buffer->SetCurrentPosition(0))) {
        printf("dsound fail\n");
        return FALSE;
    }
    m_frame_buffer_num = FRAME_BUFFER_NUM;
    m_sample_per_frame = SAMPLE_PER_FRAME;
    set_volume(DSBVOLUME_MAX);
    m_write_pos = 0;

    return TRUE;
}

bool CDSoundPlayer::set_pan(LONG pan)
{
    return (SUCCEEDED(m_dsound_buffer->SetPan(pan)));
}

bool CDSoundPlayer::set_volume(LONG volume)
{
    return (SUCCEEDED(m_dsound_buffer->SetVolume(volume)));
}

DWORD WINAPI play_thread(LPVOID param)
{
    CDSoundPlayer *p_palyer = (CDSoundPlayer *)param;
    p_palyer->run();
    return 0;
}
bool CDSoundPlayer::play(CRingBuffer *p_audio_buffer)
{
    if (m_running) {
        return false;
    }

    m_dsound_buffer->SetCurrentPosition(0);
    m_write_pos = 0;
    m_p_buffer = p_audio_buffer;
    //write_wave();

    m_running = TRUE;
    m_thread = CreateThread(NULL, 0, play_thread, this, 0, NULL);
    if (NULL == m_thread
        || FAILED(m_dsound_buffer->Play(0, 0, DSBPLAY_LOOPING))) {
        m_running = false;
        printf("play fail\n");
        return false;
    }

    return true;
}
void CDSoundPlayer::run()
{
    SetThreadPriority(m_thread, THREAD_PRIORITY_TIME_CRITICAL);

    while (m_running) {
        DWORD ret = WaitForSingleObject(m_event_notify, INFINITE);
        if (ret != WAIT_OBJECT_0 || !write_wave()) {
            printf("WaitForSingleObject fail\n");
            break;
        }
    }
}

void CDSoundPlayer::stop()
{
    if (m_running && m_dsound_buffer) {
        m_dsound_buffer->Stop();
    }
    m_running = FALSE;
    if (m_thread) {
        SetEvent(m_event_notify);
        WaitForSingleObject(m_thread, INFINITE);
        CloseHandle(m_thread);
        m_thread = NULL;
    }
}
bool CDSoundPlayer::write_wave()
{
    DWORD read_pos, write_pos;
    if (FAILED(m_dsound_buffer->GetCurrentPosition(&read_pos, &write_pos))) {
        printf("GetCurrentPosition fail\n");
        return FALSE;
    }

    //cbAvail = (cbAvail / m_cbFrameSize) * m_cbFrameSize;
    DWORD need_write_len = (read_pos + m_buffer_size - m_write_pos) % m_buffer_size;
    need_write_len = need_write_len / m_frame_size * m_frame_size;
    if (!need_write_len) {
        return TRUE;
    }
    LPVOID data1, data2;
    DWORD len1, len2;
    if (DSERR_BUFFERLOST == m_dsound_buffer->Lock(m_write_pos, need_write_len, &data1, &len1, &data2, &len2, 0)) {
        m_dsound_buffer->Restore();
        if (FAILED(m_dsound_buffer->Lock(m_write_pos, need_write_len, &data1, &len1, &data2, &len2, 0))) {
            printf("Lock fail\n");
            return FALSE;
        }
    }
    LPBYTE data;
    DWORD len;
    for (data = (LPBYTE)data1, len = len1; len > 0 && m_running; ) {
        if (NULL == m_p_frame) {
            m_p_frame = m_p_buffer->get_frame_to_render();
            if (NULL == m_p_frame) {
                Sleep(1);
                continue;
            }
            m_have_write_len = 0;
        }
        int left_len = m_p_frame->frame_len - m_have_write_len;
        if (len >= (left_len)) {
            memcpy(data, m_p_frame->p_buffer + m_have_write_len, left_len);
            m_p_frame = NULL;
            len -= left_len;
            data += left_len;
        }
        else {
            memcpy(data, m_p_frame->p_buffer+ m_have_write_len, len);
            m_have_write_len += len;
            len = 0;
        }
    }

    for (data = (LPBYTE)data2, len = len2; len > 0 && m_running; ) {
        if (NULL == m_p_frame) {
            m_p_frame = m_p_buffer->get_frame_to_render();
            if (NULL == m_p_frame) {
                Sleep(1);
                continue;
            }
            m_have_write_len = 0;
        }
        int left_len = m_p_frame->frame_len - m_have_write_len;
        if (len >= (left_len)) {
            memcpy(data, m_p_frame->p_buffer + m_have_write_len, left_len);
            m_p_frame = NULL;
            len -= left_len;
            data += left_len;
        }
        else {
            memcpy(data, m_p_frame->p_buffer + m_have_write_len, len);
            m_have_write_len += len;
            len = 0;
        }
    }

    m_write_pos = (m_write_pos + need_write_len) % m_buffer_size;
    m_dsound_buffer->Unlock(data1, len1, data2, len2);
    return TRUE;
}