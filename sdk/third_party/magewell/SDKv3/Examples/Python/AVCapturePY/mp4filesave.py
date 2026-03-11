from PyQt5.QtCore import QThread
from win32 import win32api,win32event
import queue
from mwcapture.libmwavencoder import *
from mwcapture.libmwmp4 import *
import time

def video_enc_callback(param,data,len,frame_type,delay,ts):
    param.write_mp4_video(data,len,ts)

def audio_enc_callback(param,data,len,frame_type,delay,ts):
    param.write_mp4_audio(data,len,ts)

class CMWMp4Save(object):
    def __init__(self):
        super().__init__()
        self.m_video_encoder = 0
        self.m_audio_encoder = 0
        self.m_n_cx = 1920
        self.m_n_cy = 1080
        self.m_fps = 60
        self.m_n_channels = 2
        self.m_n_samplerate = 48000
        self.m_bitrate_audio = 64000
        self.m_n_bitspersample = 16
        self.m_file_index = 0
        self.m_file_name = 0
        self.m_b_set_audio = False
        self.m_b_set_video = False
        self.m_avencoder = mw_av_encoder()
        self.m_mp4 = mw_mp4()
        self.m_file = 0
        self.m_cb_video = mw_encode_callback(video_enc_callback)
        self.m_cb_audio = mw_encode_callback(audio_enc_callback)
    
    def create_mp4(self,
        filename,
        cx,
        cy,
        frame_dur,
        channels,
        samplerate,
        bitspersample,
        bitrate_audio
        ):
        t_b_done = False
        while t_b_done == False:
            self.m_file_name = filename
            self.m_file_index = 0
            self.m_n_cx = cx
            self.m_n_cy = cy
            self.m_fps = 10000000//frame_dur
            self.m_n_channels = channels
            self.m_n_samplerate = samplerate
            self.m_n_bitspersample = bitspersample
            self.m_bitrate_audio = bitrate_audio
            self.m_avencoder.mw_avenc_init()
            t_ret = self.create_video_encoder()
            if t_ret != 0:
                break
            t_ret = self.create_audio_encoder()
            if t_ret != 0:
                break
            self.m_file = self.m_mp4.mw_mp4_open(filename)
            if self.m_file == 0:
                break
            t_b_done = True
        if t_b_done == False:
            if self.m_video_encoder != 0:
                self.m_avencoder.mw_venc_encoder_close(self.m_video_encoder)
                self.m_video_encoder = 0
            if self.m_audio_encoder != 0:
                self.m_avencoder.mw_aenc_encoder_close(self.m_audio_encoder)
                self.m_audio_encoder = 0
            return -1
        self.m_b_set_video = False
        self.m_b_set_audio = False
        return 0

    def destory_mp4(self):
        if self.m_video_encoder != 0:
            self.m_avencoder.mw_venc_encoder_close(self.m_video_encoder)
            self.m_video_encoder = 0
        if self.m_audio_encoder != 0:
            self.m_avencoder.mw_aenc_encoder_close(self.m_audio_encoder)
            self.m_audio_encoder = 0
        if self.m_file != 0:
            self.m_mp4.mw_mp4_close(self.m_file)
            self.m_file = 0

    def write_mp4_video(self,data,size,ts):
        if self.m_b_set_video == False:
            t_video_track = mw_mp4_h264_info_t()
            t_video_track.codec_type = MW_MP4_VIDEO_TYPE_H264
            t_video_track.width = self.m_n_cx
            t_video_track.height = self.m_n_cy
            t_video_track.timescale = 1000
            t_video_track.h264.sps = 0
            t_video_track.h264.sps_size = 0
            t_video_track.h264.pps = 0
            t_video_track.h264.pps_size = 0
            t_ret = self.m_mp4.mw_mp4_set_video(self.m_file,addressof(t_video_track))
            if t_ret == 0:
                print('set video track successfully')
            else:
                print('set video track failed')
            self.m_b_set_video = True
        ts = int(time.time()*1000)
        self.m_mp4.mw_mp4_write_video(self.m_file ,data,size,ts)

    def write_video(self,data,ts):
        if self.m_video_encoder == 0:
            return
        self.m_avencoder.mw_venc_encode_frame(self.m_video_encoder,data,ts)

    def write_mp4_audio(self,data,size,ts):
        if self.m_b_set_audio == False:
            t_audio_track = mw_mp4_audio_info_t()
            t_audio_track.codec_type = MW_MP4_AUDIO_TYPE_AAC
            t_audio_track.channels = self.m_n_channels
            t_audio_track.sample_rate = self.m_n_samplerate
            t_audio_track.timescale = 1000
            t_audio_track.profile = 0
            profile = c_ubyte(1)
            self.m_avencoder.mw_aenc_get_aac_profile(self.m_audio_encoder, profile)
            t_audio_track.profile = profile
            t_ret = self.m_mp4.mw_mp4_set_audio(self.m_file,addressof(t_audio_track))
            if t_ret == 0:
                print('set audio track successfully')
            else:
                print('set audio track failed')
            self.m_b_set_audio = True
        ts = int(time.time()*1000)
        self.m_mp4.mw_mp4_write_audio(self.m_file ,data,size,ts)

    def write_audio(self,data,data_len,ts):
        if self.m_audio_encoder == 0:
            return
        self.m_avencoder.mw_aenc_encode_frame(self.m_audio_encoder,data,data_len,ts)

    def create_video_encoder(self):
        self.m_video_encoder = self.m_avencoder.mw_venc_encoder_open(
            self.m_n_cx,
            self.m_n_cy,
            MW_V_FOURCC_FMT_NV12,
            4000000,
            self.m_fps,
            60,
            0,
            self.m_cb_video,
            self)
        if self.m_video_encoder == 0:
            return -1
        return 0

    def create_audio_encoder(self):
        self.m_audio_encoder = self.m_avencoder.mw_aenc_encoder_open(
            self.m_n_channels,
            self.m_n_samplerate,
            self.m_n_bitspersample,
            self.m_bitrate_audio,
            self.m_cb_audio,
            self
        )
        if self.m_audio_encoder == 0:
            return -1
        return 0

class CVenThread(QThread):
    def __init__(self, parent=None):
        super().__init__(parent=parent)
        self.m_b_running = False
        self.m_mp4_writer =  0
        self.m_event_exit = win32event.CreateEvent(None,False,False,None)
        self.m_event_video = win32event.CreateEvent(None,False,False,None)
        self.m_event_audio = win32event.CreateEvent(None,False,False,None)

        self.m_video_buffer = []
        self.m_video_buffer_ts = []
        self.m_video_write_num = 0
        self.m_video_read_num = 0

        for i in range(4):
            tm = c_int32(0)
            self.m_video_buffer_ts.append(tm)
            data = bytes(0) #create_string_buffer(MWCAP_AUDIO_SAMPLES_PER_FRAME*2*sizeof(c_int16)*4)
            self.m_video_buffer.append(data)
        self.m_audio_buffer = []
        self.m_audio_buffer_len = []
        self.m_audio_buffer_ts = []
        self.m_audio_write_num = 0
        self.m_audio_read_num = 0

        for i in range(16):
            len = c_uint32(0)
            self.m_audio_buffer_len.append(len)
            tm = c_int32(0)
            self.m_audio_buffer_ts.append(tm)
            data = bytes(0) #create_string_buffer(MWCAP_AUDIO_SAMPLES_PER_FRAME*2*sizeof(c_int16)*4)
            self.m_audio_buffer.append(data)

    def start_enc(self,mp4_writer):
        self.m_b_running = True
        self.m_mp4_writer = mp4_writer
        self.start()

    def stop_enc(self):
        #self.m_b_running = False
        win32event.SetEvent(self.m_event_exit)
        self.wait()

    def put_video_frame(self,frame,t_len,ts):
        write_index = self.m_video_write_num % 4
        self.m_video_buffer[write_index] = frame
        self.m_video_buffer_ts[write_index] = ts
        self.m_video_write_num = self.m_video_write_num+1
        win32event.SetEvent(self.m_event_video)

    def put_audio_frame(self,frame,len,ts):
        write_index = self.m_audio_write_num % 16
        self.m_audio_buffer[write_index] = frame
        self.m_audio_buffer_len[write_index] = len
        self.m_audio_buffer_ts[write_index] = ts
        self.m_audio_write_num = self.m_audio_write_num+1
        win32event.SetEvent(self.m_event_audio)

    def run(self):
        t_array_event = [self.m_event_exit,self.m_event_video,self.m_event_audio]
        t_events = tuple(t_array_event)
        while self.m_b_running == True:
            t_wait_ret = win32event.WaitForMultipleObjects(
                t_events,
                False,
                win32event.INFINITE)
            if t_wait_ret == win32event.WAIT_OBJECT_0 + 0:
                break
            while self.m_video_read_num < self.m_video_write_num:
                read_index = self.m_video_read_num%4
                self.m_video_read_num = self.m_video_read_num+1
                self.m_mp4_writer.write_video(self.m_video_buffer[read_index],self.m_video_buffer_ts[read_index])

            while self.m_audio_read_num < self.m_audio_write_num:
                read_index = self.m_audio_read_num%16
                self.m_audio_read_num = self.m_audio_read_num+1
                self.m_mp4_writer.write_audio(self.m_audio_buffer[read_index],self.m_audio_buffer_len[read_index],self.m_audio_buffer_ts[read_index])
            


t_encoder = CMWMp4Save()

def test():
    global t_encoder
    t_encoder.create_mp4('1.mp4',1920,1080,166667)

if __name__ == '__main__':
    test()
