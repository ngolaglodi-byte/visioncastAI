using System;
using System.Windows.Forms;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Drawing;
using System.Threading;
using System.Data;
using System.Drawing.Text;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.Drawing.Printing;

namespace AVCapture2CSharp
{
    public class MWCapture
    {
        //video device 
        protected IntPtr m_hVideoChannel = IntPtr.Zero;
        protected IntPtr m_hD3DRenderer = IntPtr.Zero;
        protected IntPtr m_hAudioRender = IntPtr.Zero;

        //video params
        protected IntPtr m_hWnd = IntPtr.Zero;
        //protected CAPTURE_PARAMS m_capParams;

        // video device index
        protected int m_nCurrentIndex = -1;
        protected Boolean m_bIsCapture = false;

        protected int m_nBoard = 0;
        protected int m_nChannelIndex = 0;

        // capture thread
        Thread m_capThread = null;
        IntPtr m_hExitEvent = IntPtr.Zero;

        protected IntPtr m_hVideo;
        protected IntPtr m_hAudio;
        LibMWCapture.VIDEO_CAPTURE_STDCALLBACK video_callback;
        LibMWCapture.VIDEO_CAPTURE_CALLBACK video_callback1;
        LibMWCapture.AUDIO_CAPTURE_STDCALLBACK audio_callback;

        protected static int llCount;
        protected static long m_llCurrentTime;
        protected static long m_llRefTime;
        protected static double m_dfps;

        public static void video_callback_sub(IntPtr pbFrame,int cbFrame,ulong u64TimeStamp,IntPtr pParam) 
        {
            LibMWMedia.MWD3DRendererPushFrame(pParam, pbFrame, 720 * 4);
            llCount += 1;
            if (llCount >= 10) {
                m_llCurrentTime = (UInt32)Libkernel32.GetTickCount();
                m_dfps = (Double)(llCount * 1000) / (m_llCurrentTime - m_llRefTime);
                m_llRefTime = m_llCurrentTime;
                llCount = 0;
            }
        }

        public static void audio_callback_sub(IntPtr pbFrame, int cbFrame,ulong u64TimeStamp, IntPtr pParam)
        {
            LibMWMedia.MWDSoundRendererPushFrame(pParam, pbFrame, cbFrame);
        }

        public MWCapture()
        {

        }

        static public void Init()
        {
            LibMWCapture.MWCaptureInitInstance();
        }

        static public void Exit()
        {
            LibMWCapture.MWCaptureExitInstance();
        }

        static public Boolean RefreshDevices()
        {
            LibMWCapture.MW_RESULT mr;
            mr = LibMWCapture.MWRefreshDevice();
            if (mr != LibMWCapture.MW_RESULT.MW_SUCCEEDED)
                return false;

            return true;
        }

        static public int GetChannelCount()
        {
            return LibMWCapture.MWGetChannelCount();
        }

        static public void GetChannelInfobyIndex(int nChannelIndex, ref LibMWCapture.MWCAP_CHANNEL_INFO channelInfo)
        {
            int iSize = Marshal.SizeOf(typeof(LibMWCapture.MWCAP_CHANNEL_INFO));
            IntPtr pChannelInfo = Marshal.AllocCoTaskMem(iSize);
            LibMWCapture.MWGetChannelInfoByIndex(nChannelIndex, pChannelInfo);
            channelInfo = (LibMWCapture.MWCAP_CHANNEL_INFO)Marshal.PtrToStructure(pChannelInfo, typeof(LibMWCapture.MWCAP_CHANNEL_INFO));
            Marshal.FreeCoTaskMem(pChannelInfo);

            return;
        }

        public Boolean OpenVideoChannel(int nChannelIndex, UInt32 dwFourcc, int cx, int cy, UInt32 nFrameDuration, IntPtr hWnd, Rectangle rcPanel)
        {
            // open video device
            LibMWCapture.MW_RESULT mr;
            int iSize = Marshal.SizeOf(typeof(LibMWCapture.MWCAP_CHANNEL_INFO));
            IntPtr pChannelInfo = Marshal.AllocCoTaskMem(iSize);
            mr = LibMWCapture.MWGetChannelInfoByIndex(nChannelIndex, pChannelInfo);
            LibMWCapture.MWCAP_CHANNEL_INFO channelInfo = (LibMWCapture.MWCAP_CHANNEL_INFO)Marshal.PtrToStructure(pChannelInfo, typeof(LibMWCapture.MWCAP_CHANNEL_INFO));
            Marshal.FreeCoTaskMem(pChannelInfo);

            ushort[] wpath=new ushort[512];
            IntPtr pwpath = GCHandle.Alloc(wpath, GCHandleType.Pinned).AddrOfPinnedObject();
            LibMWCapture.MWGetDevicePath(nChannelIndex, pwpath);

            m_nBoard = channelInfo.byBoardIndex;
            m_nChannelIndex = channelInfo.byChannelIndex;

            m_hVideoChannel = LibMWCapture.MWOpenChannelByPath(pwpath);
            if (m_hVideoChannel == IntPtr.Zero)
                return false;

            video_callback = new LibMWCapture.VIDEO_CAPTURE_STDCALLBACK(video_callback_sub);
            video_callback1 = new LibMWCapture.VIDEO_CAPTURE_CALLBACK(video_callback_sub);
            // create video renderer

            bool t_b_reverse = false;
            if (dwFourcc == MWCap_FOURCC.MWCAP_FOURCC_BGR24 || dwFourcc == MWCap_FOURCC.MWCAP_FOURCC_BGRA)
            {
                t_b_reverse = true;
            }
            m_hD3DRenderer = LibMWMedia.MWCreateD3DRenderer(cx, cy, dwFourcc,t_b_reverse,false, hWnd);

            if (m_hD3DRenderer == IntPtr.Zero)
            {
                return false;
            }

            llCount = 0;
            m_llCurrentTime=m_llRefTime = (UInt32)Libkernel32.GetTickCount();
            uint fourcc = (uint)dwFourcc;
            int frameduration = (int)nFrameDuration;
            //m_hVideo = LibMWCapture.MWCreateVideoCaptureWithStdCallBack(m_hVideoChannel, cx, cy, fourcc, frameduration, video_callback, m_hD3DRenderer);
            m_hVideo = LibMWCapture.MWCreateVideoCapture(m_hVideoChannel, cx, cy, (int)fourcc, frameduration, video_callback1, m_hD3DRenderer);

            if (m_hVideo == IntPtr.Zero)
            {
                return false;
            }

            audio_callback = new LibMWCapture.AUDIO_CAPTURE_STDCALLBACK(audio_callback_sub);
            m_hAudioRender = LibMWMedia.MWCreateDSoundRenderer(48000, 2, 480, 10);
            if (m_hAudioRender == IntPtr.Zero) {
                return false;
            }
            //m_hAudio = LibMWCapture.MWCreateAudioCaptureWithStdCallBack(m_hVideoChannel, LibMWCapture.MWCAP_AUDIO_CAPTURE_NODE.MWCAP_AUDIO_CAPTURE_NODE_DEFAULT, 48000, 16, 2,audio_callback, m_hAudioRender);
            //if (m_hAudio == IntPtr.Zero) {
            //    return false;            
            //}
            return true;
        }

        public void Destory()
        {
            if (m_hVideo != IntPtr.Zero) {
                LibMWCapture.MWDestoryVideoCapture(m_hVideo);
                m_hVideo = IntPtr.Zero;
            }

            if (m_hAudio != IntPtr.Zero) {
                LibMWCapture.MWDestoryAudioCapture(m_hAudio);
                m_hAudio = IntPtr.Zero;
            }

            if (m_hVideoChannel != IntPtr.Zero) {
                LibMWCapture.MWCloseChannel(m_hVideoChannel);
                m_hVideoChannel = IntPtr.Zero;
            }

            if (m_hD3DRenderer != IntPtr.Zero) {
                LibMWMedia.MWDestroyD3DRenderer(m_hD3DRenderer);
                m_hD3DRenderer = IntPtr.Zero;
            }

            if (m_hAudioRender != IntPtr.Zero) {
                LibMWMedia.MWDestroyDSoundRenderer(m_hAudioRender);
                m_hAudioRender = IntPtr.Zero;
            }
        }

        public double GetFps() 
        {
            return m_dfps;
        }
    }
}
