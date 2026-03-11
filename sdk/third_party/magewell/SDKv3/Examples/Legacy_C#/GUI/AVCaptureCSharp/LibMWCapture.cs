
using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Linq;
using System.Text;


namespace AVCaptureCSharp
{
    public partial class LibMWCapture
    {

        public enum MW_RESULT
        {
            MW_SUCCEEDED = 0x00,
            MW_FAILED,
            MW_INVALID_PARAMS,
        }


        public enum MWCAP_PRODUCT_ID
        {
            MWCAP_PRODUCT_ID_PRO_CAPTURE_AIO = 0x00000102,
            MWCAP_PRODUCT_ID_PRO_CAPTURE_DVI = 0x00000103,
            MWCAP_PRODUCT_ID_PRO_CAPTURE_HDMI = 0x00000104,
            MWCAP_PRODUCT_ID_PRO_CAPTURE_SDI = 0x00000105,
            MWCAP_PRODUCT_ID_PRO_CAPTURE_DUAL_SDI = 0x00000106,
            MWCAP_PRODUCT_ID_PRO_CAPTURE_DUAL_DVI = 0x00000107,
            MWCAP_PRODUCT_ID_PRO_CAPTURE_DUAL_HDMI = 0x00000108,
            MWCAP_PRODUCT_ID_PRO_CAPTURE_QUAD_SDI = 0x00000109,
            MWCAP_PRODUCT_ID_PRO_CAPTURE_QUAD_HDMI = 0x00000110,
            MWCAP_PRODUCT_ID_PRO_CAPTURE_MINI_HDMI = 0x00000111,
            MWCAP_PRODUCT_ID_PRO_CAPTURE_HDMI_4K = 0x00000112,
            MWCAP_PRODUCT_ID_PRO_CAPTURE_MINI_SDI = 0x00000113
        }


        public enum MWCAP_VIDEO_INPUT_TYPE
        {
            MWCAP_VIDEO_INPUT_TYPE_NONE = 0x00,
            MWCAP_VIDEO_INPUT_TYPE_HDMI = 0x01,
            MWCAP_VIDEO_INPUT_TYPE_VGA = 0x02,
            MWCAP_VIDEO_INPUT_TYPE_SDI = 0x04,
            MWCAP_VIDEO_INPUT_TYPE_COMPONENT = 0x08,
            MWCAP_VIDEO_INPUT_TYPE_CVBS = 0x10,
            MWCAP_VIDEO_INPUT_TYPE_YC = 0x20

        }

        public enum MWCAP_AUDIO_INPUT_TYPE
        {
            MWCAP_AUDIO_INPUT_TYPE_NONE = 0x00,
            MWCAP_AUDIO_INPUT_TYPE_HDMI = 0x01,
            MWCAP_AUDIO_INPUT_TYPE_SDI = 0x02,
            MWCAP_AUDIO_INPUT_TYPE_LINE_IN = 0x04,
            MWCAP_AUDIO_INPUT_TYPE_MIC_IN = 0x08
        };

        public enum MWCAP_VIDEO_FRAME_TYPE
        {
            MWCAP_VIDEO_FRAME_2D = 0x00,
            MWCAP_VIDEO_FRAME_3D_TOP_AND_BOTTOM_FULL = 0x01,
            MWCAP_VIDEO_FRAME_3D_TOP_AND_BOTTOM_HALF = 0x02,
            MWCAP_VIDEO_FRAME_3D_SIDE_BY_SIDE_FULL = 0x03,
            MWCAP_VIDEO_FRAME_3D_SIDE_BY_SIDE_HALF = 0x04
        }

        public enum MWCAP_VIDEO_COLOR_FORMAT
        {
            MWCAP_VIDEO_COLOR_FORMAT_UNKNOWN = 0x00,
            MWCAP_VIDEO_COLOR_FORMAT_RGB = 0x01,
            MWCAP_VIDEO_COLOR_FORMAT_YUV601 = 0x02,
            MWCAP_VIDEO_COLOR_FORMAT_YUV709 = 0x03,
            MWCAP_VIDEO_COLOR_FORMAT_YUV2020 = 0x04,
            MWCAP_VIDEO_COLOR_FORMAT_YUV2020C = 0x05				// Constant luminance, not supported yet.
        }

        public enum MWCAP_VIDEO_QUANTIZATION_RANGE
        {
            MWCAP_VIDEO_QUANTIZATION_UNKNOWN = 0x00,
            MWCAP_VIDEO_QUANTIZATION_FULL = 0x01, 			// Black level: 0, White level: 255/1023/4095/65535
            MWCAP_VIDEO_QUANTIZATION_LIMITED = 0x02				// Black level: 16/64/256/4096, White level: 235(240)/940(960)/3760(3840)/60160(61440)
        }


        public enum MWCAP_VIDEO_SATURATION_RANGE
        {
            MWCAP_VIDEO_SATURATION_UNKNOWN = 0x00,
            MWCAP_VIDEO_SATURATION_FULL = 0x01, 			// Min: 0, Max: 255/1023/4095/65535
            MWCAP_VIDEO_SATURATION_LIMITED = 0x02, 			// Min: 16/64/256/4096, Max: 235(240)/940(960)/3760(3840)/60160(61440)
            MWCAP_VIDEO_SATURATION_EXTENDED_GAMUT = 0x03  			// Min: 1/4/16/256, Max: 254/1019/4079/65279
        }


        ///////////////////////////////////////////////////////////////////////////////
        //  Initialized  and UnInitialized LibXIStream library
#if DEBUG
        [DllImport("LibMWCaptured.dll", CallingConvention = CallingConvention.Cdecl)]
#else
        [DllImport("LibMWCapture.dll", CallingConvention = CallingConvention.Cdecl)]
#endif
        public static extern int MWCaptureInitInstance();



#if DEBUG
        [DllImport("LibMWCaptured.dll", CallingConvention = CallingConvention.Cdecl)]
#else
        [DllImport("LibMWCapture.dll", CallingConvention = CallingConvention.Cdecl)]
#endif
        public static extern void MWCaptureExitInstance();


        ///////////////////////////////////////////////////////////////////////////////
        // Device query
#if DEBUG
        [DllImport("LibMWCaptured.dll", CallingConvention = CallingConvention.Cdecl)]
#else
        [DllImport("LibMWCapture.dll", CallingConvention = CallingConvention.Cdecl)]
#endif
        public static extern MW_RESULT MWRefreshDevice();



#if DEBUG
        [DllImport("LibMWCaptured.dll", CallingConvention = CallingConvention.Cdecl)]
#else
        [DllImport("LibMWCapture.dll", CallingConvention = CallingConvention.Cdecl)]
#endif
        public static extern int MWGetChannelCount();



        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi, Pack = 1)]
        public struct MWCAP_CHANNEL_INFO
        {
            public UInt16 wFamilyID;
            public UInt16 wProductID;
            public SByte chHardwareVersion;
            public Byte byFirmwareID;
            public UInt32 dwFirmwareVersion;
            public UInt32 dwDriverVersion;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
            public string szFamilyName;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
            public string szProductName;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
            public string szFirmwareName;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 16)]
            public string szBoardSerialNo;
            public Byte byBoardIndex;
            public Byte byChannelIndex;
        }

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
        public struct MWCAP_PRO_CAPTURE_INFO
        {
            public Byte byPCIBusID;
            public Byte byPCIDevID;
            public Byte byLinkType;
            public Byte byLinkWidth;
            public Byte byBoardIndex;
            public UInt16 wMaxPayloadSize;
            public UInt16 wMaxReadRequestSize;
            public UInt32 cbTotalMemorySize;
            public UInt32 cbFreeMemorySize;
        }

#if DEBUG
        [DllImport("LibMWCaptured.dll", CallingConvention = CallingConvention.Cdecl)]
#else
        [DllImport("LibMWCapture.dll", CallingConvention = CallingConvention.Cdecl)]
#endif
        public static extern MW_RESULT MWGetChannelInfoByIndex(int nIndex, IntPtr pChannelInfo);


#if DEBUG
        [DllImport("LibMWCaptured.dll", CallingConvention = CallingConvention.Cdecl)]
#else
        [DllImport("LibMWCapture.dll", CallingConvention = CallingConvention.Cdecl)]
#endif
        public static extern MW_RESULT MWGetFamilyInfoByIndex(int nIndex, IntPtr pFamilyInfo, UInt32 dwSize);


#if DEBUG
        [DllImport("LibMWCaptured.dll", CallingConvention = CallingConvention.Cdecl)]
#else
        [DllImport("LibMWCapture.dll", CallingConvention = CallingConvention.Cdecl)]
#endif
        public static extern MW_RESULT MWGetChannelInfo(IntPtr hChannel, IntPtr pChannelInfo);


//////////////////////////////////////////////////////////////////////////////
// Channel Open and Close

#if DEBUG
        [DllImport("LibMWCaptured.dll", CallingConvention = CallingConvention.Cdecl)]
#else
        [DllImport("LibMWCapture.dll", CallingConvention = CallingConvention.Cdecl)]
#endif
        public static extern IntPtr MWOpenChannel(int nBoardValue, int nChannelIndex); // string szDShowID


#if DEBUG
        [DllImport("LibMWCaptured.dll", CallingConvention = CallingConvention.Cdecl)]
#else
        [DllImport("LibMWCapture.dll", CallingConvention = CallingConvention.Cdecl)]
#endif
        public static extern void MWCloseChannel(IntPtr hChannel);



        // Video signal status
        public enum MWCAP_VIDEO_SIGNAL_STATE
        {
            MWCAP_VIDEO_SIGNAL_NONE,						// No signal detectd
            MWCAP_VIDEO_SIGNAL_UNSUPPORTED,					// Video signal status not valid
            MWCAP_VIDEO_SIGNAL_LOCKING,						// Video signal status valid but not locked yet
            MWCAP_VIDEO_SIGNAL_LOCKED						// Every thing OK
        }

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi,Pack = 1)]
        public struct MWCAP_VIDEO_SIGNAL_STATUS
        {
            public MWCAP_VIDEO_SIGNAL_STATE state;
            public int x;
            public int y;
            public int cx;
            public int cy;
            public int cxTotal;
            public int cyTotal;
            public Byte bInterlaced;
            public UInt32 dwFrameDuration;
            public int nAspectX;
            public int nAspectY;
            public Byte bSegmentedFrame;
            public MWCAP_VIDEO_FRAME_TYPE frameType;
            public MWCAP_VIDEO_COLOR_FORMAT colorFormat;
            public MWCAP_VIDEO_QUANTIZATION_RANGE quantRange;
            public MWCAP_VIDEO_SATURATION_RANGE satRange;
        }

#if DEBUG
        [DllImport("LibMWCaptured.dll", CallingConvention = CallingConvention.Cdecl)]
#else
        [DllImport("LibMWCapture.dll", CallingConvention = CallingConvention.Cdecl)]
#endif
        public static extern MW_RESULT MWGetVideoSignalStatus(IntPtr hChannel, IntPtr pSignalStatus);

//////////////////////////////////////////////////////////////////////////////////
// video capture
#if DEBUG
        [DllImport("LibMWCaptured.dll", CallingConvention = CallingConvention.Cdecl)]
#else
        [DllImport("LibMWCapture.dll", CallingConvention = CallingConvention.Cdecl)]
#endif
        public static extern LibMWCapture.MW_RESULT MWStartVideoCapture(IntPtr hChannel, IntPtr hEvent);


#if DEBUG
        [DllImport("LibMWCaptured.dll", CallingConvention = CallingConvention.Cdecl)]
#else
        [DllImport("LibMWCapture.dll", CallingConvention = CallingConvention.Cdecl)]
#endif
        public static extern LibMWCapture.MW_RESULT MWStopVideoCapture(IntPtr hChannel);



        static public int MWCAP_VIDEO_FRAME_ID_NEWEST_BUFFERED = -1;

#if DEBUG
        [DllImport("LibMWCaptured.dll", CallingConvention = CallingConvention.Cdecl)]
#else
        [DllImport("LibMWCapture.dll", CallingConvention = CallingConvention.Cdecl)]
#endif
        public static extern LibMWCapture.MW_RESULT MWCaptureVideoFrameToVirtualAddress(
                    IntPtr hChannel,
                    int iFrame,
                    IntPtr pbFrame,
                    UInt32 cbFrame,
                    UInt32 cbStride,
                    Byte bBottomUp,
                    Int64 pvContext,
                    UInt32 dwFOURCC,
                    int cx,
                    int cy
                    );


        [StructLayout(LayoutKind.Explicit, CharSet=CharSet.Ansi,Pack=1)]
        public struct MWCAP_VIDEO_CAPTURE_STATUS 
        {
            [FieldOffset(0)]
	        public Int64								pvContext;
            [FieldOffset(8)]
    	    public Byte	                                bPhysicalAddress;

            [FieldOffset(9)]
            public IntPtr                               pvFrame;
            [FieldOffset(9)]
            public Int64 liPhysicalAddress;

            [FieldOffset(17)]
	        public  int									iFrame;
            [FieldOffset(21)]
	        public Byte									bFrameCompleted;

            [FieldOffset(22)]
	        public UInt16								cyCompleted;

            [FieldOffset(24)]
	        public UInt16								cyCompletedPrev;
        }

#if DEBUG
        [DllImport("LibMWCaptured.dll", CallingConvention = CallingConvention.Cdecl)]
#else
        [DllImport("LibMWCapture.dll", CallingConvention = CallingConvention.Cdecl)]
#endif
        public static extern MW_RESULT MWGetVideoCaptureStatus(IntPtr 	hChannel, IntPtr pStatus);


///////////////////////////////////////////////////////////////////////////
// timer event
#if DEBUG
        [DllImport("LibMWCaptured.dll", CallingConvention = CallingConvention.Cdecl)]
#else
        [DllImport("LibMWCapture.dll", CallingConvention = CallingConvention.Cdecl)]
#endif
        public static extern UInt64 MWRegisterTimer(IntPtr	hChannel, IntPtr  hEvent);


#if DEBUG
        [DllImport("LibMWCaptured.dll", CallingConvention = CallingConvention.Cdecl)]
#else
        [DllImport("LibMWCapture.dll", CallingConvention = CallingConvention.Cdecl)]
#endif
        public static extern MW_RESULT MWUnregisterTimer(IntPtr hChannel, UInt64 hTimer);



#if DEBUG
        [DllImport("LibMWCaptured.dll", CallingConvention = CallingConvention.Cdecl)]
#else
        [DllImport("LibMWCapture.dll", CallingConvention = CallingConvention.Cdecl)]
#endif
        public static extern MW_RESULT MWScheduleTimer(IntPtr hChannel, UInt64 hTimer, Int64 llExpireTime);



///////////////////////////////////////////////////////////////////////////////
// Notify event
#if DEBUG
        [DllImport("LibMWCaptured.dll", CallingConvention = CallingConvention.Cdecl)]
#else
        [DllImport("LibMWCapture.dll", CallingConvention = CallingConvention.Cdecl)]
#endif
        public static extern IntPtr MWRegisterNotify(IntPtr hChannel, IntPtr hEvent, UInt32	dwEnableBits);


#if DEBUG
        [DllImport("LibMWCaptured.dll", CallingConvention = CallingConvention.Cdecl)]
#else
        [DllImport("LibMWCapture.dll", CallingConvention = CallingConvention.Cdecl)]
#endif
        public static extern MW_RESULT MWUnregisterNotify(IntPtr hChannel, IntPtr hNotify);


#if DEBUG
        [DllImport("LibMWCaptured.dll", CallingConvention = CallingConvention.Cdecl)]
#else
        [DllImport("LibMWCapture.dll", CallingConvention = CallingConvention.Cdecl)]
#endif
        public static extern MW_RESULT MWGetNotifyStatus(IntPtr hChannel, IntPtr hNotify, ref UInt64 ullStatus);


////////////////////////////////////////////////////////////////////////////////////
// Device clock
#if DEBUG
        [DllImport("LibMWCaptured.dll", CallingConvention = CallingConvention.Cdecl)]
#else
        [DllImport("LibMWCapture.dll", CallingConvention = CallingConvention.Cdecl)]
#endif
        public static extern MW_RESULT MWGetDeviceTime(IntPtr hChannel, ref long pllTime);

    }
}
















