/************************************************************************************************/
// mp4_muxer.cpp : Defines the entry point for the console application.
 
// MAGEWELL PROPRIETARY INFORMATION

// The following license only applies to head files and library within Magewell’s SDK 
// and not to Magewell’s SDK as a whole. 

// Copyrights © Nanjing Magewell Electronics Co., Ltd. (“Magewell”) All rights reserved.

// Magewell grands to any person who obtains the copy of Magewell’s head files and library 
// the rights,including without limitation, to use, modify, publish, sublicense, distribute
// the Software on the conditions that all the following terms are met:
// - The above copyright notice shall be retained in any circumstances.
// -The following disclaimer shall be included in the software and documentation and/or 
// other materials provided for the purpose of publish, distribution or sublicense.

// THE SOFTWARE IS PROVIDED BY MAGEWELL “AS IS” AND ANY EXPRESS, INCLUDING BUT NOT LIMITED TO,
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL MAGEWELL BE LIABLE 

// FOR ANY CLAIM, DIRECT OR INDIRECT DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT,
// TORT OR OTHERWISE, ARISING IN ANY WAY OF USING THE SOFTWARE.

// CONTACT INFORMATION:
// SDK@magewell.net
// http://www.magewell.com/
//
/************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "LibMWMp4\mw_mp4.h"
#include "h26x_reader.h"
#define H264_FILE "../../test_data/test.h264"
#define AAC_FILE "../../test_data/test48000.aac"
#define CC608_FILE "../../test_data/test.cc608"
#define H264_FPS 25
#define CC608_FPS 25
FILE *g_fp_video = NULL;
FILE *g_fp_audio = NULL;
FILE *g_fp_subtitle = NULL;
bool get_h264_video(uint8_t *p_stream, uint32_t *p_frame_len, uint64_t *p_ts);
bool get_aac_audio(uint8_t *p_stream, uint32_t *p_frame_len, uint64_t *p_ts);
bool get_cc608_subtitle(uint8_t *p_stream, uint32_t *p_frame_len, uint64_t *p_ts);

int main(int argc, char* argv[])
{
    uint8_t *p_stream = (uint8_t *)malloc(1024000);
    uint8_t a_buffer[2048];
    uint8_t s_buffer[256];
    mw_mp4_handle_t mp4_handle = mw_mp4_open("h264_aac_cc608.mp4");
    if ((NULL == p_stream) || (NULL == mp4_handle)) {
        printf("open fail\n");
        goto end_and_close;
    }

    mw_mp4_video_info_t video_info;
    video_info.codec_type = MW_MP4_VIDEO_TYPE_H264;
    video_info.width = 1280;
    video_info.height = 720;
    video_info.timescale = 1000;
    video_info.h264.sps = NULL;
    video_info.h264.sps_size = 0;
    video_info.h264.pps = NULL;
    video_info.h264.pps_size = 0;
    mw_mp4_set_video(mp4_handle, &video_info);

    mw_mp4_audio_info_t audio_info;
    audio_info.codec_type = MW_MP4_AUDIO_TYPE_ADTS_AAC;
    audio_info.channels = 0;
    audio_info.sample_rate = 0;
    audio_info.timescale = 1000;
    mw_mp4_set_audio(mp4_handle, &audio_info);

    mw_mp4_subtitle_info_t subtitle_info;
    subtitle_info.codec_type = MW_MP4_SUBTITLE_TYPE_CC608;
    subtitle_info.timescale = 1000;
    mw_mp4_set_subtitle(mp4_handle, &subtitle_info);
 
    for (int i = 0; i < 500; i++) {
        uint32_t frame_len;
        uint64_t ts;
        if (get_h264_video(p_stream, &frame_len, &ts)) {
            mw_mp4_write_video(mp4_handle, p_stream, frame_len, ts);

        }
        if (get_aac_audio(a_buffer, &frame_len, &ts)) {
            mw_mp4_write_audio(mp4_handle, a_buffer, frame_len, ts);
        }
        if (get_cc608_subtitle(s_buffer, &frame_len, &ts)) {
            mw_mp4_write_subtitle(mp4_handle, s_buffer, frame_len, ts);
        }
    }
    printf("muxer end\n");
end_and_close:
    if (p_stream) {
        free(p_stream);
    }
    if (mp4_handle) {
        mw_mp4_close(mp4_handle);
    }
    if (g_fp_video) {
        fclose(g_fp_video);
    }
    if (g_fp_audio) {
        fclose(g_fp_audio);
    }
    if (g_fp_subtitle) {
        fclose(g_fp_subtitle);
    }

    getchar();
    return 0;
}
uint64_t v_ts = 0;
uint64_t a_ts = 0;
uint64_t s_ts = 0;
bool get_h264_video(uint8_t *p_stream, uint32_t *p_frame_len, uint64_t *p_ts)
{
    if (NULL == g_fp_video) {
        g_fp_video = fopen(H264_FILE, "rb");
    }
    if ((v_ts > a_ts) || (v_ts > s_ts)) {
        return false;
    }
    *p_ts = v_ts;
    v_ts += 1000 / H264_FPS;
    static unsigned int frame_len = 0;
    static unsigned int read_len = 0;
    read_h264_frame(g_fp_video, p_stream, 1024000, &frame_len, &read_len);
    *p_frame_len = frame_len;
    if (frame_len) {
        return true;
    }
    return false;
}

bool get_aac_audio(uint8_t *p_stream, uint32_t *p_frame_len, uint64_t *p_ts)
{
    static uint64_t tick = 0;
    if (NULL == g_fp_audio) {
        g_fp_audio = fopen(AAC_FILE, "rb");
    }
    if ((a_ts > v_ts) || (a_ts > s_ts)) {
        return false;
    }
    *p_ts = a_ts;
    tick += 1024;
    a_ts = tick * 1000/ 48000;
    unsigned int uiLen;
    fread(p_stream, 1, 7, g_fp_audio);
    uiLen = p_stream[3] & 0x3;
    uiLen = uiLen << 8;
    uiLen += p_stream[4];
    uiLen = uiLen << 3;
    uiLen += p_stream[5] >> 5;
    *p_frame_len = uiLen;
    fread(p_stream + 7, 1, uiLen - 7, g_fp_audio);
    return true;
}

bool get_cc608_subtitle(uint8_t *p_stream, uint32_t *p_frame_len, uint64_t *p_ts)
{
    if (NULL == g_fp_subtitle) {
        g_fp_subtitle = fopen(CC608_FILE, "rb");
    }
    if ((s_ts > v_ts) || (s_ts > a_ts)) {
        return false;
    }
    *p_ts = s_ts;
    s_ts += 1000 / CC608_FPS;
    fread(p_stream, 1, 6, g_fp_subtitle);
    *p_frame_len = 6;
    return true;
}