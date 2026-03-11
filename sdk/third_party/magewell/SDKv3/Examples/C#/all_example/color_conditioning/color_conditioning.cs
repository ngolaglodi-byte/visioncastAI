
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Linq;
using System.Windows.Forms;
using System.Drawing;
using MWModle;

namespace ColorConditioning
{
    class CColorConditioning
    {
        IntPtr m_channel_handle = IntPtr.Zero;
        LibMWCapture.MWCAP_VIDEO_SIGNAL_STATUS m_video_signal_status =new LibMWCapture.MWCAP_VIDEO_SIGNAL_STATUS();
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
            Console.WriteLine("Magewell MWCapture SDK "+maj+"."+min+".1."+build+" - color_conditioning\n");
            Console.WriteLine("just support pro device\n");
            Console.WriteLine("Usage:\n");
            Console.WriteLine("color_conditioning_csharp.exe <channel index>\n");
        }


        public Boolean open_channel(int index)
        {
            int total_device = LibMWCapture.MWGetChannelCount();
            if (total_device <= 0){
                Console.WriteLine("not find magewell device or not install device\n");
                return false;
            }
            for (int i = 0; i < total_device; i++){
                LibMWCapture.MWCAP_CHANNEL_INFO info = new LibMWCapture.MWCAP_CHANNEL_INFO();
                LibMWCapture.MW_RESULT mr = LibMWCapture.MWGetChannelInfoByIndex(i, ref info);
                if ((LibMWCapture.MW_RESULT.MW_SUCCEEDED == mr) && (info.wFamilyID == (UInt16)LibMWCapture.MW_FAMILY_ID.MW_FAMILY_ID_PRO_CAPTURE)){
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
            }
            Console.WriteLine("index out of range\n");
            return false;
        }

        //check the input status and related information of the video signal
        public Boolean check_signal()
        {
            LibMWCapture.MWGetVideoSignalStatus(m_channel_handle, ref m_video_signal_status);
            switch (m_video_signal_status.state){
                case LibMWCapture.MWCAP_VIDEO_SIGNAL_STATE.MWCAP_VIDEO_SIGNAL_NONE:
                    Console.WriteLine("Input signal status: NONE\n");
                    break;
                case LibMWCapture.MWCAP_VIDEO_SIGNAL_STATE.MWCAP_VIDEO_SIGNAL_UNSUPPORTED:
                    Console.WriteLine("Input signal status: Unsupported\n");
                    break;
                case LibMWCapture.MWCAP_VIDEO_SIGNAL_STATE.MWCAP_VIDEO_SIGNAL_LOCKING:
                    Console.WriteLine("Input signal status: Locking\n");
                    break;
                case LibMWCapture.MWCAP_VIDEO_SIGNAL_STATE.MWCAP_VIDEO_SIGNAL_LOCKED:
                    Console.WriteLine("Input signal status: Locked\n");
                    break;
            }

            if (m_video_signal_status.state == LibMWCapture.MWCAP_VIDEO_SIGNAL_STATE.MWCAP_VIDEO_SIGNAL_LOCKED){
                Console.WriteLine("Input signal resolution: "+ m_video_signal_status.cx+" x "+ m_video_signal_status.cy);
                double fps = (m_video_signal_status.bInterlaced == 1) ? (double)20000000 / m_video_signal_status.dwFrameDuration : (double)10000000 / m_video_signal_status.dwFrameDuration;
                Console.WriteLine("Input signal fps: "+ String.Format("{0:F}", fps));
                Console.WriteLine("Input signal interlaced: "+ m_video_signal_status.bInterlaced);
                Console.WriteLine("Input signal frame segmented:"+ m_video_signal_status.bSegmentedFrame);
                return true;
            }
            return false;
        }

        public void start_capture()
        {
            IntPtr capture_event = Libkernel32.CreateEvent(IntPtr.Zero, 0, 0, IntPtr.Zero);
            if (IntPtr.Zero == capture_event){
                Console.WriteLine("create event fail\n");
                return;
            }
            if (LibMWCapture.MWStartVideoCapture(m_channel_handle, capture_event) != LibMWCapture.MW_RESULT.MW_SUCCEEDED){
                Console.WriteLine("Open Video Capture error!\n");
                Libkernel32.CloseHandle(capture_event);
                return;
            }
            IntPtr notify_event = Libkernel32.CreateEvent(IntPtr.Zero, 0, 0, IntPtr.Zero);
            UInt64 notify = LibMWCapture.MWRegisterNotify(m_channel_handle, notify_event, LibMWCapture.MWCAP_NOTIFY_VIDEO_FRAME_BUFFERED);
            if (notify == 0){
                Console.WriteLine("register notify fail\n");
                goto end_and_free;
            }
            int cx = m_video_signal_status.cx;
            int cy = m_video_signal_status.cy;
            UInt32 stride = MWFOURCC.FOURCC_CalcMinStride(MWFOURCC.MWFOURCC_BGR24, cx, 2);
            UInt32 frame_size = MWFOURCC.FOURCC_CalcImageSize(MWFOURCC.MWFOURCC_BGR24, cx, cy, stride);

            Byte[] capture_data = new Byte[frame_size];
            if (null == capture_data){
                Console.WriteLine("malloc fail\n");
                goto end_and_free;
            }
            IntPtr p_capture_status = Marshal.AllocCoTaskMem(Marshal.SizeOf(typeof(LibMWCapture.MWCAP_VIDEO_CAPTURE_STATUS)));
            LibMWCapture.MWPinVideoBuffer(m_channel_handle, capture_data, frame_size);
            Int16 contrast = 100;
            Int16 brightness = 0;
            Int16 saturation = 100;
            Int16 hue = 0;
            string bmp_name;
            for (int i = 0; i < 5; i++)
            {
                Libkernel32.WaitForSingleObject(notify_event, 0xFFFFFFFF);

                UInt64 notify_status = 0;
                if (LibMWCapture.MWGetNotifyStatus(m_channel_handle, notify, ref notify_status) != LibMWCapture.MW_RESULT.MW_SUCCEEDED){
                    continue;
                }
                if (0 == (notify_status & LibMWCapture.MWCAP_NOTIFY_VIDEO_FRAME_BUFFERED)){
                    continue;
                }

                if (1 == i){
                    contrast = 50;
                    brightness = 0;
                    saturation = 100;
                    hue = 0;
                }
                else if (2 == i){
                    contrast = 100;
                    brightness = -10;
                    saturation = 100;
                    hue = 0;
                }
                else if (3 == i){
                    contrast = 100;
                    brightness = 0;
                    saturation = 50;
                    hue = 0;
                }
                else if (4 == i){
                    contrast = 100;
                    brightness = 0;
                    saturation = 100;
                    hue = 10;
                }
                bmp_name = "c" + contrast + "_b" + brightness + "_s" + saturation + "_h" + hue + ".bmp";

                LibMWCapture.MWCaptureVideoFrameToVirtualAddressEx(m_channel_handle, LibMWCapture.MWCAP_VIDEO_FRAME_ID_NEWEST_BUFFERED, capture_data,
                    frame_size, stride, 1, 0, MWFOURCC.MWFOURCC_BGR24, cx, cy, 0, 0, 0, null, 0,
                    contrast, brightness, saturation, hue,
                    LibMWCapture.MWCAP_VIDEO_DEINTERLACE_MODE.MWCAP_VIDEO_DEINTERLACE_WEAVE, 
                    LibMWCapture.MWCAP_VIDEO_ASPECT_RATIO_CONVERT_MODE.MWCAP_VIDEO_ASPECT_RATIO_IGNORE, 
                    null, null, 0, 0, 
                    LibMWCapture.MWCAP_VIDEO_COLOR_FORMAT.MWCAP_VIDEO_COLOR_FORMAT_UNKNOWN,
                    LibMWCapture.MWCAP_VIDEO_QUANTIZATION_RANGE.MWCAP_VIDEO_QUANTIZATION_UNKNOWN,
                    LibMWCapture.MWCAP_VIDEO_SATURATION_RANGE.MWCAP_VIDEO_SATURATION_UNKNOWN
                );
                Libkernel32.WaitForSingleObject(capture_event, 0xffffffff);

                LibMWCapture.MWGetVideoCaptureStatus(m_channel_handle, p_capture_status);
                CBmp.create_bmp_byte(bmp_name, capture_data, frame_size, cx, cy);
            }
            LibMWCapture.MWUnpinVideoBuffer(m_channel_handle, capture_data);
            Console.WriteLine("End capture.\n");


            end_and_free:
            LibMWCapture.MWStopVideoCapture(m_channel_handle);
            if (IntPtr.Zero != notify_event){
                Libkernel32.CloseHandle(notify_event);
            }
            if (IntPtr.Zero != capture_event){
                Libkernel32.CloseHandle(capture_event);
            }
            if (0 != notify){
                LibMWCapture.MWUnregisterNotify(m_channel_handle, notify);
            }
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
