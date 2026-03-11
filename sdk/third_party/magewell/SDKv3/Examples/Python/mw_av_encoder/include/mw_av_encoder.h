// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the MW_AV_ENCODER_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// MW_AV_ENCODER_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef MW_AV_ENCODER_EXPORTS
#define MW_AV_ENCODER_API __declspec(dllexport)
#else
#define MW_AV_ENCODER_API __declspec(dllimport)
#endif

#include<stdio.h>
#include<stdlib.h>

#pragma pack(push)
#pragma pack(1)

typedef enum mw_v_fourcc_e{
	MW_V_FOURCC_FMT_NV12 = 25
}mw_v_fourcc_t;

typedef enum mw_a_sample_fmt_e{
	MW_A_SAMPLE_FMT_S16 = 1
}mw_a_sample_fmt_t;

typedef enum mw_v_codec_e{
	MW_V_CODEC_H264 = 28,
	MW_V_CODEC_H265 = 174
}mw_v_codec_t;

typedef enum mw_venc_frame_type
{
	MW_VENC_FRAME_TYPE_UNKNOWN,							///<Unknown frame
	MW_VENC_FRAME_TYPE_IDR,								///<IDR frame
	MW_VENC_FRAME_TYPE_I,								///<I-frame
	MW_VENC_FRAME_TYPE_P,								///<P-frame
	MW_VENC_FRAME_TYPE_B,								///<B-frame
	MW_VENC_FRAME_TYPE_COUNT							///<Number of frame types enumerated
}mw_venc_frame_type_t;
typedef struct mw_venc_frame_info
{
	mw_venc_frame_type_t frame_type;					
	int delay;										
	long long pts;										
}mw_venc_frame_info_t;

typedef void(*MW_ENCODER_CALLBACK)(void * user_ptr, unsigned char * p_frame, unsigned int frame_len, int frame_type,int delay,int pts);
typedef  void* mw_video_encoder_handle;
typedef  void* mw_audio_encoder_handle;

#pragma pack(pop)


#ifdef __cplusplus
extern "C"
{
#endif
	MW_AV_ENCODER_API void mw_av_encode_init();


	MW_AV_ENCODER_API 
		mw_video_encoder_handle mw_video_encoder_open(
		int width, 
		int height, 
		unsigned int mwfourcc, 
		int bitrate, 
		int fps, 
		int idr_interval, 
		int is_h265, 
		MW_ENCODER_CALLBACK frame_callback, 
		void *user_ptr);

	MW_AV_ENCODER_API 
		int mw_video_encode_frame(
		mw_video_encoder_handle handle, 
		unsigned char *p_data, 
		long long ts);

	MW_AV_ENCODER_API	
		int mw_video_encoder_close(mw_video_encoder_handle handle);

	MW_AV_ENCODER_API 
		mw_audio_encoder_handle mw_audio_encoder_open(
		unsigned int channels, 
		unsigned int sample_rate, 
		unsigned int bits_per_sample, 
		unsigned int bit_rate, 
		MW_ENCODER_CALLBACK frame_callback, 
		void *user_ptr);

	MW_AV_ENCODER_API
		int mw_audio_encode_frame(
		mw_audio_encoder_handle handle, 
		unsigned char *p_data, 
		unsigned int data_len, 
		long long ts);
    MW_AV_ENCODER_API
        int mw_get_aac_profile(
        mw_audio_encoder_handle handle,
        unsigned char *p_profile);

	MW_AV_ENCODER_API	
		int mw_audio_encoder_close(mw_audio_encoder_handle handle);

#ifdef __cplusplus
}
#endif
