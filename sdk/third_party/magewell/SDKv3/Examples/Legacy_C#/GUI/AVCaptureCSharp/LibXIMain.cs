
using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Linq;
using System.Text;


namespace XICaptureCSharp
{
    public partial class LibXIMain
    {
        public enum XI_ADAPTER_MODEL
        {
            XIM_ADAPTER_UNKNOWN,
            XIM_PRO_CAPTURE_AIO = 0x00000102,
            XIM_PRO_CAPTURE_DVI = 0x00000103,
            XIM_PRO_CAPTURE_HDMI = 0x00000104,
            XIM_PRO_CAPTURE_SDI = 0x00000105,
            XIM_PRO_CAPTURE_DUAL_SDI = 0x00000106,
            XIM_PRO_CAPTURE_DUAL_DVI = 0x00000107,
            XIM_PRO_CAPTURE_DUAL_HDMI = 0x00000108,
            XIM_PRO_CAPTURE_QUAD_SDI = 0x00000109,
            XIM_PRO_CAPTURE_QUAD_HDMI = 0x00000110
        }

        public enum XIM_FAMILY_ID
        {
            XIM_FAMILY_ID_PRO_CAPTURE = 0x00,
            XIM_FAMILY_ID_VALUE_CAPTURE = 0x01,
            XIM_FAMILY_ID_USB_CAPTURE = 0x02
        }


///////////////////////////////////////////////////////////////////////////////
//  Initialized  and UnInitialized LibXIStream library
#if DEBUG
        [DllImport("LibXIMaind.dll", CallingConvention = CallingConvention.Cdecl)]
#else
        [DllImport("LibXIMain.dll", CallingConvention = CallingConvention.Cdecl)]
#endif
        public static extern int XIM_Initialize();



#if DEBUG
        [DllImport("LibXIMaind.dll", CallingConvention = CallingConvention.Cdecl)]
#else
        [DllImport("LibXIMain.dll", CallingConvention = CallingConvention.Cdecl)]
#endif
        public static extern void XIM_Uninitialize();


///////////////////////////////////////////////////////////////////////////////
// Device query
#if DEBUG
        [DllImport("LibXIMaind.dll", CallingConvention = CallingConvention.Cdecl)]
#else
        [DllImport("LibXIMain.dll", CallingConvention = CallingConvention.Cdecl)]
#endif
        public static extern int XIM_GetDeviceCount();


        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
        public struct XIM_VIDEO_CHANNEL_INFO
        {
            public Byte byChannelIndex; //Channel index on board (NOT global !!!)
            public Byte byPairedChannelIndex; //Paired channel index on board, 0xFF for none
            public UInt32 dwSupportedInputTypes; //XIM_VIDEO_INPUT_TYPE
        }

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
        public struct XIM_PRO_CAPTURE_INFO
        {
            public Byte byPCIBusID;
            public Byte byPCIDevID;
            public Byte byLinkType;
            public Byte byLinkWidth;
            public Byte byBoardIndex;
            public UInt16 wMaxPlayLoadSize;
            public UInt16 wMaxReadRequestSize;
            public UInt32 cwMemorySize;
        }


        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public struct XIM_DEVICE_INFO
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
            public string szBordSerialName;

            public Byte byBoardIndex;
            public UInt32 dwVideoCount;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 8)]
            public XIM_VIDEO_CHANNEL_INFO[] videoInfo;
            public XIM_PRO_CAPTURE_INFO proCaptureInfo;
        }

#if DEBUG
        [DllImport("LibXIMaind.dll", CallingConvention = CallingConvention.Cdecl)]
#else
        [DllImport("LibXIMain.dll", CallingConvention = CallingConvention.Cdecl)]
#endif
        public static extern int XIM_GetDeviceInfo(int iDevice, IntPtr pVideoDeviceInfo);


//////////////////////////////////////////////////////////////////////////////
// Device Open and Close

#if DEBUG
        [DllImport("LibXIMaind.dll", CallingConvention = CallingConvention.Cdecl)]
#else
        [DllImport("LibXIMain.dll", CallingConvention = CallingConvention.Cdecl)]
#endif
        public static extern IntPtr XIM_OpenVideoChannel(int mDeviceIndex, int nChannelIndex); // string szDShowID


#if DEBUG
        [DllImport("LibXIMaind.dll", CallingConvention = CallingConvention.Cdecl)]
#else
        [DllImport("LibXIMain.dll", CallingConvention = CallingConvention.Cdecl)]
#endif
        public static extern void XIM_CloseVideoChannel(IntPtr hVideoChannel);


//////////////////////////////////////////////////////////////////////////////////
// video capture

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate void LPFN_XIM_VIDEO_CAPTURE_CALLBACK(IntPtr pbImage, UInt32 cbFrame, int cbImageStride, IntPtr pvParam, UInt64 u64TimesStamp);

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
        public struct XIM_VIDEO_CAPTURE_PARAMS
        {
            public UInt32 dwFourcc;
            public int cx;
            public int cy;
            public UInt32 dwDuration;
            public int bBottomUp;
            public LPFN_XIM_VIDEO_CAPTURE_CALLBACK pFuncCallback;
            public IntPtr pvParam;
        };


#if DEBUG
        [DllImport("LibXIMaind.dll", CallingConvention = CallingConvention.Cdecl)]
#else
        [DllImport("LibXIMain.dll", CallingConvention = CallingConvention.Cdecl)]
#endif
        public static extern int XIM_StartVideoCaptureByCallback(IntPtr hVideoChannel, ref XIM_VIDEO_CAPTURE_PARAMS pParams);


#if DEBUG
        [DllImport("LibXIMaind.dll", CallingConvention = CallingConvention.Cdecl)]
#else
        [DllImport("LibXIMain.dll", CallingConvention = CallingConvention.Cdecl)]
#endif
        public static extern void XIM_StopVideoCapture(IntPtr hVideoChannel);


//////////////////////////////////////////////////////////////////////////////////////////
// video renderer

#if DEBUG
        [DllImport("LibXIMaind.dll", CallingConvention = CallingConvention.Cdecl)]
#else
        [DllImport("LibXIMain.dll", CallingConvention = CallingConvention.Cdecl)]
#endif
        public static extern int XIM_GetMonitorGUIDFromWindow(IntPtr hWnd, ref Guid pGuidMonitor);


#if DEBUG
        [DllImport("LibXIMaind.dll", CallingConvention = CallingConvention.Cdecl)]
#else
        [DllImport("LibXIMain.dll", CallingConvention = CallingConvention.Cdecl)]
#endif
        public static extern IntPtr XIM_CreateDDrawRenderer(ref Guid pGUID, IntPtr hWnd,
            UInt32 dwFourcc, int nWidth, int nHeight, int bUseOverlay, int bWaitForVerticalBlank);


#if DEBUG
        [DllImport("LibXIMaind.dll", CallingConvention = CallingConvention.Cdecl)]
#else
        [DllImport("LibXIMain.dll", CallingConvention = CallingConvention.Cdecl)]
#endif
        public static extern void XIM_DestroyDDrawRenderer(IntPtr hDDrawRenderer);


#if DEBUG
        [DllImport("LibXIMaind.dll", CallingConvention = CallingConvention.Cdecl)]
#else
        [DllImport("LibXIMain.dll", CallingConvention = CallingConvention.Cdecl)]
#endif
        public static extern int XIM_ResetDDrawRenderer(IntPtr hDDrawRenderer, ref Guid pGUID, IntPtr hWnd,
            UInt32 dwFourcc, int nWidth, int nHeight, int bUseOverlay, int bWaitForVerticalBlank);


        public struct RECT
        {
            public int nLeft;
            public int nTop;
            public int nRight;
            public int nBottom;
        }


#if DEBUG
        [DllImport("LibXIMaind.dll", CallingConvention = CallingConvention.Cdecl)]
#else
        [DllImport("LibXIMain.dll", CallingConvention = CallingConvention.Cdecl)]
#endif
        public static extern int XIM_SetDDrawRendererPosition(IntPtr hDDrawRenderer, IntPtr lprcSource, ref RECT lprcDest);



#if DEBUG
        [DllImport("LibXIMaind.dll", CallingConvention = CallingConvention.Cdecl)]
#else
        [DllImport("LibXIMain.dll", CallingConvention = CallingConvention.Cdecl)]
#endif
        public static extern int XIM_DrawRendererDrawImage(IntPtr hDDrawRenderer, IntPtr pbImage, int cbStride, int bSrcTopDown);

    }
}



