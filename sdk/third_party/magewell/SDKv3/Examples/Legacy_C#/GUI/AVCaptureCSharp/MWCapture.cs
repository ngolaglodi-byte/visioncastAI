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

namespace AVCaptureCSharp
{
    public delegate void LPFN_MWCAP_VIDEO_CAPTURE_CALLBACK(IntPtr pbImage, UInt32 cbFrame, int cbImageStride, IntPtr pvParam);

    public struct CAPTURE_PARAMS
    {
        public int cx;
        public int cy;

        public UInt32 dwFourcc;
        public UInt32 dwFrameDuration;
        public LPFN_MWCAP_VIDEO_CAPTURE_CALLBACK pFuncCallback;
    }


    public class MWCapture
    {
        //video device 
        protected IntPtr m_hVideoChannel = IntPtr.Zero;
        protected IntPtr m_hD3DRenderer = IntPtr.Zero;

        //video params
        protected IntPtr m_hWnd = IntPtr.Zero;
        protected CAPTURE_PARAMS m_capParams;

        // video device index
        protected int m_nCurrentIndex = -1;
        protected Boolean m_bIsCapture = false;

        protected int m_nBoard = 0;
        protected int m_nChannelIndex = 0;

        // capture thread
        Thread m_capThread = null;
        IntPtr m_hExitEvent = IntPtr.Zero;

        //snapshot thread
        Thread m_snapThread = null;
        Boolean m_bSnapshot = false;
        string m_strFilePath;
        int m_nIndexFilter = 0;

        // count
        long m_llStartTime;
        long m_llFrameCount;
 
        public MWCapture() { 
        
        }

        //Initialize and Uninitialize LibXIStream
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


        public Boolean OpenVideoChannel(int nChannelIndex,UInt32 dwFourcc, int cx, int cy, UInt32 nFrameDuration, IntPtr hWnd, Rectangle rcPanel)
        {
            // open video device
            LibMWCapture.MW_RESULT mr;

            int iSize = Marshal.SizeOf(typeof(LibMWCapture.MWCAP_CHANNEL_INFO));
            IntPtr pChannelInfo = Marshal.AllocCoTaskMem(iSize);
            mr = LibMWCapture.MWGetChannelInfoByIndex(nChannelIndex, pChannelInfo);
            LibMWCapture.MWCAP_CHANNEL_INFO channelInfo = (LibMWCapture.MWCAP_CHANNEL_INFO)Marshal.PtrToStructure(pChannelInfo, typeof(LibMWCapture.MWCAP_CHANNEL_INFO));
            Marshal.FreeCoTaskMem(pChannelInfo);

            m_nBoard = channelInfo.byBoardIndex;
            m_nChannelIndex = channelInfo.byChannelIndex;

            m_hVideoChannel = LibMWCapture.MWOpenChannel(channelInfo.byBoardIndex, channelInfo.byChannelIndex);
            if (m_hVideoChannel == IntPtr.Zero)
                return false;

            //get video signal status
            int t_n_size=Marshal.SizeOf(typeof(LibMWCapture.MWCAP_VIDEO_SIGNAL_STATUS));
            IntPtr t_p_video_signal_status=Marshal.AllocCoTaskMem(t_n_size);
            LibMWCapture.MWGetVideoSignalStatus(m_hVideoChannel,t_p_video_signal_status);
            LibMWCapture.MWCAP_VIDEO_SIGNAL_STATUS t_video_signal_status = (LibMWCapture.MWCAP_VIDEO_SIGNAL_STATUS)Marshal.PtrToStructure(t_p_video_signal_status, typeof(LibMWCapture.MWCAP_VIDEO_SIGNAL_STATUS));
            Marshal.FreeCoTaskMem(t_p_video_signal_status);

            // create video renderer
            bool t_b_reverse=false;
            m_hD3DRenderer = LibMWMedia.MWCreateD3DRenderer(cx, cy, dwFourcc, t_b_reverse, false,hWnd);
            if (m_hD3DRenderer == IntPtr.Zero)
            {
                CloseVideoChannel();
                return false;
            }
                

            m_capParams.dwFourcc = dwFourcc;
            m_capParams.cx = cx;
            m_capParams.cy = cy;
            m_capParams.dwFrameDuration = nFrameDuration;
            m_capParams.pFuncCallback = new LPFN_MWCAP_VIDEO_CAPTURE_CALLBACK(MWCapture.VideoCaptureCallback);
           
            m_hWnd = hWnd;



            Boolean bRet = StartVideoCapature(nChannelIndex);
            if (bRet == false)
            {
                CloseVideoChannel();
                return false;
            }

            return true;
        }

        public void CloseVideoChannel()
        {
            if (m_hVideoChannel != IntPtr.Zero)
            {
                StopVideoCapture();
                LibMWCapture.MWCloseChannel(m_hVideoChannel);
                m_hVideoChannel = IntPtr.Zero;
            }
            if (m_hD3DRenderer != IntPtr.Zero)
            {
                LibMWMedia.MWDestroyD3DRenderer(m_hD3DRenderer);
                m_hD3DRenderer = IntPtr.Zero;
            }
        }

        public Boolean StartVideoCapature(int nIndex)
        {
            if (nIndex == m_nCurrentIndex && m_bIsCapture == true)
                return true;

            if (nIndex != m_nCurrentIndex && m_bIsCapture == true)
                StopVideoCapture();

            if (m_hVideoChannel == IntPtr.Zero)
                return false;


            m_hExitEvent = Libkernel32.CreateEvent(IntPtr.Zero, 0, 0, IntPtr.Zero);
            if (m_hExitEvent == IntPtr.Zero)
                return false;

            m_bIsCapture = true;
            m_llFrameCount = 0;
            m_capThread = new Thread(new ParameterizedThreadStart(VideoCapThread));
            m_capThread.Start(m_capParams);

            m_nCurrentIndex = nIndex;

            return true;
        }


        public void StopVideoCapture()
        {
            if (m_bIsCapture == false && m_nCurrentIndex == -1)
                return;

            m_bIsCapture = false;
            Libkernel32.SetEvent(m_hExitEvent);
            m_capThread.Join();

            m_nCurrentIndex = -1;
        }

        public double GetAveFps()
        {
            long llCurTime = 0;
            LibMWCapture.MWGetDeviceTime(m_hVideoChannel, ref llCurTime);

            return 1.0 *10000000* m_llFrameCount /(llCurTime - m_llStartTime);
        }

        public void SnapshotThread(object oParams)
        {
            do
            {
                CAPTURE_PARAMS cp = (CAPTURE_PARAMS)oParams;
                Bitmap bitmap = new Bitmap(cp.cx, cp.cy, PixelFormat.Format32bppArgb);
                int cx = cp.cx;
                int cy = cp.cy;

                Rectangle rect = new Rectangle(0, 0, cx, cy);
                BitmapData bitmapData;
                bitmapData = bitmap.LockBits(rect, ImageLockMode.ReadWrite, PixelFormat.Format32bppArgb);

                IntPtr pbImage = IntPtr.Zero;
                int cbOSDImageStride = 0;
                Byte bBottomUp = 0;

                if (bitmapData.Stride < 0)
                {
                    pbImage = (IntPtr)((Int64)(bitmapData.Scan0) + bitmapData.Stride * (cy - 1));
                    cbOSDImageStride = -bitmapData.Stride;
                    bBottomUp = 1;
                }
                else
                {
                    pbImage = (IntPtr)bitmapData.Scan0;
                    cbOSDImageStride = bitmapData.Stride;
                    bBottomUp = 0;
                }

                // event
                LibMWCapture.MW_RESULT mr;
                IntPtr hCaptureEvent = IntPtr.Zero;

                IntPtr hVideoChannel = LibMWCapture.MWOpenChannel(m_nBoard, m_nChannelIndex);
                if (hVideoChannel == IntPtr.Zero)
                    break;

                hCaptureEvent = Libkernel32.CreateEvent(IntPtr.Zero, 0, 0, IntPtr.Zero);
                if (hCaptureEvent == IntPtr.Zero)
                    break;

                mr = LibMWCapture.MWStartVideoCapture(hVideoChannel, hCaptureEvent);
                if (mr != LibMWCapture.MW_RESULT.MW_SUCCEEDED)
                    break;

                UInt32 dwFourcc = MWCap_FOURCC.MWCAP_FOURCC_BGRA;
                UInt32 dwFrameDuration = cp.dwFrameDuration;
                UInt32 dwMinStride = MWCap_FOURCC.FOURCC_CalcMinStride(dwFourcc, cx, 4);
                UInt32 dwImageSize = MWCap_FOURCC.FOURCC_CalcImageSize(dwFourcc, cx, cy, dwMinStride);

                IntPtr pCapStatus = IntPtr.Zero;
                int nSize = Marshal.SizeOf(typeof(LibMWCapture.MWCAP_VIDEO_CAPTURE_STATUS));
                pCapStatus = Marshal.AllocCoTaskMem(nSize);

                mr = LibMWCapture.MWCaptureVideoFrameToVirtualAddress(hVideoChannel, -1, pbImage, dwImageSize, dwMinStride, bBottomUp, 0, dwFourcc, cx, cy);
                if (mr != LibMWCapture.MW_RESULT.MW_SUCCEEDED)
                    break;

                Int64 dwRet = Libkernel32.WaitForSingleObject(hCaptureEvent, Libkernel32.INFINITE);
                mr = LibMWCapture.MWGetVideoCaptureStatus(hVideoChannel, pCapStatus);
                if (mr != LibMWCapture.MW_RESULT.MW_SUCCEEDED)
                    break;

                ImageCodecInfo myImageCodecInfo;
                if(m_nIndexFilter == 1)
                    myImageCodecInfo = GetEncoderInfo("image/bmp");
                else
                    myImageCodecInfo = GetEncoderInfo("image/png");

                bitmap.Save(m_strFilePath, myImageCodecInfo, null);

                bitmap.UnlockBits(bitmapData);

                if (pCapStatus != IntPtr.Zero)
                    Marshal.FreeCoTaskMem(pCapStatus);

                LibMWCapture.MWStopVideoCapture(hVideoChannel);

                if (hCaptureEvent != IntPtr.Zero)
                {
                    Libkernel32.CloseHandle(hCaptureEvent);
                    hCaptureEvent = IntPtr.Zero;
                }

                if (hVideoChannel != IntPtr.Zero)
                {
                    LibMWCapture.MWCloseChannel(hVideoChannel);
                    hVideoChannel = IntPtr.Zero;
                }

            } while (false);

            return;
        }

        public void VideoCapThread(object oParams)
        {
            LibMWCapture.MW_RESULT mr;

            CAPTURE_PARAMS cp = (CAPTURE_PARAMS)oParams;

            // event
            UInt64 hTimer = 0;
            IntPtr hTimerEvent = IntPtr.Zero;
            IntPtr hCaptureEvent = IntPtr.Zero;


            hTimerEvent = Libkernel32.CreateEvent(IntPtr.Zero, 0, 0, IntPtr.Zero);
            if (hTimerEvent == IntPtr.Zero)
                return;

            hTimer = LibMWCapture.MWRegisterTimer(m_hVideoChannel, hTimerEvent);
            if (hTimer == 0)
                return;

            hCaptureEvent = Libkernel32.CreateEvent(IntPtr.Zero, 0, 0, IntPtr.Zero);
            if (hCaptureEvent == IntPtr.Zero)
                return;

            mr = LibMWCapture.MWStartVideoCapture(m_hVideoChannel, hCaptureEvent);
            if (mr != LibMWCapture.MW_RESULT.MW_SUCCEEDED)
                return;


            m_llStartTime = 0;
            mr = LibMWCapture.MWGetDeviceTime(m_hVideoChannel, ref m_llStartTime);
            if (mr != LibMWCapture.MW_RESULT.MW_SUCCEEDED)
                return;

            int cx = cp.cx;
            int cy = cp.cy;
            UInt32 dwFourcc = cp.dwFourcc;
            UInt32 dwFrameDuration = cp.dwFrameDuration;
            UInt32 dwMinStride = MWCap_FOURCC.FOURCC_CalcMinStride(dwFourcc, cx, 4);
            UInt32 dwImageSize = MWCap_FOURCC.FOURCC_CalcImageSize(dwFourcc, cx, cy, dwMinStride);

            IntPtr pbImage = Marshal.AllocCoTaskMem((int)dwImageSize);
            if (pbImage == IntPtr.Zero)
                return;

            m_llFrameCount = 0;

            int nSize = 0;
            IntPtr pSignalStatus = IntPtr.Zero;
            nSize = Marshal.SizeOf(typeof(LibMWCapture.MWCAP_VIDEO_SIGNAL_STATUS));
            pSignalStatus = Marshal.AllocCoTaskMem(nSize);
            //LibMWCapture.MWCAP_VIDEO_SIGNAL_STATUS signalStatus;

            IntPtr pCapStatus = IntPtr.Zero;
            nSize = Marshal.SizeOf(typeof(LibMWCapture.MWCAP_VIDEO_CAPTURE_STATUS));
            pCapStatus = Marshal.AllocCoTaskMem(nSize);
            LibMWCapture.MWCAP_VIDEO_CAPTURE_STATUS capStatus;

            while (true)
            {
                //LibMWCapture.MWGetVideoSignalStatus(m_hVideoChannel, pSignalStatus);
                //signalStatus = (LibMWCapture.MWCAP_VIDEO_SIGNAL_STATUS)Marshal.PtrToStructure(pSignalStatus, typeof(LibMWCapture.MWCAP_VIDEO_SIGNAL_STATUS));
                
                
                
                ////if (signalStatus.state != LibMWCapture.MWCAP_VIDEO_SIGNAL_STATE.MWCAP_VIDEO_SIGNAL_LOCKED)
                ////{
                ////    Thread.Sleep(500);
                ////    continue;
                ////}

                try
                {
                    mr = LibMWCapture.MWScheduleTimer(m_hVideoChannel, hTimer, m_llStartTime + (m_llFrameCount + 1) * dwFrameDuration);
                    if (mr != LibMWCapture.MW_RESULT.MW_SUCCEEDED)
                        break;

                    IntPtr[] hEvents = { m_hExitEvent, hTimerEvent };
                    UInt32 dwRet = Libkernel32.WaitForMultipleObjects(2, hEvents, 0, 1000);
                    if (dwRet != 1)
                        break;
                    //UInt32 dwRet = Libkernel32.WaitForSingleObject(hTimerEvent, 1000);
                    //if (dwRet != 0)
                    //    break;

                   // mr = LibMWCapture.MWCaptureVideoFrameToVirtualAddress(m_hVideoChannel, -1, pbImageSnap, dwImageSize, dwMinStride, 0, 0, dwFourcc, cx, cy);

                    mr = LibMWCapture.MWCaptureVideoFrameToVirtualAddress(m_hVideoChannel, -1, pbImage, dwImageSize, dwMinStride, 0, 0, dwFourcc, cx, cy);
                 
                    
                    if (mr != LibMWCapture.MW_RESULT.MW_SUCCEEDED)
                        break;
                    
                    dwRet = Libkernel32.WaitForSingleObject(hCaptureEvent, Libkernel32.INFINITE);
                    mr = LibMWCapture.MWGetVideoCaptureStatus(m_hVideoChannel, pCapStatus);
                    if (mr != LibMWCapture.MW_RESULT.MW_SUCCEEDED)
                        break;

                    m_llFrameCount++;

                    if (cp.pFuncCallback != null)
                    {
                        cp.pFuncCallback((IntPtr)pbImage, dwImageSize, (int)dwMinStride, m_hD3DRenderer);
                    }

                    if (m_bSnapshot == true)
                    {
                        CreateSnapshot();
                        m_bSnapshot = false;
                    }
                }
                catch (Exception ex) { }           
            
            }

            if (pSignalStatus != IntPtr.Zero)
                Marshal.FreeCoTaskMem(pSignalStatus);

            if (pCapStatus != IntPtr.Zero)
                Marshal.FreeCoTaskMem(pCapStatus);

            if (pbImage != IntPtr.Zero)
            {
                Marshal.FreeCoTaskMem((IntPtr)pbImage);
                pbImage = IntPtr.Zero;
            }

            if (hTimer != 0)
            {
                LibMWCapture.MWUnregisterTimer(m_hVideoChannel, hTimer);
                hTimer = 0;
            }

            if (hTimerEvent != IntPtr.Zero)
            {
                Libkernel32.CloseHandle(hTimerEvent);
                hTimerEvent = IntPtr.Zero;
            }

            LibMWCapture.MWStopVideoCapture(m_hVideoChannel);

            if (hCaptureEvent != IntPtr.Zero)
            {
                Libkernel32.CloseHandle(hCaptureEvent);
                hCaptureEvent = IntPtr.Zero;
            }          
        }

        ImageCodecInfo GetEncoderInfo(String mimeType)
        {
            int j;
            ImageCodecInfo[] encoders;
            encoders = ImageCodecInfo.GetImageEncoders();
            for (j = 0; j < encoders.Length; ++j)
            {
                if (encoders[j].MimeType == mimeType)
                    return encoders[j];
            }
            return null;
        }


        public void CreateSnapshot()
        {
            m_snapThread = new Thread(new ParameterizedThreadStart(SnapshotThread));
            m_snapThread.Start(m_capParams);
           // m_snapThread.Join();
        }

        public static void VideoCaptureCallback(IntPtr pbyImage, UInt32 cbFrame, int cbImageStride, IntPtr pvParam)
        {
            if (pvParam != IntPtr.Zero)
            {
                LibMWMedia.MWD3DRendererPushFrame(pvParam, pbyImage, cbImageStride);
            }
        }

        public void SetSnapshotflag(string strName, int nIndex)
        {
            m_nIndexFilter = nIndex;
            m_strFilePath = strName;
            m_bSnapshot = true;
        }
    }
}
