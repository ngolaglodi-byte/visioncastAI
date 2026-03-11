/************************************************************************************************/
// crop_capture.cpp : Defines the entry point for the console application.

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
#include "bitmap.h"
#include "LibMWCapture\MWCapture.h"

HCHANNEL					g_channel_handle = NULL;
MWCAP_VIDEO_SIGNAL_STATUS	g_video_signal_status;

void	 show_info();
BOOL	 open_channel(int argc, char* argv[]);
BOOL	 check_signal();
void	 start_capture();
void	 close_channel();

int main(int argc, char* argv[])
{
    MWCaptureInitInstance();
    show_info();

    if (!open_channel(argc, argv)) {
        MWCaptureExitInstance();
        getchar();
        return 1;
    }

    if (!check_signal()) {
        close_channel();
        MWCaptureExitInstance();
        getchar();
        return 1;
    }

    start_capture();
    close_channel();
    MWCaptureExitInstance();
    getchar();
    return 0;
}

//show the basic information of our capture card
void show_info()
{
    BYTE maj, min;
    WORD build;
    MWGetVersion(&maj, &min, &build);

    printf("Magewell MWCapture SDK %d.%d.1.%d - crop_capture\n", maj, min, build);
    printf("just support pro device\n");
    printf("Usage:\n");
    printf("crop_capture.exe <channel_index>\n");
}


BOOL open_channel(int argc, char* argv[]) 
{
    int index = 0;
    if (argc == 2) {
        index = atoi(argv[1]);
    }
    int total_device = MWGetChannelCount();
    if (total_device <= 0) {
        printf("not find magewell device or not install device\n");
        return false;
    }
    for (int i = 0; i < total_device; i++) {
        MWCAP_CHANNEL_INFO info;
        MW_RESULT mr = MWGetChannelInfoByIndex(i, &info);
        if ((MW_SUCCEEDED == mr) && (info.wFamilyID == MW_FAMILY_ID_PRO_CAPTURE)) {
            if (index) {
                index--;
                continue;
            }
            WCHAR path[128] = { 0 };
            MWGetDevicePath(i, path);
            g_channel_handle = MWOpenChannelByPath(path);
            if (NULL == g_channel_handle) {
                printf("open fail\n");
                return false;
            }
            return true;
        }
    }
    printf("index out of range\n");
    return false;
}

//check the input status and related information of the video signal
BOOL check_signal()
{
    MWGetVideoSignalStatus(g_channel_handle, &g_video_signal_status);
    switch (g_video_signal_status.state)
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

    if (g_video_signal_status.state == MWCAP_VIDEO_SIGNAL_LOCKED){
        printf("Input signal resolution: %d x %d\n", g_video_signal_status.cx, g_video_signal_status.cy);
        double fps = (g_video_signal_status.bInterlaced == TRUE) ? (double)20000000LL / g_video_signal_status.dwFrameDuration : (double)10000000LL / g_video_signal_status.dwFrameDuration;
        printf("Input signal fps: %.2f\n", fps);
        printf("Input signal interlaced: %d\n", g_video_signal_status.bInterlaced);
        printf("Input signal frame segmented: %d\n", g_video_signal_status.bSegmentedFrame);
        return TRUE;
    }
    return FALSE;
}

void start_capture()
{
    HANDLE capture_event = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (MWStartVideoCapture(g_channel_handle, capture_event) != MW_SUCCEEDED) {
        printf("Open Video Capture error!\n");
        CloseHandle(capture_event);
        return;
    }

    HANDLE notify_event = CreateEvent(NULL, FALSE, FALSE, NULL);
    HNOTIFY notify = MWRegisterNotify(g_channel_handle, notify_event, MWCAP_NOTIFY_VIDEO_FRAME_BUFFERED);
    if (notify == NULL) {
        printf("register notify error\n");
        goto end_and_free;
    }

    int cx = g_video_signal_status.cx;
    int cy = g_video_signal_status.cy;
    DWORD stride = FOURCC_CalcMinStride(MWFOURCC_BGR24, cx, 2);
    DWORD frame_size = FOURCC_CalcImageSize(MWFOURCC_BGR24, cx, cy, stride);
    uint8_t *p_capture_data = (uint8_t *)malloc(frame_size);
    if (NULL == p_capture_data) {
        printf("malloc fail\n");
        goto end_and_free;
    }
    MWPinVideoBuffer(g_channel_handle, p_capture_data, frame_size);
    MWCAP_VIDEO_ASPECT_RATIO_CONVERT_MODE ratio_mode = MWCAP_VIDEO_ASPECT_RATIO_IGNORE;
    RECT *p_src_rect = NULL;
    RECT *p_dst_rect = NULL;
    RECT src_rect = { 0 };
    RECT dst_rect = { 0 };
    char bmp_name[128];
    for (int i = 0; i < 6; i++)
    {
        WaitForSingleObject(notify_event, INFINITE);

        ULONGLONG notify_status = 0;
        if (MWGetNotifyStatus(g_channel_handle, notify, &notify_status) != MW_SUCCEEDED){
            continue;
        }

        if (!(notify_status & MWCAP_NOTIFY_VIDEO_FRAME_BUFFERED)) {
            continue;
        }
        
        if (0 == i) {
            sprintf(bmp_name, "%dx%d.bmp",cx,cy);
        }
        else if (1 == i) {
            src_rect.left = cx / 5;
            src_rect.top = cy / 5;
            src_rect.right = cx / 5 * 4;
            src_rect.bottom = cy / 5 * 4;
            p_src_rect = &src_rect;
            p_dst_rect = NULL;
            sprintf(bmp_name, "%dx%d_src_%d_%d_%d_%d.bmp", cx, cy, src_rect.left, src_rect.top, src_rect.right, src_rect.bottom);
            
        }
        else if (2 == i) {
            dst_rect.left = cx / 5;
            dst_rect.top = cy / 5;
            dst_rect.right = cx / 5 * 4;
            dst_rect.bottom = cy / 5 * 4;
            p_src_rect = NULL;
            p_dst_rect = &dst_rect;
            sprintf(bmp_name, "%dx%d_dst_%d_%d_%d_%d.bmp", cx, cy, dst_rect.left, dst_rect.top, dst_rect.right, dst_rect.bottom);
            memset(p_capture_data,0, frame_size);
        }
        else if (3 == i) {
            ratio_mode = MWCAP_VIDEO_ASPECT_RATIO_IGNORE;
            p_src_rect = NULL;
            p_dst_rect = NULL;
            cx = cx / 2;
            stride = FOURCC_CalcMinStride(MWFOURCC_BGR24, cx, 2);
            frame_size = FOURCC_CalcImageSize(MWFOURCC_BGR24, cx, cy, stride);
            sprintf(bmp_name, "%dx%d_IGNORE.bmp", cx, cy);
        }
        else if (4 == i) {
            ratio_mode = MWCAP_VIDEO_ASPECT_RATIO_CROPPING;
            p_src_rect = NULL;
            p_dst_rect = NULL;
            sprintf(bmp_name, "%dx%d_CROPPING.bmp", cx, cy);
        }
        else if (5 == i) {
            ratio_mode = MWCAP_VIDEO_ASPECT_RATIO_PADDING;
            p_src_rect = NULL;
            p_dst_rect = NULL;
            sprintf(bmp_name, "%dx%d_PADDING.bmp", cx, cy);
        }


        MWCaptureVideoFrameToVirtualAddressEx(g_channel_handle, MWCAP_VIDEO_FRAME_ID_NEWEST_BUFFERED, (LPBYTE)p_capture_data,
            frame_size, stride, 1, NULL, MWFOURCC_BGR24, cx, cy, 0, 0, NULL, NULL, 0, 100, 0, 100, 0, MWCAP_VIDEO_DEINTERLACE_WEAVE,
            ratio_mode,//
            p_src_rect, p_dst_rect,
            0, 0, MWCAP_VIDEO_COLOR_FORMAT_UNKNOWN, MWCAP_VIDEO_QUANTIZATION_UNKNOWN, MWCAP_VIDEO_SATURATION_UNKNOWN
        );
        WaitForSingleObject(capture_event, INFINITE);

        MWCAP_VIDEO_CAPTURE_STATUS capture_status;
        MWGetVideoCaptureStatus(g_channel_handle, &capture_status);
        create_bitmap(bmp_name, p_capture_data, frame_size, cx, cy);
    }
    MWUnpinVideoBuffer(g_channel_handle, (LPBYTE)p_capture_data);
    printf("End capture.\n");

    
end_and_free:
    MWStopVideoCapture(g_channel_handle);
    if (notify_event) {
        CloseHandle(notify_event);
    }
    if (capture_event) {
        CloseHandle(capture_event);
    }
    if (notify) {
        MWUnregisterNotify(g_channel_handle, notify);
    }
}

void close_channel()
{
    if (g_channel_handle != NULL) {
        MWCloseChannel(g_channel_handle);
        g_channel_handle = NULL;
    }
}
