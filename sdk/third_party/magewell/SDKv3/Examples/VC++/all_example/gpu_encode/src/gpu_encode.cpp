/************************************************************************************************/
// gpu_encode.cpp : Defines the entry point for the console application.

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
#include "argparse.h"
#include "LibMWCapture\MWCapture.h"
#include "LibMWVenc\mw_venc.h"
#define CAPTURE_WIDTH 1920
#define CAPTURE_HEIGHT 1080
#define CAPTURE_FOURCC MWFOURCC_NV12
HCHANNEL					g_channel_handle = NULL;
MWCAP_VIDEO_SIGNAL_STATUS	g_video_signal_status;
HANDLE g_video_handle = NULL;
int g_gpu_index = 0;
int g_need_encode_frames = 100;
mw_venc_param_t g_enc_param;
char *g_p_out_filename = NULL;
void	 show_info();
BOOL	 open_channel();
bool	 create_frame_buffer();
void	 destory_frame_buffer();
bool	 start_capture();
void	 stop_capture();
void	 start_encode();
void	 close_channel();
void     parse_cmd(int argc, char** argv);
int main(int argc, char* argv[])
{
    MWCaptureInitInstance();
    mw_venc_init();
    show_info();
    parse_cmd(argc, argv);
    if (!open_channel()) {
        goto end_and_destory;
    }
    if (!create_frame_buffer()) {
        goto end_and_destory;
    }
    if (!start_capture()) {
        goto end_and_destory;
    }
    start_encode();
end_and_destory:
    stop_capture();
    close_channel();
    destory_frame_buffer();
    MWCaptureExitInstance();
    mw_venc_deinit();
    getchar();
    return 0;
}

//show the basic information of our capture card
void show_info()
{
    BYTE maj, min;
    WORD build;
    MWGetVersion(&maj, &min, &build);

    printf("Magewell MWCapture SDK %d.%d.1.%d - gpu_encode\n", maj, min, build);
    printf("not support usb device\n");
//     printf("Usage:\n");
//     printf("gpu_encode.exe <gpu index>\n");
}
int opt_cb(struct argparse *self,
    const struct argparse_option *option)
{
    *(int *)option->value = option->data;
    return 0;
}
void parse_cmd(int argc, char** argv)
{
    mw_venc_get_default_param(&g_enc_param);
    g_enc_param.width = CAPTURE_WIDTH;
    g_enc_param.height = CAPTURE_HEIGHT;
    g_enc_param.fourcc = MW_VENC_FOURCC_NV12;
    g_enc_param.code_type = MW_VENC_CODE_TYPE_H264;
    g_enc_param.rate_control.target_bitrate = 1024;
    g_enc_param.rate_control.mode = MW_VENC_RATECONTROL_CBR;
    g_enc_param.fps.den = 1;
    g_enc_param.fps.num = 25;
    char *p_qp = NULL;
    char *p_fps = NULL;
    const char *const usages[] = {
        "test_argparse [options] [[--] args]",
        NULL,
    };
    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_GROUP("options"),
        OPT_BIT(0, "h264", &g_enc_param.code_type, "code type H264", opt_cb, MW_VENC_CODE_TYPE_H264, 0),
        OPT_BIT(0, "h265", &g_enc_param.code_type, "code type H265", opt_cb, MW_VENC_CODE_TYPE_H265, 0),

        OPT_BIT(0, "best_quality", &g_enc_param.targetusage, "targetusage best quality", opt_cb, MW_VENC_TARGETUSAGE_BEST_QUALITY, 0),
        OPT_BIT(0, "balanced", &g_enc_param.targetusage, "targetusage balanced", opt_cb, MW_VENC_TARGETUSAGE_BALANCED, 0),
        OPT_BIT(0, "best_speed", &g_enc_param.targetusage, "targetusage best speed", opt_cb, MW_VENC_TARGETUSAGE_BEST_SPEED, 0),

        OPT_BIT(0, "cbr", &g_enc_param.rate_control.mode, "cbr rate control mode", opt_cb, MW_VENC_RATECONTROL_CBR, 0),
        OPT_BIT(0, "vbr", &g_enc_param.rate_control.mode, "vbr rate control mode", opt_cb, MW_VENC_RATECONTROL_VBR, 0),
        OPT_BIT(0, "cqp", &g_enc_param.rate_control.mode, "cqp rate control mode", opt_cb, MW_VENC_RATECONTROL_CQP, 0),

        OPT_INTEGER('m', "max_bitrate", &g_enc_param.rate_control.max_bitrate, "max bitrate", NULL, 0, 0),
        OPT_INTEGER('t', "target_bitrate", &g_enc_param.rate_control.target_bitrate, "target bitrate", NULL, 0, 0),
        OPT_STRING('q', "qp", &p_qp, "qp value, cpq need set(qpi:qpb:qpp 22 21:22 21:22:23)", NULL, 0, 0),

        OPT_INTEGER('w', "width", &g_enc_param.width, "width", NULL, 0, 0),
        OPT_INTEGER('l', "height", &g_enc_param.height, "height", NULL, 0, 0),

        OPT_STRING('f', "fps", &p_fps, "fps(60 60/1)", NULL, 0, 0),

        OPT_INTEGER('s', "slice_num", &g_enc_param.slice_num, "slice num", NULL, 0, 0),
        OPT_INTEGER('p', "gop_size", &g_enc_param.gop_pic_size, "gop size", NULL, 0, 0),
        OPT_INTEGER('r', "ref_size", &g_enc_param.gop_ref_size, "ref size(4:ipbbb)", NULL, 0, 0),
        //OPT_BOOLEAN('c', "force", &p_param->code_type, "force to do", NULL, 0, 0),
        OPT_BIT(0, "base", &g_enc_param.profile, "h264 baseline profile, h265 just main", opt_cb, MW_VENC_PROFILE_H264_BASELINE, 0),
        OPT_BIT(0, "main", &g_enc_param.profile, "main profile", opt_cb, MW_VENC_PROFILE_H264_MAIN, 0),
        OPT_BIT(0, "high", &g_enc_param.profile, "h264 high profile, h265 just main", opt_cb, MW_VENC_PROFILE_H264_HIGH, 0),
        OPT_STRING('e', "enc_frames", &g_need_encode_frames, "encode frames", NULL, 0, 0),
        OPT_STRING('o', "output", &g_p_out_filename, "output file name", NULL, 0, 0),

        OPT_END(),
    };
    struct argparse argparse;
    argparse_init(&argparse, options, usages, 0);

    argparse_describe(&argparse, "\nA brief description of what the program does and how it works.", "\nAdditional description of the program after the description of the arguments.");
    if (1 == argc) {
        argparse_usage(&argparse);
    }
    argc = argparse_parse(&argparse, argc, (const char **)argv);
    if (p_qp) {
        g_enc_param.rate_control.qpi= g_enc_param.rate_control.qpb= g_enc_param.rate_control.qpp = atoi(p_qp);
        p_qp = strstr(p_qp,":");
        if (p_qp) {
            p_qp++;
            g_enc_param.rate_control.qpb = g_enc_param.rate_control.qpp = atoi(p_qp);
        }
        p_qp = strstr(p_qp, ":");
        if (p_qp) {
            p_qp++;
            g_enc_param.rate_control.qpp = atoi(p_qp);
        }
    }
    if (p_fps) {
        g_enc_param.fps.num = atoi(p_fps);
        p_fps = strstr(p_fps, ":");
        if (p_fps) {
            p_fps++;
            g_enc_param.fps.den = atoi(p_fps);
        }
    }
}

BOOL open_channel() 
{
    int total_device = MWGetChannelCount();
    if (total_device <= 0) {
        printf("not find magewell device or not install device\n");
        return false;
    }
    for (int i = 0; i < total_device; i++) {
        MWCAP_CHANNEL_INFO info;
        MW_RESULT mr = MWGetChannelInfoByIndex(i, &info);
        if ((MW_SUCCEEDED == mr) && (info.wFamilyID != MW_FAMILY_ID_USB_CAPTURE)) {
            WCHAR path[128] = { 0 };
            MWGetDevicePath(i, path);
            g_channel_handle = MWOpenChannelByPath(path);
            if (NULL == g_channel_handle) {
                printf("open fail\n");
                return false;
            }
            break;
        }
    }
    if (NULL == g_channel_handle) {
        printf("not find pro device\n");
        return false;
    }
    return true;
}
#define MAX_FRAME_BUFFER 4
unsigned char *g_p_frame_buffer[MAX_FRAME_BUFFER] = {NULL};
int g_write_num = 0;
int g_read_num = 0;
DWORD g_frame_size = 0;
bool create_frame_buffer()
{
    DWORD stride = FOURCC_CalcMinStride(CAPTURE_FOURCC, g_enc_param.width, 2);
    g_frame_size = FOURCC_CalcImageSize(CAPTURE_FOURCC, g_enc_param.width, g_enc_param.height, stride);
    for (int i = 0; i < MAX_FRAME_BUFFER; i++) {
        g_p_frame_buffer[i] = (unsigned char *)malloc(g_frame_size);
        if (NULL == g_p_frame_buffer[i]) {
            return false;
        }
    }
    return true;
}
void destory_frame_buffer()
{
    for (int i = 0; i < MAX_FRAME_BUFFER; i++) {
        if (NULL == g_p_frame_buffer[i]) {
            return;
        }
        free(g_p_frame_buffer[i]);
    }
}
void video_callback(BYTE *frame, int frame_size, UINT64 time_stamp, void* param)
{
    if (frame_size > g_frame_size) {
        return;
    }
    if (g_write_num - g_read_num >= 4) {
        return;
    }
    memcpy(g_p_frame_buffer[g_write_num % MAX_FRAME_BUFFER], frame, frame_size);
    g_write_num++;
}
bool start_capture()
{
    g_video_handle = MWCreateVideoCapture(g_channel_handle, g_enc_param.width, g_enc_param.height, CAPTURE_FOURCC, 400000, video_callback, NULL);
    if (g_video_handle == NULL) {
        printf("Open Video Capture error!\n");
        return false;
    }
    return true;
}
void stop_capture() 
{
    if (g_video_handle) {
        MWDestoryVideoCapture(g_video_handle);
    }
}
void close_channel()
{
    if (g_channel_handle != NULL) {
        MWCloseChannel(g_channel_handle);
        g_channel_handle = NULL;
    }
}
void encoder_callback(void * user_ptr, const uint8_t * p_frame, uint32_t frame_len, mw_venc_frame_info_t *p_frame_info)
{
    if (user_ptr) {
        fwrite(p_frame,1,frame_len,(FILE*)user_ptr);
    }
}


void start_encode()
{
    FILE *fp;
    if (mw_venc_get_gpu_num() <= g_gpu_index) {
        printf("please install gpu device\n");
        return;
    }
    if (g_p_out_filename) {
        fp = fopen(g_p_out_filename, "wb");
    }
    else {
        if (MW_VENC_CODE_TYPE_H265 == g_enc_param.code_type) {
            fp = fopen("gpuenc.h265", "wb");
        }
        else {
            fp = fopen("gpuenc.h264", "wb");
        }
    }
    
    mw_venc_handle_t handle = mw_venc_create_by_index(g_gpu_index, &g_enc_param, encoder_callback, fp);
    if (NULL == handle) {
        printf("open encoder fail\n");
        if (fp) {
            fclose(fp);        
        }
        return;
    }
    mw_venc_gpu_info_t info;
    if (mw_venc_get_gpu_info_by_index(g_gpu_index, &info) == MW_VENC_STATUS_SUCCESS) {
        printf("encoder open on gpu %s\n",info.gpu_name);
    }
    g_read_num = g_write_num;
    while (g_need_encode_frames) {
        if (g_write_num <= g_read_num) {
            Sleep(5);
            continue;
        }
        mw_venc_put_frame(handle, g_p_frame_buffer[g_read_num%MAX_FRAME_BUFFER]);
        g_read_num++;
        g_need_encode_frames--;
    }
    mw_venc_destory(handle);
    if (fp) {
        fclose(fp);
    }
    printf("end encode\n");
}