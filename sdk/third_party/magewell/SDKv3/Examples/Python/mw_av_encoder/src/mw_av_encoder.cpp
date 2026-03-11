// mw_av_encoder.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "mw_av_encoder.h"
#include "stdint.h"

extern "C"{
#include <libavutil/avassert.h>
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
#include <libavutil/mathematics.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>

#include <libavutil/imgutils.h>
#include <libavutil/parseutils.h>
#include <libswscale/swscale.h>
}

#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"swscale.lib")
#pragma comment(lib,"swresample.lib")

#ifdef _DEBUG

#else

#endif

typedef struct st_mw_video_encoder{
	int32_t				m_i32_fps;
	AVCodecContext*		m_p_enc_ctx_video;
	MW_ENCODER_CALLBACK	m_callback;
	void*				m_param;
}mw_video_encoder_t;

#define MAX_AAC_AUDIO_BUF 8192
typedef struct st_mw_audio_encoder{
	AVCodecContext*		m_p_enc_ctx_audio;
	AVFrame*			m_p_audio_frame;
	SwrContext*			m_p_swr_ctx_audio;
	uint8_t*			m_p_audio_data;
	uint32_t			m_u32_data_size;
	uint8_t*			m_p_encode_data;
	MW_ENCODER_CALLBACK	m_callback;
	void*				m_param;
}mw_audio_encoder_t;

MW_AV_ENCODER_API void mw_av_encode_init()
{
	printf("win - mw_av_encode_init\n");
	av_register_all();
	printf("win - mw_av_encode_init end\n");
}

MW_AV_ENCODER_API mw_video_encoder_handle mw_video_encoder_open(
	int width, 
	int height, 
	unsigned int mwfourcc, 
	int bitrate, 
	int fps,
	int idr_interval,
	int is_h265, 
	MW_ENCODER_CALLBACK frame_callback, 
	void *user_ptr)
{
	mw_video_encoder_t *t_p_vencoder=(mw_video_encoder_t *)malloc(sizeof(mw_video_encoder_t));
	if(NULL == t_p_vencoder){
		printf("malloc mw_video_encoder_t fail\n");
		return NULL;
	}
	memset(t_p_vencoder,0,sizeof(mw_video_encoder_t));

	t_p_vencoder->m_i32_fps = fps;
	t_p_vencoder->m_callback = frame_callback;
	t_p_vencoder->m_param = user_ptr;
	AVCodecID t_codec_id = is_h265==0?AV_CODEC_ID_H264:AV_CODEC_ID_H265;

	bool t_b_done = false;
	do 
	{
		AVCodec* t_p_video_codec = NULL;
		t_p_video_codec = avcodec_find_encoder(t_codec_id);
		if(t_p_video_codec == NULL){
			printf("ERROR - can't find encoder\n");
			break;
		}
		t_p_vencoder->m_p_enc_ctx_video = avcodec_alloc_context3(t_p_video_codec);
		if(t_p_vencoder->m_p_enc_ctx_video==NULL){
			printf("ERROR - can't alloc encoding context\n");
		}
		t_p_vencoder->m_p_enc_ctx_video->codec_id = t_codec_id;
		t_p_vencoder->m_p_enc_ctx_video->bit_rate = bitrate;
		t_p_vencoder->m_p_enc_ctx_video->width = width;
		t_p_vencoder->m_p_enc_ctx_video->height = height;

		printf("t_p_vencoder->m_p_enc_ctx_video->width:%d\n"
			"t_p_vencoder->m_p_enc_ctx_video->height:%d\n",
			t_p_vencoder->m_p_enc_ctx_video->width,
			t_p_vencoder->m_p_enc_ctx_video->height);

		av_opt_set(t_p_vencoder->m_p_enc_ctx_video->priv_data, "preset", "ultrafast", 0);
		av_opt_set(t_p_vencoder->m_p_enc_ctx_video->priv_data, "tune", "zerolatency", 0);

		AVRational t_base = {1,t_p_vencoder->m_i32_fps};
		t_p_vencoder->m_p_enc_ctx_video->time_base = t_base;
		t_p_vencoder->m_p_enc_ctx_video->gop_size = idr_interval;
		t_p_vencoder->m_p_enc_ctx_video->pix_fmt = (AVPixelFormat)(mwfourcc);

		if(avcodec_open2(t_p_vencoder->m_p_enc_ctx_video,t_p_video_codec,NULL)<0){
			printf("ERROR - could not open video codec\n");
			break;
		}
		t_b_done = true;
	} while (false);

	if(t_b_done==false){
		if(t_p_vencoder!=NULL){
			free(t_p_vencoder);
			t_p_vencoder = NULL;
		}
	}

	return (mw_video_encoder_handle)t_p_vencoder;
}

MW_AV_ENCODER_API int mw_video_encode_frame(
	mw_video_encoder_handle handle, 
	unsigned char *p_data, 
	long long ts)
{
	mw_video_encoder_t *t_p_vencoder=(mw_video_encoder_t *)handle;
	if(NULL == t_p_vencoder){
		return -1;
	}

	if(t_p_vencoder->m_p_enc_ctx_video == NULL)
		return -1;

	AVFrame t_frame = {0};
	AVPacket t_pkt = {0};
	t_frame.format = t_p_vencoder->m_p_enc_ctx_video->pix_fmt;
	t_frame.width = t_p_vencoder->m_p_enc_ctx_video->width;
	t_frame.height = t_p_vencoder->m_p_enc_ctx_video->height;

	av_init_packet(&t_pkt);
	t_frame.data[0]=p_data;
	t_frame.data[1]=p_data + t_p_vencoder->m_p_enc_ctx_video->width*t_p_vencoder->m_p_enc_ctx_video->height;
	t_frame.linesize[0]=t_p_vencoder->m_p_enc_ctx_video->width;
	t_frame.linesize[1]=t_p_vencoder->m_p_enc_ctx_video->width;
	t_frame.pts = ts/10000;

	int t_n_got_pkt = 0;
	avcodec_encode_video2(t_p_vencoder->m_p_enc_ctx_video,&t_pkt,&t_frame,&t_n_got_pkt);
	if((0==t_n_got_pkt)||(0==t_pkt.size))
		return 0;

	mw_venc_frame_info t_info;
	if(t_pkt.flags&AV_PKT_FLAG_KEY)
		t_info.frame_type = MW_VENC_FRAME_TYPE_IDR;
	else
		t_info.frame_type = MW_VENC_FRAME_TYPE_COUNT;
	t_info.delay = 0;
	//t_info.pts = ts;
	t_info.pts = t_pkt.pts;

	if(t_p_vencoder->m_callback!=NULL)
		t_p_vencoder->m_callback(
		t_p_vencoder->m_param,
		t_pkt.data,
		t_pkt.size,
		t_info.frame_type,
		t_info.delay,
		t_info.pts);

	return 0;
}

MW_AV_ENCODER_API int mw_video_encoder_close(mw_video_encoder_handle handle)
{
	mw_video_encoder_t *t_p_vencoder=(mw_video_encoder_t *)handle;
	if(NULL == t_p_vencoder){
		return -1;
	}

	if(t_p_vencoder->m_p_enc_ctx_video!=NULL){
		avcodec_free_context(&(t_p_vencoder->m_p_enc_ctx_video));
		t_p_vencoder->m_p_enc_ctx_video = NULL;
	}

	free(t_p_vencoder);
	return 0;
}

MW_AV_ENCODER_API mw_audio_encoder_handle mw_audio_encoder_open(
	unsigned int channels, 
	unsigned int sample_rate, 
	unsigned int bits_per_sample, 
	unsigned int bit_rate, 
	MW_ENCODER_CALLBACK frame_callback, 
	void *user_ptr)
{
	printf("channels:%d\nsamplerate:%d\nbitspersample:%d\nbitrate:%d\n",
		channels,
		sample_rate,
		bits_per_sample,
		bit_rate);

	mw_audio_encoder_t *t_p_aencoder = (mw_audio_encoder_t*)malloc(sizeof(mw_audio_encoder_t));
	if(NULL == t_p_aencoder){
		printf("malloc mw_video_encoder_t fail\n");
		return NULL;
	}

	memset(t_p_aencoder,0,sizeof(mw_audio_encoder_t));

	t_p_aencoder->m_callback = frame_callback;
	t_p_aencoder->m_param = user_ptr;
	bool t_b_done = false;
	do 
	{
		t_p_aencoder->m_p_audio_data = new uint8_t[192000*8*4];
		t_p_aencoder->m_u32_data_size = 0;
		t_p_aencoder->m_p_encode_data = new uint8_t[1024*8];

		if(t_p_aencoder->m_p_audio_data==NULL||
			t_p_aencoder->m_p_encode_data==NULL){
				break;
		}

		AVCodec* t_p_audio_codec = NULL;
		t_p_audio_codec = avcodec_find_encoder(AV_CODEC_ID_AAC);
		if(t_p_audio_codec==NULL){
			printf("ERROR - could not find encoder for aac\n");
			break;
		}
		t_p_aencoder->m_p_enc_ctx_audio = avcodec_alloc_context3(t_p_audio_codec);
		if(t_p_aencoder->m_p_enc_ctx_audio==NULL){
			printf("ERROR - could not alloc encoding context\n");
			break;
		}

		t_p_aencoder->m_p_enc_ctx_audio->bit_rate = bit_rate;
		if(bits_per_sample!=16){
			printf("ERROR - not 16 bits\n");
			break;
		}

		bool t_b_need_swr = false;

		AVSampleFormat t_sp_fmt = AV_SAMPLE_FMT_S16;
		t_p_aencoder->m_p_enc_ctx_audio->sample_fmt = t_sp_fmt;
		if(t_p_audio_codec->sample_fmts){
			t_p_aencoder->m_p_enc_ctx_audio->sample_fmt = t_p_audio_codec->sample_fmts[0];
			for(int i=0;t_p_audio_codec->sample_fmts[i]!=AV_SAMPLE_FMT_NONE;i++){
				if(t_sp_fmt== t_p_audio_codec->sample_fmts[i]){
					t_p_aencoder->m_p_enc_ctx_audio->sample_fmt = t_sp_fmt;
					break;
				}
			}
			if(t_sp_fmt != t_p_aencoder->m_p_enc_ctx_audio->sample_fmt)
				t_b_need_swr=true;
		}

		int t_sp_sample_rate = sample_rate;
		t_p_aencoder->m_p_enc_ctx_audio->sample_rate = t_sp_sample_rate;
		if(t_p_audio_codec->supported_samplerates){
			t_p_aencoder->m_p_enc_ctx_audio->sample_rate = t_p_audio_codec->supported_samplerates[0];
			for(int i=0;t_p_audio_codec->supported_samplerates[i];i++){
				if(t_sp_sample_rate==t_p_audio_codec->supported_samplerates[i]){
					t_p_aencoder->m_p_enc_ctx_audio->sample_rate = t_sp_sample_rate;
					break;
				}
			}
			if(t_p_aencoder->m_p_enc_ctx_audio->sample_rate!=t_sp_sample_rate)
				t_b_need_swr = true;
		}

		int t_sp_channels = channels;
		uint64_t t_sp_channel_layout = av_get_default_channel_layout(t_sp_channels);
		t_p_aencoder->m_p_enc_ctx_audio->channels = t_sp_channels;
		t_p_aencoder->m_p_enc_ctx_audio->channel_layout = t_sp_channel_layout;
		if(t_p_audio_codec->channel_layouts){
			t_p_aencoder->m_p_enc_ctx_audio->channel_layout = t_p_audio_codec->channel_layouts[0];
			t_p_aencoder->m_p_enc_ctx_audio->channels = av_get_channel_layout_nb_channels(t_p_aencoder->m_p_enc_ctx_audio->channel_layout);
			for(int i=0;t_p_audio_codec->channel_layouts[i];i++){
				int t_n_channels = av_get_channel_layout_nb_channels(t_p_audio_codec->channel_layouts[i]);
				if(t_n_channels == t_sp_channels){
					t_p_aencoder->m_p_enc_ctx_audio->channels = t_sp_channels;
					t_p_aencoder->m_p_enc_ctx_audio->channel_layout = t_sp_channel_layout;
					break;
				}
			}
			if(t_sp_channels!=t_p_aencoder->m_p_enc_ctx_audio->channels)
				t_b_need_swr = true;
		}

		if(avcodec_open2(t_p_aencoder->m_p_enc_ctx_audio,t_p_audio_codec,NULL)<0){
			printf("ERROR - open audio codec failed\n");
			break;
		}

		t_p_aencoder->m_p_audio_frame = av_frame_alloc();
		if(t_p_aencoder->m_p_audio_frame==NULL)
			break;
		t_p_aencoder->m_p_audio_frame->format = t_p_aencoder->m_p_enc_ctx_audio->sample_fmt;
		t_p_aencoder->m_p_audio_frame->channel_layout = t_p_aencoder->m_p_enc_ctx_audio->channel_layout;
		t_p_aencoder->m_p_audio_frame->sample_rate = t_p_aencoder->m_p_enc_ctx_audio->sample_rate;
		t_p_aencoder->m_p_audio_frame->nb_samples = 1024;
		if(t_b_need_swr){
			t_p_aencoder->m_p_swr_ctx_audio = swr_alloc();
			if(t_p_aencoder->m_p_swr_ctx_audio == NULL){
				printf("ERROR - could not alloc resampler context\n");
				break;
			}

			av_opt_set_int(t_p_aencoder->m_p_swr_ctx_audio, "in_channel_count", channels, 0);
			av_opt_set_int(t_p_aencoder->m_p_swr_ctx_audio, "in_sample_rate", sample_rate, 0);
			av_opt_set_sample_fmt(t_p_aencoder->m_p_swr_ctx_audio, "in_sample_fmt", t_sp_fmt, 0);
			av_opt_set_int(t_p_aencoder->m_p_swr_ctx_audio, "out_channel_count", t_p_aencoder->m_p_enc_ctx_audio->channels, 0);
			av_opt_set_int(t_p_aencoder->m_p_swr_ctx_audio, "out_sample_rate", t_p_aencoder->m_p_enc_ctx_audio->sample_rate, 0);
			av_opt_set_sample_fmt(t_p_aencoder->m_p_swr_ctx_audio, "out_sample_fmt", t_p_aencoder->m_p_enc_ctx_audio->sample_fmt, 0);

			/* initialize the resampling context */
			if (swr_init(t_p_aencoder->m_p_swr_ctx_audio) < 0) {
				printf("ERROR - failed to initialize the resampling context\n");
				break;
			}
			av_frame_get_buffer(t_p_aencoder->m_p_audio_frame, 0);
		}
		t_b_done = true;
	} while (false);

	if(t_b_done==false){
		if(t_p_aencoder->m_p_audio_data!=NULL){
			delete[] t_p_aencoder->m_p_audio_data;
			t_p_aencoder->m_p_audio_data = NULL;
		}

		if(t_p_aencoder->m_p_encode_data!=NULL){
			delete[] t_p_aencoder->m_p_encode_data;
			t_p_aencoder->m_p_encode_data = NULL;
		}

		if(t_p_aencoder->m_p_enc_ctx_audio!=NULL){
			avcodec_free_context(&t_p_aencoder->m_p_enc_ctx_audio);
			t_p_aencoder->m_p_enc_ctx_audio = NULL;
		}

		if(t_p_aencoder->m_p_audio_frame!=NULL){
			av_frame_free(&t_p_aencoder->m_p_audio_frame);
			t_p_aencoder->m_p_audio_frame = NULL;
		}

		if(t_p_aencoder!=NULL){
			delete t_p_aencoder;
			t_p_aencoder = NULL;
		}
	}

	return (mw_audio_encoder_handle)t_p_aencoder;
}

MW_AV_ENCODER_API int mw_audio_encode_frame(
	mw_audio_encoder_handle handle, 
	unsigned char *p_data, 
	unsigned int data_len, 
	long long ts)
{
	mw_audio_encoder_t *t_p_aencoder = (mw_audio_encoder_t*)handle;
	int out_len;
	if(NULL == t_p_aencoder){
		return -1;
	}

	if(t_p_aencoder->m_p_audio_data==NULL)
		return -1;

	memcpy(t_p_aencoder->m_p_audio_data+t_p_aencoder->m_u32_data_size,p_data,data_len);
	t_p_aencoder->m_u32_data_size += data_len;

	int t_n_sp_size = 1024*4;

	if(t_p_aencoder->m_u32_data_size<t_n_sp_size)
		return 0;
	else{
		memcpy(t_p_aencoder->m_p_encode_data,t_p_aencoder->m_p_audio_data,t_n_sp_size);
		t_p_aencoder->m_u32_data_size-=t_n_sp_size;
		memcpy(t_p_aencoder->m_p_audio_data,t_p_aencoder->m_p_audio_data+t_n_sp_size,t_p_aencoder->m_u32_data_size);
	}

	int t_n_got_pkt = 0;
	AVPacket t_pkt = {0};
	if(t_p_aencoder->m_p_enc_ctx_audio==NULL){
		printf("ERROR - no enc\n");
		return -1;
	}
	if(t_p_aencoder->m_p_swr_ctx_audio!=NULL){
		uint8_t* t_p_data_in[8]={0};
		t_p_data_in[0]=t_p_aencoder->m_p_encode_data;
		if(swr_convert(t_p_aencoder->m_p_swr_ctx_audio,
			t_p_aencoder->m_p_audio_frame->data,
			t_n_sp_size*4,
			(const uint8_t**)t_p_data_in,
			t_n_sp_size/4)<0){
				printf("ERROR - convert failed\n");
		}
	}else{
		t_p_aencoder->m_p_audio_frame->data[0]=t_p_aencoder->m_p_encode_data;
	}

	t_p_aencoder->m_p_audio_frame->nb_samples = t_n_sp_size/4;
	t_p_aencoder->m_p_audio_frame->format = t_p_aencoder->m_p_enc_ctx_audio->sample_fmt;
	t_p_aencoder->m_p_audio_frame->channel_layout = t_p_aencoder->m_p_enc_ctx_audio->channel_layout;
	t_p_aencoder->m_p_audio_frame->sample_rate = t_p_aencoder->m_p_enc_ctx_audio->sample_rate;

	avcodec_encode_audio2(t_p_aencoder->m_p_enc_ctx_audio,&t_pkt,t_p_aencoder->m_p_audio_frame,&t_n_got_pkt);
	if((0==t_n_got_pkt)||(0==t_pkt.size)){
		return 0;
	}

	if(t_p_aencoder->m_callback!=NULL){
		int callback_len;
		mw_venc_frame_info_t frame_info;
		int index = 0;
		frame_info.delay = 0;
		frame_info.frame_type = MW_VENC_FRAME_TYPE_UNKNOWN;
		frame_info.pts = ts;
		t_p_aencoder->m_callback(t_p_aencoder->m_param,t_pkt.data,t_pkt.size,frame_info.frame_type,frame_info.delay,frame_info.pts);
	}
	return 0;
}

int mw_get_aac_profile(
    mw_audio_encoder_handle handle,
    unsigned char *p_profile)
{
    mw_audio_encoder_t *t_p_aencoder = (mw_audio_encoder_t*)handle;
    if ((NULL == t_p_aencoder) || (NULL == p_profile) || (NULL == t_p_aencoder->m_p_enc_ctx_audio) || (NULL == t_p_aencoder->m_p_enc_ctx_audio->extradata)) {
        return -1;
    }
    *p_profile = ((t_p_aencoder->m_p_enc_ctx_audio->extradata[0] & 0xf8) >> 3) - 1;
    return 0;
}

MW_AV_ENCODER_API int mw_audio_encoder_close(mw_audio_encoder_handle handle)
{
	mw_audio_encoder_t *t_p_aencoder = (mw_audio_encoder_t*)handle;
	if(NULL == t_p_aencoder){
		return -1;
	}

	if(t_p_aencoder->m_p_audio_data!=NULL){
		delete[] t_p_aencoder->m_p_audio_data;
		t_p_aencoder->m_p_audio_data = NULL;
	}

	if(t_p_aencoder->m_p_encode_data!=NULL){
		delete[] t_p_aencoder->m_p_encode_data;
		t_p_aencoder->m_p_encode_data = NULL;
	}

	if(t_p_aencoder->m_p_enc_ctx_audio!=NULL){
		avcodec_free_context(&t_p_aencoder->m_p_enc_ctx_audio);
		t_p_aencoder->m_p_enc_ctx_audio = NULL;
	}

	if(t_p_aencoder->m_p_audio_frame!=NULL){
		av_frame_free(&t_p_aencoder->m_p_audio_frame);
		t_p_aencoder->m_p_audio_frame = NULL;
	}

	if(t_p_aencoder!=NULL){
		delete t_p_aencoder;
		t_p_aencoder = NULL;
	}
	return 0;
}
