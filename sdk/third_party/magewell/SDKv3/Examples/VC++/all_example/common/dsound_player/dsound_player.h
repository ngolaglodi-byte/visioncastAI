#pragma once

#include <dsound.h>
#include "ring_buffer/ring_buffer.h"
#include "LockUtils.h"



class CDSoundPlayer
{
public:
    CDSoundPlayer(void);
    virtual ~CDSoundPlayer(void);
    bool set_param(int sample_rate, int channels);

    bool play(CRingBuffer *p_audio_buffer);
    void stop();
    bool set_pan(LONG pan);
    bool set_volume(LONG volume);
    void run();
protected:
    bool write_wave();
    LPDIRECTSOUND				m_dsound;

    bool						m_running;
    HANDLE						m_thread;
    CRingBuffer				    *m_p_buffer;

    st_frame_t                  *m_p_frame;
    int                         m_have_write_len;

    WAVEFORMATEX				m_wfx;

    DWORD						m_frame_buffer_num;
    DWORD						m_sample_per_frame;

    DWORD						m_frame_size;
    DWORD						m_buffer_size;
    DWORD						m_write_pos;
    HANDLE						m_event_notify;

    LPDIRECTSOUNDBUFFER			m_dsound_buffer;
    LPDIRECTSOUNDNOTIFY			m_dsound_notify;




    CMWLock						m_csBuffer;
    int							m_nFrameSeq;
    int							m_cEmptyCount;

};

