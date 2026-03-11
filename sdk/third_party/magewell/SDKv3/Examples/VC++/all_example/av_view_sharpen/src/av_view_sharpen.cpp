/************************************************************************************************/
// scale_capture.cpp : Defines the entry point for the console application.

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

#include "imgui.h"
#include "imgui_glfw/imgui_impl_glfw.h"
#include "imgui_opengl3/imgui_impl_opengl3.h"
#include "opengl_render/opengl_render.h"
#include "mw_capture/mw_capture.h"
// using gl3w to load opengl functions
#include "GL/glew.h"

// include glfw3.h after opengl definitions
#include "GLFW/glfw3.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

#define MAX_CAPTURE_DEVICE 32
#include "dsound_player/dsound_player.h"

GLFWwindow* g_p_window = NULL;
CMWCapture *g_p_mw_capture = NULL;
MWOpenGLRender *g_p_video_render = NULL;
CDSoundPlayer  *g_p_audio_player = NULL;
char g_device_name[MAX_CAPTURE_DEVICE][128];
int g_device_index[MAX_CAPTURE_DEVICE];
unsigned char *g_p_sharpen_frame = NULL;
int g_device_num = 0;
int g_render_count = 0;
int g_sharpen_value = 0;
bool create_windows();

bool imgui_init();
bool imgui_ui();
bool imgui_render();
void imgui_deinit();

void list_device();
void sharpen_frame(unsigned char *p_frame);
bool start_capture(int index);
void stop_capture();

int main(int argc, char* argv[])
{
    CMWCapture::init();
    create_windows();
    imgui_init();

    st_frame_t *p_frame = NULL;
    mw_render_ctrl_t ctrl = {0};
    while (!glfwWindowShouldClose(g_p_window)) {
        imgui_ui();
        glfwGetFramebufferSize(g_p_window, (int*)&ctrl.display_w, (int *)&ctrl.display_h);
        if (g_p_mw_capture) {
            int no_data_times = 0;
            p_frame = NULL;
            while (NULL == p_frame){
                p_frame = g_p_mw_capture->m_p_video_buffer->get_frame_to_render();
                if(p_frame){
                    break;
                }
                Sleep(1);
                no_data_times++;
                if (no_data_times >= 1000000) {
                    stop_capture();
                    break;
                }
            }
        }
        if (g_p_mw_capture && p_frame && g_p_video_render) {
            if (g_sharpen_value) {
                if (NULL == g_p_sharpen_frame) {
                    g_p_sharpen_frame = (unsigned char*)malloc(p_frame->buffer_len);
                }
                sharpen_frame(p_frame->p_buffer);
                g_p_video_render->render(g_p_sharpen_frame, &ctrl);
            }
            else {
                g_p_video_render->render(p_frame->p_buffer, &ctrl);
            }

            g_render_count++;
        }
        else {
            glClearColor(0.2, 0.3, 0.3, 1.0);
        }
        imgui_render();
        glfwSwapBuffers(g_p_window);
        glfwPollEvents();
    }
    if (g_p_audio_player) {
        delete g_p_audio_player;
    }
    if (g_p_video_render) {
        delete g_p_video_render;
    }
    if (g_p_mw_capture) {
        delete g_p_mw_capture;
    }
    CMWCapture::exit();
    imgui_deinit();
    glfwDestroyWindow(g_p_window);
    glfwTerminate();
    return 0;
}
void sharpen_yuv420(unsigned char *p_frame) {
    int width;// = g_p_mw_capture->m_width;
    int height;// = g_p_mw_capture->m_height;
    g_p_mw_capture->get_resolution(&width, &height);
    int loop_width = width - 1;
    int loop_height = height - 1;
    unsigned char *p_sharpen_frame = g_p_sharpen_frame;
    memcpy(p_sharpen_frame, p_frame, width);
    memcpy(p_sharpen_frame + width*(height - 1), p_frame + width*(height - 1), width);
    p_frame += width;
    p_sharpen_frame += width;
    for (int i = 1; i < loop_height; i++) {
        p_sharpen_frame[0] = p_frame[0];
        p_frame++;
        p_sharpen_frame++;
        for (int j = 1; j < loop_width; j++) {
            short temp = (short)(p_frame[0] * (10 + g_sharpen_value * 9) / 10 -
                (p_frame[-1- width] + p_frame[-width] + p_frame[1 - width] +
                    p_frame[- 1] + p_frame[1] +
                    p_frame[-1 + width] + p_frame[width] + p_frame[1 + width]) * g_sharpen_value / 10);
            if (temp > 255) {
                p_sharpen_frame[0] = 255;
            }
            else if (temp < 0) {
                p_sharpen_frame[0] = 0;
            }
            else {
                p_sharpen_frame[0] = (unsigned char)(temp);
            }
            p_frame++;
            p_sharpen_frame++;
        }
        p_sharpen_frame[0] = p_frame[0];
        p_frame++;
        p_sharpen_frame++;
    }
    memcpy(p_sharpen_frame, p_frame, width * height / 2);
}

void sharpen_yuyv(unsigned char *p_frame)
{
    int width;// = g_p_mw_capture->m_width;
    int height;// = g_p_mw_capture->m_height;
    g_p_mw_capture->get_resolution(&width, &height);
    int stride = width*2;// g_p_mw_capture->m_width * 2;
    int loop_width = width - 1;
    int loop_height = height - 1;
    unsigned char *p_sharpen_frame = g_p_sharpen_frame;
    memcpy(p_sharpen_frame, p_frame, stride);
    memcpy(p_sharpen_frame+ stride*(height-1), p_frame + stride*(height - 1), stride);
    p_frame +=width*2;
    p_sharpen_frame += width*2;
    
    for (int i = 1; i < loop_height; i++) {
        p_sharpen_frame[0] = p_frame[0];
        p_sharpen_frame[1] = p_frame[1];
        p_frame += 2;
        p_sharpen_frame += 2;
        for (int j = 1; j < loop_width; j++) {
            short temp = (short)(p_frame[0] * (10 + g_sharpen_value * 9) / 10 -
                (p_frame[-2 - stride] + p_frame[-stride] + p_frame[2 - stride] +
                    p_frame[-2] + p_frame[2] +
                    p_frame[-2 + stride] + p_frame[stride] + p_frame[2 + stride]) * g_sharpen_value / 10);
            if (temp > 255) {
                p_sharpen_frame[0] = 255;
            }
            else if (temp < 0) {
                p_sharpen_frame[0] = 0;
            }
            else {
                p_sharpen_frame[0] = (unsigned char)(temp);
            }
            p_sharpen_frame[1] = p_frame[1];
            p_frame += 2;
            p_sharpen_frame += 2;
        }
        p_sharpen_frame[0] = p_frame[0];
        p_sharpen_frame[1] = p_frame[1];
        p_frame += 2;
        p_sharpen_frame += 2;
    }
}
void sharpen_frame(unsigned char *p_frame)
{
    DWORD mw_fourcc;
    g_p_mw_capture->get_mw_fourcc(&mw_fourcc);
    if (MWFOURCC_YUY2 == mw_fourcc) {
        return sharpen_yuyv(p_frame);
    }else if (MWFOURCC_NV12 == mw_fourcc) {
        return sharpen_yuv420(p_frame);
    }
}
static void glfw_error_callback(int error, const char* des)
{
    printf("glfw error %d: %s\n", error, des);
}
bool create_windows()
{
    glfwSetErrorCallback(glfw_error_callback);
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    g_p_window = glfwCreateWindow(960, 540, "av_view", NULL, NULL);
    //HWND handle = glfwGetWin32Window(g_p_window);
    glfwMakeContextCurrent(g_p_window);
    glfwSwapInterval(1);
    glewInit();
    glClearColor(0.2, 0.3, 0.3, 1.0);
    return false;
}
bool imgui_init()
{
    IMGUI_CHECKVERSION();
    ImGuiContext* m_p_imgui_ctx = ImGui::CreateContext();
    ImGui::StyleColorsDark();

    const char* t_cs_glsl_version = "#version 130";
    ImGui_ImplOpenGL3_Init(t_cs_glsl_version);
    ImGui_ImplGlfw_InitForOpenGL(g_p_window, true);
    return false;
}

void imgui_deinit()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
}
void list_device()
{
    int capture_num;
    MWCAP_CHANNEL_INFO mci;
    CMWCapture::refresh_devices();
    g_device_num = 0;
    capture_num = CMWCapture::get_channel_count();
    for (int i = 0; i < capture_num; i++) {
        CMWCapture::get_channel_info_by_index(i, &mci);
//         if (mci.wFamilyID == MW_FAMILY_ID_USB_CAPTURE) {
//             continue;
//         }
        sprintf(g_device_name[g_device_num], "%d %s", i, mci.szProductName);
        g_device_index[g_device_num] = i;
        g_device_num++;
        if (g_device_num >= MAX_CAPTURE_DEVICE) {
            printf("too many capture, just list %d\n", MAX_CAPTURE_DEVICE);
            break;
        }
    }
}
bool start_capture(int index)
{
    static int render_w = 0;
    static int render_h = 0;
    static DWORD render_fourcc;
	static bool render_mirror;
	static bool render_reverse;
    g_p_mw_capture = CMWCapture::mw_capture_factory(index);// mw_capture_create(index);
    if (NULL == g_p_mw_capture) {
        return false;
    }
	//g_p_mw_capture->set_mw_fourcc(MWFOURCC_NV12);
	if (!g_p_mw_capture->start_capture(true, true)) {
		return false;
	}
    if (NULL == g_p_video_render) {
        g_p_video_render = new MWOpenGLRender();
    }
    int capture_width;
    int capture_height;
    int capture_sample_rate;
    int capture_channel_num;

    bool capture_mirror;
	bool capture_reverse;
	DWORD capture_fourcc;
    g_p_mw_capture->get_resolution(&capture_width, &capture_height);
    g_p_mw_capture->get_mw_fourcc(&capture_fourcc);
	g_p_mw_capture->get_mirror_and_reverse(&capture_mirror, &capture_reverse);
    if (g_p_video_render && ((render_w != capture_width) || (render_h != capture_height) || (render_fourcc != capture_fourcc) || (render_mirror != capture_mirror) || (render_reverse != capture_reverse))) {
        mw_render_init_t rinit = { 0 };
        rinit.width = capture_width;
        rinit.height = capture_height;
		rinit.is_mirror = capture_mirror;
		rinit.is_reverse = capture_reverse;
        rinit.render_fourcc = capture_fourcc;
        g_p_video_render->open(&rinit);
        render_w = capture_width;
        render_h = capture_height;
        render_fourcc = capture_fourcc;
		render_mirror = capture_mirror;
		render_reverse = capture_reverse;
		if (g_p_sharpen_frame) {
			free(g_p_sharpen_frame);
			g_p_sharpen_frame = NULL;
		}
    }
    if (NULL == g_p_audio_player) {
        g_p_audio_player = new CDSoundPlayer();
        g_p_mw_capture->get_audio_channels(&capture_channel_num);
        g_p_mw_capture->get_audio_sample_rate(&capture_sample_rate);
        g_p_audio_player->set_param(capture_sample_rate, capture_channel_num);
        g_p_audio_player->play(g_p_mw_capture->m_p_audio_buffer);
        //g_audio_play_thread = CreateThread(NULL, 0, audio_player, NULL, 0, NULL);
    }
	return true;
}
void stop_capture()
{
    if (g_p_audio_player) {
        g_p_audio_player->stop();
        delete g_p_audio_player;
        g_p_audio_player = NULL;
    }

    if (g_p_mw_capture) {
        delete g_p_mw_capture;
    }
    g_p_mw_capture = NULL;
}
float get_render_fps()
{
    static float fps = 0.0;
    static int prev_render_count = 0;
    static int prev_time = 0;
    long long now_time = GetTickCount();
    int diff = now_time - prev_time;
    if (diff <= 1000) {
        return fps;
    }
    fps = (g_render_count - prev_render_count)*1000.0 / diff;
    prev_time = now_time;
    prev_render_count = g_render_count;
    return fps;
}
void imgui_draw_fps()
{
    bool m_b_open = true;
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 pos;
    ImVec2 size;
    size.x = 0;
    size.y = 0;
    ImGuiWindowFlags m_imgui_flags = ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoNav;
    pos.x = io.DisplaySize.x - 260;// -size.x - 10;
    pos.y = io.DisplaySize.y - 30;// -size.y - 10;
    ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(size, ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.5);
    ImGui::Begin("status", &m_b_open, m_imgui_flags);
    ImGui::Text("capture fps:%0.2f|render fps:%0.2f", g_p_mw_capture ? g_p_mw_capture->get_capture_fps():0.0, get_render_fps());
    ImGui::End();

}
bool imgui_ui()
{
    static bool open_device = false;
    bool in_device = false;
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    static bool choose[MAX_CAPTURE_DEVICE] = { NULL };
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Devices")) {
            if (!open_device) {
                list_device();
                open_device = true;
            }
            if (NULL == g_p_mw_capture) {
                memset(choose, 0, sizeof(choose));
            }
            in_device = true;
            for (int i = 0; i < g_device_num; i++) {
                if (ImGui::MenuItem(g_device_name[i], NULL, &choose[i])) {
                    if (choose[i]) {
                        memset(choose, 0, sizeof(choose));
                        choose[i] = true;
                        stop_capture();
						if (!start_capture(g_device_index[i])) {
							stop_capture();
							choose[i] = false;
						}
                    }
                    else {
                        stop_capture();
                    }
                }
            }
            ImGui::EndMenu();
        }
        ImGui::Text("     sharpen");
        ImGui::SliderInt("", &g_sharpen_value, 0, 100);
        ImGui::EndMainMenuBar();
    }
    imgui_draw_fps();
    if (!in_device && open_device) {
        open_device = false;
    }
    return false;
}

bool imgui_render()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    return false;
}
