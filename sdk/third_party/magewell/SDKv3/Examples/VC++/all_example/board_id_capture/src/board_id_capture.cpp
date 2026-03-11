/************************************************************************************************/
// board_id_capture.cpp : Defines the entry point for the console application.

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

    printf("Magewell MWCapture SDK %d.%d.1.%d - board_id_capture\n", maj, min, build);
    printf("just support pro device\n");
    printf("Usage:\n");
    printf("board_id_capture.exe <board_id:channel_id>\n");
}

int get_id(char c)
{
    if (c >= '0' && c <= '9')
        return (int)(c - '0');
    if (c >= 'a' && c <= 'f')
        return (int)(c - 'a' + 10);
    if (c >= 'A' && c <= 'F')
        return (int)(c - 'A' + 10);
    return 0;
}

BOOL open_channel(int argc, char* argv[]) 
{
    int board_id = 0;
    int channel_id = 0;
    if ((argc == 2) && (strlen(argv[1]))) {
        board_id = get_id(argv[1][0]);
        channel_id = get_id(argv[1][2]);
    }
    g_channel_handle = MWOpenChannel(board_id, channel_id);
    if (NULL == g_channel_handle) {
        printf("open %d:%d fail\n", board_id, channel_id);
        return false;
    }
    return true;
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

        MWCaptureVideoFrameToVirtualAddressEx(g_channel_handle, MWCAP_VIDEO_FRAME_ID_NEWEST_BUFFERED, (LPBYTE)p_capture_data,
            frame_size, stride, 1, NULL, MWFOURCC_BGR24, cx, cy, 0, 0, NULL, NULL, 0, 100, 0, 100, 0, MWCAP_VIDEO_DEINTERLACE_WEAVE,
            MWCAP_VIDEO_ASPECT_RATIO_IGNORE,//
            NULL, NULL,
            0, 0, MWCAP_VIDEO_COLOR_FORMAT_UNKNOWN, MWCAP_VIDEO_QUANTIZATION_UNKNOWN, MWCAP_VIDEO_SATURATION_UNKNOWN
        );
        WaitForSingleObject(capture_event, INFINITE);

        MWCAP_VIDEO_CAPTURE_STATUS capture_status;
        MWGetVideoCaptureStatus(g_channel_handle, &capture_status);
    }
    create_bitmap("board_id_capture.bmp", p_capture_data, frame_size, cx, cy);
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
