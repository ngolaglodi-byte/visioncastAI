/************************************************************************************************/
// color_conditioning2.cpp : Defines the entry point for the console application.

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

#define CAPTURE_WIDTH 1920
#define CAPTURE_HEIGHT 1080

HCHANNEL					g_channel_handle = NULL;

void	 show_info();
BOOL	 open_channel(int argc, char* argv[]);
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

    printf("Magewell MWCapture SDK %d.%d.1.%d - color_conditioning2\n", maj, min, build);
    printf("Usage:\n");
    printf("color_conditioning2.exe <channel_index>\n");
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
    if (index >= total_device) {
        printf("index >= total_device\n");
        return false;
    }
    for (int i = 0; i < total_device; i++) {
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
    printf("index out of range\n");
    return false;
}

char bmp_name[128];
uint8_t save_status = 0;
void video_callback(BYTE *frame, int frame_size, UINT64 time_stamp, void* param)
{
    if (save_status == 1) {
        create_bitmap(bmp_name, frame, frame_size, CAPTURE_WIDTH, CAPTURE_HEIGHT);
        save_status = 2;
    }
}

void start_capture()
{
    HANDLE video_handle = MWCreateVideoCapture(g_channel_handle, CAPTURE_WIDTH, CAPTURE_HEIGHT, MWFOURCC_BGR24, 400000, video_callback, NULL);
    if (video_handle == NULL){
        printf("Open Video Capture error!\n");
        return;
    }

    for (MWCAP_VIDEO_PROC_PARAM_TYPE type = MWCAP_VIDEO_PROC_BRIGHTNESS; type <= MWCAP_VIDEO_PROC_SATURATION; type= MWCAP_VIDEO_PROC_PARAM_TYPE(type+1)) {
        long min, max, def,value;
        MWGetVideoProcParamRange(video_handle, type, &min, &max, &def);
        MWGetVideoProcParam(video_handle, type, &value);
        MWSetVideoProcParam(video_handle, type, (min + value)/2);
        if (MWCAP_VIDEO_PROC_BRIGHTNESS == type) {
            sprintf(bmp_name, "BRIGHTNESS_min(%d)_max(%d)_def(%d)_from(%d)to(%d).bmp",min,max,def,value, (min + value) / 2);
        }
        else if (MWCAP_VIDEO_PROC_CONTRAST == type) {
            sprintf(bmp_name, "CONTRAST_min(%d)_max(%d)_def(%d)_from(%d)to(%d).bmp", min, max, def, value, (min + value) / 2);
        }
        else if (MWCAP_VIDEO_PROC_HUE == type) {
            sprintf(bmp_name, "HUE_min(%d)_max(%d)_def(%d)_from(%d)to(%d).bmp", min, max, def, value, (min + value) / 2);
        }
        else {
            sprintf(bmp_name, "SATURATION_min(%d)_max(%d)_def(%d)_from(%d)to(%d).bmp", min, max, def, value, (min + value) / 2);
        }
        printf("capture %s\n",bmp_name);
        Sleep(1000);
        save_status = 1;
        while(1 == save_status){
            Sleep(10);
        }
        MWSetVideoProcParam(video_handle, type, def);
        save_status = 0;
    }
    sprintf(bmp_name, "default.bmp");
    Sleep(1000);
    save_status = 1;
    while (1 == save_status) {
        Sleep(10);
    }
    save_status = 0;
    MWDestoryVideoCapture(video_handle);
    printf("End capture.\n");
}

void close_channel()
{
    if (g_channel_handle != NULL) {
        MWCloseChannel(g_channel_handle);
        g_channel_handle = NULL;
    }
}
