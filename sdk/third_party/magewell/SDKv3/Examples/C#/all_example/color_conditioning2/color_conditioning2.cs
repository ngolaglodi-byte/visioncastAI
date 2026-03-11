
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Linq;
using System.Windows.Forms;
using System.Drawing;
using MWModle;
using System.Threading;
namespace ColorConditioning2
{
    class CColorConditioning2
    {
        const int CAPTURE_WIDTH = 1920;
        const int CAPTURE_HEIGHT = 1080;
        IntPtr m_channel_handle = IntPtr.Zero;
        string m_bmp_name;
        int save_status = 0;
        public static void init()
        {
            LibMWCapture.MWCaptureInitInstance();
        }
        public static void deinit()
        {
            LibMWCapture.MWCaptureExitInstance();
        }
        public static void show_info()
        {
            Byte maj = 0;
            Byte min = 0;
            UInt16 build = 0;
            LibMWCapture.MWGetVersion(ref maj, ref min, ref build);
            Console.WriteLine("Magewell MWCapture SDK "+maj+"."+min+".1."+build+" - color_conditioning2\n");
            Console.WriteLine("Usage:\n");
            Console.WriteLine("color_conditioning2_csharp.exe <channel index>\n");
        }


        public Boolean open_channel(int index)
        {
            int total_device = LibMWCapture.MWGetChannelCount();
            if (total_device <= 0){
                Console.WriteLine("not find magewell device or not install device\n");
                return false;
            }
            for (int i = 0; i < total_device; i++){
                if (0 != index){
                    index--;
                    continue;
                }
                ushort[] path = new ushort[128];
                LibMWCapture.MWGetDevicePath(i, path);
                m_channel_handle = LibMWCapture.MWOpenChannelByPath(path);
                if (IntPtr.Zero == m_channel_handle){
                    Console.WriteLine("open fail\n");
                    return false;
                }
                return true;
            }
            Console.WriteLine("index out of range\n");
            return false;
        }

        void video_frame_callback(IntPtr frame, int frame_size, UInt64 time_stamp, IntPtr param)
        {
            if (save_status == 1){
                CBmp.create_bmp_ptr(m_bmp_name, frame, (uint)frame_size, CAPTURE_WIDTH, CAPTURE_HEIGHT);
                save_status = 2;
            }
        }

        public void start_capture()
        {
            LibMWCapture.VIDEO_CAPTURE_CALLBACK video_callback = new LibMWCapture.VIDEO_CAPTURE_CALLBACK(video_frame_callback);
            IntPtr video_handle = LibMWCapture.MWCreateVideoCapture(m_channel_handle, CAPTURE_WIDTH, CAPTURE_HEIGHT, (int)MWFOURCC.MWFOURCC_BGR24, 400000, video_callback, IntPtr.Zero);
            if (video_handle == null){
                Console.WriteLine("Open Video Capture error!\n");
                return;
            }

            for (LibMWCapture.MWCAP_VIDEO_PROC_PARAM_TYPE type = LibMWCapture.MWCAP_VIDEO_PROC_PARAM_TYPE.MWCAP_VIDEO_PROC_BRIGHTNESS; 
                type <= LibMWCapture.MWCAP_VIDEO_PROC_PARAM_TYPE.MWCAP_VIDEO_PROC_SATURATION; 
                type = (LibMWCapture.MWCAP_VIDEO_PROC_PARAM_TYPE)(type + 1)){
                long min = 0, max = 0, def = 0, value = 0;
                LibMWCapture.MWGetVideoProcParamRange(video_handle, type, ref min, ref max, ref def);
                LibMWCapture.MWGetVideoProcParam(video_handle, type, ref value);
                LibMWCapture.MWSetVideoProcParam(video_handle, type, (min + value) / 2);
                if (LibMWCapture.MWCAP_VIDEO_PROC_PARAM_TYPE.MWCAP_VIDEO_PROC_BRIGHTNESS == type){
                    m_bmp_name = "BRIGHTNESS_min(" + (int)min + ")_max(" + (int)max + ")_def(" + (int)def + ")_from(" + (int)value + ")to(" + (int)(min + value) / 2 + ").bmp";
                }
                else if (LibMWCapture.MWCAP_VIDEO_PROC_PARAM_TYPE.MWCAP_VIDEO_PROC_CONTRAST == type){
                    m_bmp_name = "CONTRAST_min(" + (int)min + ")_max(" + (int)max + ")_def(" + (int)def + ")_from(" + (int)value + ")to(" + (int)(min + value) / 2 + ").bmp";
                }
                else if (LibMWCapture.MWCAP_VIDEO_PROC_PARAM_TYPE.MWCAP_VIDEO_PROC_HUE == type){
                    m_bmp_name = "HUE_min(" + (int)min + ")_max(" + (int)max + ")_def(" + (int)def + ")_from(" + (int)value + ")to(" + (int)(min + value) / 2 + ").bmp";
                }
                else{
                    m_bmp_name = "SATURATION_min(" + (int)min + ")_max(" + (int)max + ")_def(" + (int)def + ")_from(" + (int)value + ")to(" + (int)(min + value) / 2 + ").bmp";
                }
                Console.WriteLine("capture "+ m_bmp_name);
                Thread.Sleep(1000);
                save_status = 1;
                while (1 == save_status){
                    Thread.Sleep(10);
                }
                LibMWCapture.MWSetVideoProcParam(video_handle, type, def);
                save_status = 0;
            }
            m_bmp_name = "default.bmp";
            Thread.Sleep(1000);
            save_status = 1;
            while (1 == save_status){
                Thread.Sleep(10);
            }
            save_status = 0;
            LibMWCapture.MWDestoryVideoCapture(video_handle);
            Console.WriteLine("End capture.\n");
        }

        public void close_channel()
        {
            if (m_channel_handle != IntPtr.Zero){
                LibMWCapture.MWCloseChannel(m_channel_handle);
                m_channel_handle = IntPtr.Zero;
            }
        }
    }
}
