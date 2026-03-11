Imports System
Imports System.Runtime.InteropServices
Imports System.Collections.Generic
Imports System.Linq
Imports System.Text
Public Class LibMWCapture
    Public Enum MW_RESULT
        MW_SUCCEEDED = &H0&
        MW_FAILED = &H1&
        MW_INVAILD_PARAMS = &H2&
    End Enum

    Public Enum MWCAP_PRODUCT_ID
        MWCAP_PRODUCT_ID_PRO_CAPTURE_AIO = &H102&
        MWCAP_PRODUCT_ID_PRO_CAPTURE_DVI = &H103&
        MWCAP_PRODUCT_ID_PRO_CAPTURE_HDMI = &H104&
        MWCAP_PRODUCT_ID_PRO_CAPTURE_SDI = &H105&
        MWCAP_PRODUCT_ID_PRO_CAPTURE_DUAL_SDI = &H106&
        MWCAP_PRODUCT_ID_PRO_CAPTURE_DUAL_DVI = &H107&
        MWCAP_PRODUCT_ID_PRO_CAPTURE_DUAL_HDMI = &H108&
        MWCAP_PRODUCT_ID_PRO_CAPTURE_QUAD_SDI = &H109&
        MWCAP_PRODUCT_ID_PRO_CAPTURE_QUAD_HDMI = &H110&
        MWCAP_PRODUCT_ID_PRO_CAPTURE_MINI_HDMI = &H111&
        MWCAP_PRODUCT_ID_PRO_CAPTURE_HDMI_4K = &H112&
        MWCAP_PRODUCT_ID_PRO_CAPTURE_MINI_SDI = &H113&
        MWCAP_PRODUCT_ID_PRO_CAPTURE_AIO_4K_PLUS = &H114&
        MWCAP_PRODUCT_ID_PRO_CAPTURE_HDMI_4K_PLUS = &H115&
        MWCAP_PRODUCT_ID_PRO_CAPTURE_DVI_4K = &H116&
        MWCAP_PRODUCT_ID_PRO_CAPTURE_AIO_4K = &H117&
        MWCAP_PRODUCT_ID_PRO_CAPTURE_SDI_4K_PLUS = &H118&
        MWCAP_PRODUCT_ID_PRO_CAPTURE_DUAL_HDMI_4K_PLUS = &H119&
        MWCAP_PRODUCT_ID_PRO_CAPTURE_DUAL_SDI_4K_PLUS = &H120&

        MWCAP_PRODUCT_ID_ECO_CAPTURE_OCTA_SDI = &H150&
        MWCAP_PRODUCT_ID_ECO_CAPTURE_DUAL_HDMI_M2 = &H151&
        MWCAP_PRODUCT_ID_ECO_CAPTURE_HDMI_4K_M2 = &H152&
        MWCAP_PRODUCT_ID_ECO_CAPTURE_DUAL_SDI_M2 = &H153&
        MWCAP_PRODUCT_ID_ECO_CAPTURE_QUAD_SDI_M2 = &H154&

        MWCAP_PRODUCT_ID_USB_CAPTURE_HDMI_PLUS = &H204&
        MWCAP_PRODUCT_ID_USB_CAPTURE_SDI_PLUS = &H205&
        MWCAP_PRODUCT_ID_USB_CAPTURE_HDMI = &H206&
        MWCAP_PRODUCT_ID_USB_CAPTURE_SDI = &H207&
        MWCAP_PRODUCT_ID_USB_CAPTURE_DVI = &H208&
        MWCAP_PRODUCT_ID_USB_CAPTURE_HDMI_4K = &H209&
        MWCAP_PRODUCT_ID_USB_CAPTURE_SDI_4K = &H210&
        MWCAP_PRODUCT_ID_USB_CAPTURE_AIO = &H211&
        MWCAP_PRODUCT_ID_USB_CAPTURE_AIO_4K = &H212&
    End Enum

    Public Enum MWCAP_VIDEO_INPUT_TYPE
        MWCAP_VIDEO_INPUT_TYPE_NONE = &H0&
        MWCAP_VIDEO_INPUT_TYPE_HDMI = &H1&
        MWCAP_VIDEO_INPUT_TYPE_VGA = &H2&
        MWCAP_VIDEO_INPUT_TYPE_SDI = &H4&
        MWCAP_VIDEO_INPUT_TYPE_COMPONENT = &H8&
        MWCAP_VIDEO_INPUT_TYPE_CVBS = &H10&
        MWCAP_VIDEO_INPUT_TYPE_YC = &H20&
    End Enum

    Public Enum MWCAP_AUDIO_INPUT_TYPE
        MWCAP_AUDIO_INPUT_TYPE_NONE = &H0&
        MWCAP_AUDIO_INPUT_TYPE_HDMI = &H1&
        MWCAP_AUDIO_INPUT_TYPE_SDI = &H2&
        MWCAP_AUDIO_INPUT_TYPE_LINE_IN = &H4&
        MWCAP_AUDIO_INPUT_TYPE_MIC_IN = &H8&
    End Enum

    Public Enum MWCAP_PCIE_LINK_TYPE
        MWCAP_PCIE_LINK_GEN_1 = &H1&
        MWCAP_PCIE_LINK_GEN_2 = &H2&
        MWCAP_PCIE_LINK_GEN_3 = &H4&
        MWCAP_PCIE_LINK_GEN_4 = &H8&
    End Enum

    Public Enum MWCAP_VIDEO_TIMING_TYPE
        MWCAP_VIDEO_TIMING_NONE = &H0&
        MWCAP_VIDEO_TIMING_LEGACY = &H1&
        MWCAP_VIDEO_TIMING_DMT = &H2&
        MWCAP_VIDEO_TIMING_CEA = &H4&
        MWCAP_VIDEO_TIMING_GTF = &H8&
        MWCAP_VIDEO_TIMING_CVT = &H10&
        MWCAP_VIDEO_TIMING_CVT_RB = &H20&
        MWCAP_VIDEO_TIMING_FAILSAFE = &H2000&
    End Enum

    Public Enum MWCAP_VIDEO_COLOR_FORMAT
        MWCAP_VIDEO_COLOR_FORMAT_UNKNOWN = &H0&
        MWCAP_VIDEO_COLOR_FORMAT_RGB = &H1&
        MWCAP_VIDEO_COLOR_FORMAT_YUV601 = &H2&
        MWCAP_VIDEO_COLOR_FORMAT_YUV709 = &H3&
        MWCAP_VIDEO_COLOR_FORMAT_YUV2020 = &H4&
        MWCAP_VIDEO_COLOR_FORMAT_YUV2020C = &H5& ''Constant luminance, not supported yet.
    End Enum

    Public Enum MWCAP_VIDEO_QUANTIZATION_RANGE
        MWCAP_VIDEO_QUANTIZATION_UNKNOWN = &H0&
        MWCAP_VIDEO_QUANTIZATION_FULL = &H1&          '' Black level: 0, White level: 255/1023/4095/65535
        MWCAP_VIDEO_QUANTIZATION_LIMITED = &H2&              '' Black level: 16/64/256/4096, White level: 235(240)/940(960)/3760(3840)/60160(61440)
    End Enum

    Public Enum MWCAP_VIDEO_SATURATION_RANGE
        MWCAP_VIDEO_SATURATION_UNKNOWN = &H0&
        MWCAP_VIDEO_SATURATION_FULL = &H1&          ''Min: 0, Max: 255/1023/4095/65535
        MWCAP_VIDEO_SATURATION_LIMITED = &H2&       '' Min: 16/64/256/4096, Max: 235(240)/940(960)/3760(3840)/60160(61440)
        MWCAP_VIDEO_SATURATION_EXTENDED_GAMUT = &H3&              '' Min: 1/4/16/256, Max: 254/1019/4079/65279
    End Enum

    Public Enum MWCAP_VIDEO_FRAME_TYPE
        MWCAP_VIDEO_FRAME_2D = &H0&
        MWCAP_VIDEO_FRAME_3D_TOP_AND_BOTTOM_FULL = &H1&
        MWCAP_VIDEO_FRAME_3D_TOP_AND_BOTTOM_HALF = &H2&
        MWCAP_VIDEO_FRAME_3D_SIDE_BY_SIDE_FULL = &H3&
        MWCAP_VIDEO_FRAME_3D_SIDE_BY_SIDE_HALF = &H4&
    End Enum

    Public Enum MWCAP_VIDEO_DEINTERLACE_MODE
        MWCAP_VIDEO_DEINTERLACE_WEAVE = &H0&
        MWCAP_VIDEO_DEINTERLACE_BLEND = &H1&
        MWCAP_VIDEO_DEINTERLACE_TOP_FIELD = &H2&
        MWCAP_VIDEO_DEINTERLACE_BOTTOM_FIELD = &H3&
    End Enum

    Public Enum MWCAP_VIDEO_ASPECT_RATIO_CONVERT_MODE
        MWCAP_VIDEO_ASPECT_RATIO_IGNORE = &H0&
        MWCAP_VIDEO_ASPECT_RATIO_CROPPING = &H1&
        MWCAP_VIDEO_ASPECT_RATIO_PADDING = &H2&
    End Enum

    Public Enum MWCAP_VIDEO_SYNC_TYPE
        VIDEO_SYNC_ALL = &H7&
        VIDEO_SYNC_HS_VS = &H1&
        VIDEO_SYNC_CS = &H2&
        VIDEO_SYNC_EMBEDDED = &H4&
    End Enum
    Public Structure RECT
        Public left As Integer
        Public top As Integer
        Public right As Integer
        Public bottom As Integer
    End Structure

    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi)> _
    Public Structure MWCAP_VIDEO_SYNC_INFO
        Dim bySyncType As Byte
        Dim bHSPolarity As Integer
        Dim bVSPolarity As Integer
        Dim bInterlaced As Integer
        Dim dwFrameDuration As Integer
        Dim wVSyncLineCount As Short
        Dim wFrameLineCount As Short
    End Structure
    Public Const MWCAP_DWORD_PARAMETER_FLAG_AUTO As ULong = &H1&

    Public Const MW_SERIAL_NO_LEN As Integer = 16
    Public Const MW_FAMILY_NAME_LEN As Integer = 64
    Public Const MW_PRODUCT_NAME_LEN As Integer = 64
    Public Const MW_FIRMWARE_NAME_LEN As Integer = 64

    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Pack:=1)> _
    Public Structure MWCAP_CHANNEL_INFO
        Dim wFamilyID As Short
        Dim wProductID As Short
        Dim chHardwareVersion As Byte
        Dim byFirmwareID As Byte
        Dim dwFirmwareVersion As Integer
        Dim dwDriverVersion As Integer
        <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=[MW_FAMILY_NAME_LEN])> _
        Dim szFamilyName As String
        <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=[MW_PRODUCT_NAME_LEN])> _
        Dim szProductName As String
        <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=[MW_FIRMWARE_NAME_LEN])> _
        Dim szFirmwareName As String
        <MarshalAs(UnmanagedType.ByValTStr, SizeConst:=[MW_SERIAL_NO_LEN])> _
        Dim szBoardSerialNo As String
        Dim byBoardIndex As Byte
        Dim byChannelIndex As Byte
    End Structure
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi)> _
    Public Structure MWCAP_PRO_CAPTURE_INFO
        Dim byPCIBusID As Byte
        Dim byPCIDevID As Byte
        Dim byLinkType As Byte
        Dim byLinkWidth As Byte
        Dim byBoardIndex As Byte
        Dim wMaxPayloadSize As Short
        Dim wMaxReadRequestSize As Short
        Dim cbTotalMemorySize As Integer
        Dim cbFreeMemorySize As Integer
    End Structure
    Public Enum MWCAP_VIDEO_SIGNAL_STATE
        MWCAP_VIDEO_SIGNAL_NONE = &H0&
        MWCAP_VIDEO_SIGNAL_UNSUPPORTED = &H1&
        MWCAP_VIDEO_SIGNAL_LOCKING = &H2&
        MWCAP_VIDEO_SIGNAL_LOCKED = &H3&
    End Enum
    ''MWSMPTE.h
    Public Enum SDI_TYPE
        SDI_TYPE_SD = &H0&
        SDI_TYPE_HD = &H1&
        SDI_TYPE_3GA = &H2&
        SDI_TYPE_3GB_DL = &H3&
        SDI_TYPE_3GB_DS = &H4&
        SDI_TYPE_DL_CH1 = &H5&
        SDI_TYPE_DL_CH2 = &H6&
        SDI_TYPE_6G_MODE1 = &H7&
        SDI_TYPE_6G_MODE2 = &H8&
    End Enum
    Public Enum SDI_SCANNING_FORMAT
        SDI_SCANING_INTERLACED = &H0&
        SDI_SCANING_SEGMENTED_FRAME = &H1&
        SDI_SCANING_PROGRESSIVE = &H3&
    End Enum
    Public Enum SDI_BIT_DEPTH
        SDI_BIT_DEPTH_8BIT = &H0&
        SDI_BIT_DEPTH_10BIT = &H1&
        SDI_BIT_DEPTH_12BIT = &H2&
    End Enum
    Public Enum SDI_SAMPLING_STRUCT
        SDI_SAMPLING_422_YCbCr = &H0&
        SDI_SAMPLING_444_YCbCr = &H1&
        SDI_SAMPLING_444_RGB = &H2&
        SDI_SAMPLING_420_YCbCr = &H3&
        SDI_SAMPLING_4224_YCbCrA = &H4&
        SDI_SAMPLING_4444_YCbCrA = &H5&
        SDI_SAMPLING_4444_RGBA = &H6&
        SDI_SAMPLING_4224_YCbCrD = &H8&
        SDI_SAMPLING_4444_YCbCrD = &H9&
        SDI_SAMPLING_4444_RGBD = &HA&
        SDI_SAMPLING_444_XYZ = &HE&
    End Enum
    ''MWHDMIPackets.h
    Public Enum HDMI_PXIEL_ENCODING
        HDMI_ENCODING_RGB_444 = &H0&
        HDMI_ENCODING_YUV_422 = &H1&
        HDMI_ENCODING_YUV_444 = &H2&
        HDMI_ENCODING_YUV_420 = &H3&
    End Enum

    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Pack:=1)> _
    Public Structure HDMI_INFOFRAME_HEADER
        Dim byPacketType As Byte
        Dim byVersion As Byte
        Dim byLength As Byte '' : 5;
        'Dim byReservedZero As Byte '' : 3;
    End Structure

    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Pack:=1)> _
    Public Structure HDMI_AVI_INFOFRAME_PAYLOAD
        Dim byScanInfo As Byte '' : 2;
        Dim byBarDataPresent As Byte '' : 2;
        Dim byActiveFormatInfoPresent As Byte '' : 1;
        Dim byRGB_YCbCr As Byte '' : 2;
        Dim byFutureUseByte1 As Byte '' : 1;

        Dim byActivePortionAspectRatio As Byte '' : 4;
        Dim byCodedFrameAspectRatio As Byte '' : 2;
        Dim byColorimetry As Byte '' : 2;

        Dim byNonUniformPictureScaling As Byte '' : 2;
        Dim byRGBQuantizationRange As Byte '' : 2;
        Dim byExtendedColorimetry As Byte '' : 3;
        Dim byITContent As Byte '' : 1;

        Dim byVIC As Byte '' : 7;
        Dim byFutureUseByte4 As Byte '' : 1;

        Dim byPixelRepetitionFactor As Byte '' : 4;
        Dim byITContentType As Byte '' : 2;
        Dim byYCCQuantizationRange As Byte '' : 2;

        Dim wEndOfTopBar As Short
        Dim wStartOfBottomBar As Short
        Dim wEndOfLeftBar As Short
        Dim wStartOfRightBar As Short
    End Structure

    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Pack:=1)> _
    Public Structure HDMI_AUDIO_INFOFRAME_PAYLOAD
        Dim byChannelCount As Byte '' : 3;		// +1 for channel count
        Dim byReserved1 As Byte '': 1;
        Dim byAudioCodingType As Byte '': 4;

        Dim bySampleSize As Byte '': 2;
        Dim bySampleFrequency As Byte '': 3;
        Dim byReserved2 As Byte '': 3;

        Dim byAudioCodingExtensionType As Byte '': 5;
        Dim byReserved3 As Byte '': 3;

        Dim byChannelAllocation As Byte

        Dim byLFEPlaybackLevel As Byte '': 2;
        Dim byReserved4 As Byte '': 1;
        Dim byLevelShiftValue As Byte '': 4;
        Dim byDownMixInhibitFlag As Byte '': 1;
    End Structure

    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Pack:=1)> _
    Public Structure HDMI_SPD_INFOFRAME_PAYLOAD
        <MarshalAsAttribute(UnmanagedType.ByValArray, SizeConst:=8, ArraySubType:=UnmanagedType.I4)> _
        Dim achVendorName() As Byte
        <MarshalAsAttribute(UnmanagedType.ByValArray, SizeConst:=16, ArraySubType:=UnmanagedType.I4)> _
        Dim achProductDescription() As Byte
        Dim bySourceInformation As Byte
    End Structure

    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Pack:=1)> _
    Public Structure HDMI14B_VS_DATA_EXT_RES
        Dim byHDMI_VIC As Byte
    End Structure

    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Pack:=1)> _
    Public Structure HDMI14B_VS_DATA_3D_FORMAT
        Dim byReserved1 As Byte '' : 3;
        Dim by3DMetaPresent As Byte '' : 1;
        Dim by3DStructure As Byte '' : 4;

        Dim byReserved2 As Byte '' : 4;
        Dim by3DExtData As Byte '' : 4;

        Dim by3DMetadataLength As Byte '' : 5;
        Dim by3DMetadataType As Byte '' : 3;
    End Structure

    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Pack:=1)> _
    Public Structure uion_hdmi14b_vs_data
        <FieldOffset(0)> Dim vsDataExtRes As HDMI14B_VS_DATA_EXT_RES
        <FieldOffset(0)> Dim vsData3DFormat As HDMI14B_VS_DATA_3D_FORMAT
    End Structure

    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Pack:=1)> _
    Public Structure HDMI14B_VS_DATA
        Dim byReserved1 As Byte '' : 5;
        Dim byHDMIVideoFormat As Byte '' : 3;
        Dim union As uion_hdmi14b_vs_data
    End Structure

    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Pack:=1)> _
    Public Structure uion_hdmi_vs_frame_payload
        <FieldOffset(0)> <MarshalAsAttribute(UnmanagedType.ByValArray, SizeConst:=24, ArraySubType:=UnmanagedType.I4)> _
        Dim abyVSData() As Byte
        <FieldOffset(0)> Dim vsDataHDMI14B As HDMI14B_VS_DATA
    End Structure

    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Pack:=1)> _
    Public Structure HDMI_VS_INFOFRAME_PAYLOAD
        <MarshalAsAttribute(UnmanagedType.ByValArray, SizeConst:=3, ArraySubType:=UnmanagedType.I4)> _
        Dim abyRegistrationId() As Byte
        Dim union As uion_hdmi_vs_frame_payload
#If DEBUG Then
        <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function GetRegistrationId() As Integer
        End Function
#Else
        <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function GetRegistrationId() As Integer
        End Function
#End If
    End Structure

    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Pack:=1)> _
    Public Structure uion_hdmi_info_frame_packet
        <FieldOffset(0)> <MarshalAsAttribute(UnmanagedType.ByValArray, SizeConst:=27, ArraySubType:=UnmanagedType.I4)> _
        Dim abyPayload() As Byte
        <FieldOffset(0)> Dim aviInfoFramePayload As HDMI_AVI_INFOFRAME_PAYLOAD
        <FieldOffset(0)> Dim audioInfoFramePayload As HDMI_AUDIO_INFOFRAME_PAYLOAD
        <FieldOffset(0)> Dim spdInfoFramePayload As HDMI_SPD_INFOFRAME_PAYLOAD
        <FieldOffset(0)> Dim vsInfoFramePayload As HDMI_VS_INFOFRAME_PAYLOAD
    End Structure

    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Pack:=1)> _
    Public Structure HDMI_INFOFRAME_PACKET
        Dim header As HDMI_INFOFRAME_HEADER
        Dim byChecksum As Byte
        Dim union As uion_hdmi_info_frame_packet
#If DEBUG Then
        <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function IsValid() As Boolean
        End Function
#Else
        <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function IsValid() As Boolean
        End Function
#End If
    End Structure
    ''MWIEC60958.h
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Pack:=1)> _
    Public Structure Professional
        Dim byProfessional As Byte ''= 1
        Dim byNotLPCM As Byte ''  1;
        Dim byEncodedAudioSignalEmphasis As Byte '' 3;	// 000: Emphasis not indicated, 001: No emphasis, 011: 50/15 us emphasis, 111: CCITT J.17 emphasis
        Dim bySourceSamplingFreqUnlocked As Byte '' : 1;
        Dim byEncodedSamplingFreq As Byte '' : 2;			// 00: Not indicated, 10: 48, 01: 44.1, 11: 32

        Dim byEncodedChannelMode As Byte '' : 4;
        Dim byEncodedUserBitsManagement As Byte '': 4;

        Dim byZero As Byte '' : 1;
        Dim bySingleCoordinationSignal As Byte '' : 1;
        Dim byWordLength As Byte '' : 4;					// 0010: 16, 1100: 17, 0100: 18, 1000: 19, 1010: 20, 0011: 20, 1101: 21, 0101: 22, 1001: 23, 1011: 24
        Dim byAlignmentLevel As Byte '' : 2;				// 00: Not indicated, 10: 20 dB, 01: 18.06 dB, 11: Reserved

        Dim byChannelNumberOrMultiChannelMode As Byte '' : 7;
        Dim byDefinedMultiChannelMode As Byte '' : 1;

        Dim byReferenceSignal As Byte '' : 2;				// 00: Not a reference signal, 10: Grade 1, 01: Grade 2, 11: Reserved
        Dim byReserved1 As Byte '' : 1;
        Dim bySamplingFreq As Byte '' : 4;					// 0000: Not indicated, 0001: 24, 0010: 96, 0011: 192, 1001: 22.05, 1010: 88.2, 1011: 176.4, 1111: User defined
        Dim bySamplingFreqScaling As Byte '' : 1;

        Dim byReserved2 As Byte '';

        <MarshalAsAttribute(UnmanagedType.ByValArray, SizeConst:=4, ArraySubType:=UnmanagedType.I4)> _
        Dim achChannelOrigin() As Byte '';
        <MarshalAsAttribute(UnmanagedType.ByValArray, SizeConst:=4, ArraySubType:=UnmanagedType.I4)> _
        Dim achChannelDestination() As Byte
        Dim dwLocalSampleAddress As Integer
        Dim dwTimeOfDaySampleAddress As Integer
        Dim byReliabilityFlags As Byte

        Dim byCRC As Byte
    End Structure
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Pack:=1)> _
    Public Structure Consumer
        Dim byProfessional As Byte '' : 1;
        Dim byNotLPCM As Byte '' : 1;
        Dim byNoCopyright As Byte '' : 1;
        Dim byAdditionalFormatInfo As Byte '' : 3;
        Dim byMode As Byte '' : 2;

        Dim byCategoryCode As Byte

        Dim bySourceNumber As Byte '' : 4;
        Dim byChannelNumber As Byte '' : 4;

        Dim bySamplingFreq As Byte '' : 4;					// 0100: 22.05, 0000: 44.1, 1000: 88.2, 1100: 176.4, 0110: 24, 0010: 48, 1010: 96, 1110: 192, 0011: 32, 0001: Not indicated, 1001: 768
        Dim byClockAccuracy As Byte '' : 2;				// 00: Level II, 10: Level I, 01: Level III, 11: Not matched
        Dim byReserved1 As Byte '' : 2;

        Dim byWordLength As Byte '' : 4;					// 0010: 16, 1100: 17, 0100: 18, 1000: 19, 1010: 20, 0011: 20, 1101: 21, 0101: 22, 1001: 23, 1011: 24
        Dim byOrigSamplingFreq As Byte '' : 4;				// 1111: 44.1, 0111: 88.2, 1011: 22.05, 0011: 176.4, 1101: 48, 0101: 96, 1001: 24, 0001: 192, 0110: 8, 1010: 11.025, 0010: 12, 1100: 32, 1000: 16, 0000: Not indicated

        Dim byCGMS_A As Byte
    End Structure

    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Pack:=1)> _
    Public Structure IEC60958_CHANNEL_STATUS
        <MarshalAsAttribute(UnmanagedType.ByValArray, SizeConst:=24, ArraySubType:=UnmanagedType.I4)> _
        Dim abyData() As Byte
        <MarshalAsAttribute(UnmanagedType.ByValArray, SizeConst:=12, ArraySubType:=UnmanagedType.I4)> _
        Dim awData() As Short
        <MarshalAsAttribute(UnmanagedType.ByValArray, SizeConst:=6, ArraySubType:=UnmanagedType.I4)> _
        Dim adwData() As Integer

        Dim professional As Professional
        Dim consumer As Consumer
    End Structure

    ''MWCaptureExtension.h
    Public Enum MWCAP_AUDIO_CAPTURE_NODE
        MWCAP_AUDIO_CAPTURE_NODE_DEFAULT = &H0&
        MWCAP_AUDIO_CAPTURE_NODE_EMBEDDED_CAPTURE = &H1&
        MWCAP_AUDIO_CAPTURE_NODE_MICROPHONE = &H2&
        MWCAP_AUDIO_CAPTURE_NODE_USB_CAPTURE = &H3&
        MWCAP_AUDIO_CAPTURE_NODE_LINE_IN = &H4&
    End Enum

    Public Enum MWCAP_HDMI_INFOFRAME_ID
        MWCAP_HDMI_INFOFRAME_ID_AVI = &H0&
        MWCAP_HDMI_INFOFRAME_ID_AUDIO = &H1&
        MWCAP_HDMI_INFOFRAME_ID_SPD = &H2&
        MWCAP_HDMI_INFOFRAME_ID_MS = &H3&
        MWCAP_HDMI_INFOFRAME_ID_VS = &H4&
        MWCAP_HDMI_INFOFRAME_ID_ACP = &H5&
        MWCAP_HDMI_INFOFRAME_ID_ISRC1 = &H6&
        MWCAP_HDMI_INFOFRAME_ID_ISRC2 = &H7&
        MWCAP_HDMI_INFOFRAME_ID_GAMUT = &H8&
        MWCAP_HDMI_INFOFRAME_ID_VBI = &H9&
        MWCAP_HDMI_INFOFRAME_ID_HDR = &HA&
        MWCAP_HDMI_INFOFRAME_COUNT = &HB&
    End Enum

    Public Enum MWCAP_HDMI_INFOFRAME_MASK
        MWCAP_HDMI_INFOFRAME_MASK_AVI = (1 << MWCAP_HDMI_INFOFRAME_ID.MWCAP_HDMI_INFOFRAME_ID_AVI)
        MWCAP_HDMI_INFOFRAME_MASK_AUDIO = (1 << MWCAP_HDMI_INFOFRAME_ID.MWCAP_HDMI_INFOFRAME_ID_AUDIO)
        MWCAP_HDMI_INFOFRAME_MASK_SPD = (1 << MWCAP_HDMI_INFOFRAME_ID.MWCAP_HDMI_INFOFRAME_ID_SPD)
        MWCAP_HDMI_INFOFRAME_MASK_MS = (1 << MWCAP_HDMI_INFOFRAME_ID.MWCAP_HDMI_INFOFRAME_ID_MS)
        MWCAP_HDMI_INFOFRAME_MASK_VS = (1 << MWCAP_HDMI_INFOFRAME_ID.MWCAP_HDMI_INFOFRAME_ID_VS)
        MWCAP_HDMI_INFOFRAME_MASK_ACP = (1 << MWCAP_HDMI_INFOFRAME_ID.MWCAP_HDMI_INFOFRAME_ID_ACP)
        MWCAP_HDMI_INFOFRAME_MASK_ISRC1 = (1 << MWCAP_HDMI_INFOFRAME_ID.MWCAP_HDMI_INFOFRAME_ID_ISRC1)
        MWCAP_HDMI_INFOFRAME_MASK_ISRC2 = (1 << MWCAP_HDMI_INFOFRAME_ID.MWCAP_HDMI_INFOFRAME_ID_ISRC2)
        MWCAP_HDMI_INFOFRAME_MASK_GAMUT = (1 << MWCAP_HDMI_INFOFRAME_ID.MWCAP_HDMI_INFOFRAME_ID_GAMUT)
        MWCAP_HDMI_INFOFRAME_MASK_VBI = (1 << MWCAP_HDMI_INFOFRAME_ID.MWCAP_HDMI_INFOFRAME_ID_VBI)
        MWCAP_HDMI_INFOFRAME_MASK_HDR = (1 << MWCAP_HDMI_INFOFRAME_ID.MWCAP_HDMI_INFOFRAME_ID_HDR)
    End Enum

    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Pack:=1)> _
    Public Structure MWCAP_VIDEO_BUFFER_INFO
        Dim cMaxFrames As Integer

        Dim iNewestBuffering As Byte
        Dim iBufferingFieldIndex As Byte

        Dim iNewestBuffered As Byte
        Dim iBufferedFieldIndex As Byte

        Dim iNewestBufferedFullFrame As Byte
        Dim cBufferedFullFrames As Byte
    End Structure

    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Pack:=1)> _
    Public Structure MWCAP_SDI_ANC_PACKET
        Dim byDID As Byte
        Dim bySDID As Byte
        Dim byDC As Byte
        <MarshalAsAttribute(UnmanagedType.ByValArray, SizeConst:=255, ArraySubType:=UnmanagedType.I4)> _
        Dim abyUDW() As Byte
        <MarshalAsAttribute(UnmanagedType.ByValArray, SizeConst:=2, ArraySubType:=UnmanagedType.I4)> _
        Dim abyReserved() As Byte
    End Structure

    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Pack:=1)> _
    Public Structure MWCAP_AUDIO_SIGNAL_STATUS
        Dim wChannelValid As Short
        Dim bLPCM As Byte
        Dim cBitsPerSample As Byte
        Dim dwSampleRate As Integer
        Dim bChannelStatusValid As Byte
        Dim channelStatus As IEC60958_CHANNEL_STATUS
    End Structure
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Pack:=1)> _
    Public Structure MWCAP_HDMI_VIDEO_TIMING
        Dim bInterlaced As Byte
        Dim dwFrameDuration As Integer
        Dim wHSyncWidth As Short
        Dim wHFrontPorch As Short
        Dim wHBackPorch As Short
        Dim wHActive As Short
        Dim wHTotalWidth As Short
        Dim wField0VSyncWidth As Short
        Dim wField0VFrontPorch As Short
        Dim wField0VBackPorch As Short
        Dim wField0VActive As Short
        Dim wField0VTotalHeight As Short
        Dim wField1VSyncWidth As Short
        Dim wField1VFrontPorch As Short
        Dim wField1VBackPorch As Short
        Dim wField1VActive As Short
        Dim wField1VTotalHeight As Short
    End Structure
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Pack:=1)> _
    Public Structure MWCAP_HDMI_SPECIFIC_STATUS
        Dim bHDMIMode As Byte
        Dim bHDCP As Byte
        Dim byBitDepth As Byte
        Dim pixelEncoding As HDMI_PXIEL_ENCODING
        Dim byVIC As Byte
        Dim bITContent As Byte
        Dim b3DFormat As Byte
        Dim by3DStructure As Byte
        Dim bySideBySideHalfSubSampling As Byte
        Dim videoTiming As MWCAP_HDMI_VIDEO_TIMING
    End Structure
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Pack:=1)> _
    Public Structure MWCAP_SDI_SPECIFIC_STATUS
        Dim sdiType As SDI_TYPE
        Dim sdiScanningFormat As SDI_SCANNING_FORMAT
        Dim sdiBitDepth As SDI_BIT_DEPTH
        Dim sdiSamplingStruct As SDI_SAMPLING_STRUCT
        Dim bST352DataValid As Byte
        Dim dwST352Data As Integer
    End Structure
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Pack:=1)> _
    Public Structure MWCAP_VIDEO_TIMING
        Dim dwType As Integer
        Dim dwPixelClock As Integer
        Dim bInterlaced As Byte
        Dim bySyncType As Byte
        Dim bHSPolarity As Byte
        Dim bVSPolarity As Byte
        Dim wHActive As Short
        Dim wHFrontPorch As Short
        Dim wHSyncWidth As Short
        Dim wHBackPorch As Short
        Dim wVActive As Short
        Dim wVFrontPorch As Short
        Dim wVSyncWidth As Short
        Dim wVBackPorch As Short
    End Structure
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Pack:=1)> _
    Public Structure MWCAP_VIDEO_TIMING_SETTINGS
        Dim wAspectX As Short
        Dim wAspectY As Short
        Dim x As Short
        Dim y As Short
        Dim cx As Short
        Dim cy As Short
        Dim cxTotal As Short
        Dim byClampPos As Byte
    End Structure
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Pack:=1)> _
    Public Structure MWCAP_COMPONENT_SPECIFIC_STATUS
        Dim syncInfo As MWCAP_VIDEO_SYNC_INFO
        Dim bTriLevelSync As Byte
        Dim videoTiming As MWCAP_VIDEO_TIMING   '' Not valid for custom video timing
        Dim videoTimingSettings As MWCAP_VIDEO_TIMING_SETTINGS
    End Structure
    Public Enum MWCAP_SD_VIDEO_STANDARD
        MWCAP_SD_VIDEO_NONE = &H0&
        MWCAP_SD_VIDEO_NTSC_M = &H1&
        MWCAP_SD_VIDEO_NTSC_433 = &H2&
        MWCAP_SD_VIDEO_PAL_M = &H3&
        MWCAP_SD_VIDEO_PAL_60 = &H4&
        MWCAP_SD_VIDEO_PAL_COMBN = &H5&
        MWCAP_SD_VIDEO_PAL_BGHID = &H6&
        MWCAP_SD_VIDEO_SECAM = &H7&
        MWCAP_SD_VIDEO_SECAM_60 = &H8&
    End Enum
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Pack:=1)> _
    Public Structure MWCAP_CVBS_YC_SPECIFIC_STATUS
        Dim standard As MWCAP_SD_VIDEO_STANDARD
        Dim b50Hz As Byte
    End Structure
    <StructLayout(LayoutKind.Explicit, CharSet:=CharSet.Ansi, Pack:=1)> _
    Public Structure uion_input_specific_status
        <FieldOffset(0)> Dim sdiStatus As MWCAP_SDI_SPECIFIC_STATUS
        <FieldOffset(0)> Dim hdmiStatus As MWCAP_HDMI_SPECIFIC_STATUS
        <FieldOffset(0)> Dim vgaComponentStatus As MWCAP_COMPONENT_SPECIFIC_STATUS
        <FieldOffset(0)> Dim cvbsYcStatus As MWCAP_CVBS_YC_SPECIFIC_STATUS
    End Structure
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Pack:=1)> _
    Public Structure MWCAP_INPUT_SPECIFIC_STATUS
        Dim bValid As Byte
        Dim dwVideoInputType As Integer
        Dim uion As uion_input_specific_status
    End Structure
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Pack:=1)> _
    Public Structure MWCAP_VIDEO_SIGNAL_STATUS
        Dim state As MWCAP_VIDEO_SIGNAL_STATE
        Dim x As Integer
        Dim y As Integer
        Dim cx As Integer
        Dim cy As Integer
        Dim cxTotal As Integer
        Dim cyTotal As Integer
        Dim bInterlaced As Byte
        Dim dwFrameDuration As Integer
        Dim nAspectX As Integer
        Dim nAspectY As Integer
        Dim bSegmentedFrame As Byte
        Dim frameType As MWCAP_VIDEO_FRAME_TYPE
        Dim colorFormat As MWCAP_VIDEO_COLOR_FORMAT
        Dim quantRange As MWCAP_VIDEO_QUANTIZATION_RANGE
        Dim satRange As MWCAP_VIDEO_SATURATION_RANGE
    End Structure
    Public Const MWCAP_NOTIFY_INPUT_SORUCE_START_SCAN As ULong = &H1&
    Public Const MWCAP_NOTIFY_INPUT_SORUCE_STOP_SCAN As ULong = &H2&
    Public Const MWCAP_NOTIFY_INPUT_SORUCE_SCAN_CHANGE As ULong = &H3&

    Public Const MWCAP_NOTIFY_VIDEO_INPUT_SOURCE_CHANGE As ULong = &H4&
    Public Const MWCAP_NOTIFY_AUDIO_INPUT_SOURCE_CHANGE As ULong = &H8&


    Public Const MWCAP_NOTIFY_INPUT_SPECIFIC_CHANGE As ULong = &H10&
    Public Const MWCAP_NOTIFY_VIDEO_SIGNAL_CHANGE As ULong = &H20&
    Public Const MWCAP_NOTIFY_AUDIO_SIGNAL_CHANGE As ULong = &H40&

    Public Const MWCAP_NOTIFY_VIDEO_FIELD_BUFFERING As ULong = &H80&
    Public Const MWCAP_NOTIFY_VIDEO_FRAME_BUFFERING As ULong = &H100&
    Public Const MWCAP_NOTIFY_VIDEO_FIELD_BUFFERED As ULong = &H200&
    Public Const MWCAP_NOTIFY_VIDEO_FRAME_BUFFERED As ULong = &H400&
    Public Const MWCAP_NOTIFY_VIDEO_SMPTE_TIME_CODE As ULong = &H800&


    Public Const MWCAP_NOTIFY_AUDIO_FRAME_BUFFERED As ULong = &H1000&
    Public Const MWCAP_NOTIFY_AUDIO_INPUT_RESET As ULong = &H2000&
    Public Const MWCAP_NOTIFY_VIDEO_SAMPLING_PHASE_CHANGE As ULong = &H4000&

    Public Const MWCAP_NOTIFY_LOOP_THROUGH_CHANGED As ULong = &H8000&
    Public Const MWCAP_NOTIFY_LOOP_THROUGH_EDID_CHANGED As ULong = &H10000&

    Public Const MWCAP_NOTIFY_NEW_SDI_ANC_PACKET As ULong = &H20000&

    Public Const MWCAP_MAX_VIDEO_FRAME_COUNT As ULong = &H8&

    Public Enum MWCAP_VIDEO_FRAME_STATE
        MWCAP_VIDEO_FRAME_STATE_INITIAL = &H0&
        MWCAP_VIDEO_FRAME_STATE_F0_BUFFERING = &H1&
        MWCAP_VIDEO_FRAME_STATE_F1_BUFFERING = &H2&
        MWCAP_VIDEO_FRAME_STATE_BUFFERED = &H3&
    End Enum

    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi)> _
    Public Structure MWCAP_SMPTE_TIMECODE
        Dim byFrames As Byte
        Dim bySeconds As Byte
        Dim byMinutes As Byte
        Dim byHours As Byte
    End Structure

    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi)> _
    Public Structure MWCAP_VIDEO_FRAME_INFO
        Dim state As MWCAP_VIDEO_FRAME_STATE

        Dim bInterlaced As Byte
        Dim bSegmentedFrame As Byte
        Dim bTopFieldFirst As Byte
        Dim bTopFieldInverted As Byte

        Dim cx As Integer
        Dim cy As Integer
        Dim nAspectX As UInteger
        Dim nAspectY As UInteger

        <MarshalAsAttribute(UnmanagedType.ByValArray, SizeConst:=2, ArraySubType:=UnmanagedType.I4)> _
        Dim allFieldStartTimes() As ULong
        <MarshalAsAttribute(UnmanagedType.ByValArray, SizeConst:=2, ArraySubType:=UnmanagedType.I4)> _
        Dim allFieldBufferedTimes() As ULong
        Dim aSMPTETimeCodes As MWCAP_SMPTE_TIMECODE
    End Structure

    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi)> _
    Public Structure MWCAP_VIDEO_CAPTURE_OPEN
        Dim hEvent As IntPtr
    End Structure
    Public Const MWCAP_VIDEO_MAX_NUM_OSD_RECTS As Integer = 4

    Public Const MWCAP_VIDEO_FRAME_ID_NEWEST_BUFFERED As Integer = -1
    Public Const MWCAP_VIDEO_FRAME_ID_NEWEST_BUFFERING As Integer = -2
    Public Const MWCAP_VIDEO_FRAME_ID_NEXT_BUFFERED As Integer = -3
    Public Const MWCAP_VIDEO_FRAME_ID_NEXT_BUFFERING As Integer = -4
    Public Const MWCAP_VIDEO_FRAME_ID_EMPTY As Integer = -100

    Public Const MWCAP_VIDEO_PROCESS_FLIP As ULong = &H1&
    Public Const MWCAP_VIDEO_PROCESS_MIRROR As ULong = &H2&
    Public Enum MWCAP_VIDEO_PLANE_ADDRESS_TYPE
        MWCAP_VIDEO_PLANE_VIRTUAL_ADDRESS = &H0&
        MWCAP_VIDEO_PLANE_PHYSICAL_ADDRESS = &H1&
    End Enum
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Pack:=1)> _
    Public Structure MWCAP_VIDEO_CAPTURE_STATUS
        Dim pvContext As Long

        Dim bPhysicalAddress As Byte
        Dim union As Long
        Dim iFrame As Integer
        Dim bFrameCompleted As Byte

        Dim cyCompleted As Short
        Dim cyCompletedPrev As Short
    End Structure
    Public Const MWCAP_AUDIO_FRAME_SYNC_CODE As ULong = &HFECA0357&
    Public Const MWCAP_AUDIO_SAMPLES_PER_FRAME As Integer = 192
    Public Const MWCAP_AUDIO_MAX_NUM_CHANNELS As Integer = 8
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi)> _
    Public Structure MWCAP_AUDIO_CAPTURE_FRAME
        Dim cFrameCount As Integer
        Dim iFrame As Integer
        Dim dwSyncCode As Integer
        Dim dwReserved As Integer
        Dim llTimestamp As Long
        <MarshalAsAttribute(UnmanagedType.ByValArray, SizeConst:=(MWCAP_AUDIO_SAMPLES_PER_FRAME * MWCAP_AUDIO_MAX_NUM_CHANNELS), ArraySubType:=UnmanagedType.I4)> _
        Dim adwSamples() As Integer
    End Structure
    ''MWUSBCaptureExtension.h
    Public Enum MWUSBHOT_PLUG_EVETN
        USBHOT_PLUG_EVENT_DEVICE_ARRIVED = &H1&
        USBHOT_PLUG_EVENT_DEVICE_LEFT = &H2&
    End Enum
    Public Delegate Sub LPFN_HOT_PLUG_STDCALL_CALLBACK(ByVal usb_event As MWUSBHOT_PLUG_EVETN, ByVal pszDevicePath As IntPtr, ByVal pParam As IntPtr)

    Public Delegate Sub VIDEO_CAPTURE_STDCALL_CALLBACK(ByVal pbFrame As IntPtr, ByVal vbFrame As Integer, ByVal u64TimeStamp As ULong, ByVal pParam As IntPtr)

    Public Delegate Sub AUDIO_CAPTURE_STDCALL_CALLBACK(ByVal pbFrame As IntPtr, ByVal vbFrame As Integer, ByVal u64TimeStamp As ULong, ByVal pParam As IntPtr)

    ''MWUSBCapture.h
#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWUSBRegisterHotPlugWithStdCallBack(ByVal lpfnCallback As LPFN_HOT_PLUG_STDCALL_CALLBACK, ByVal pParam As IntPtr) As MW_RESULT
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWUSBRegisterHotPlugWithStdCallBack(ByVal lpfnCallback As LPFN_HOT_PLUG_STDCALL_CALLBACK, ByVal pParam As IntPtr) As MW_RESULT
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWUSBUnRegisterHotPlug() As MW_RESULT
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWUSBUnRegisterHotPlug() As MW_RESULT
    End Function
#End If

    ''MWCapture.h
#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWCreateVideoCaptureWithStdCallBack(ByVal hChannel As IntPtr, ByVal nWidth As Integer, ByVal nHeight As Integer, ByVal nFourcc As UInteger, ByVal nFrameDuration As Integer, ByVal callback As VIDEO_CAPTURE_STDCALL_CALLBACK, ByVal pParam As IntPtr) As IntPtr
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWCreateVideoCaptureWithStdCallBack(ByVal hChannel As IntPtr, ByVal nWidth As Integer, ByVal nHeight As Integer, ByVal nFourcc As UInteger, ByVal nFrameDuration As Integer, ByVal callback As VIDEO_CAPTURE_STDCALL_CALLBACK, ByVal pParam As IntPtr) As IntPtr
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWDestoryVideoCapture(ByVal hVideo As IntPtr) As IntPtr
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWDestoryVideoCapture(ByVal hVideo As IntPtr) As IntPtr
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWCreateAudioCaptureWithStdCallBack(ByVal hChannel As IntPtr, ByVal captureNode As MWCAP_AUDIO_CAPTURE_NODE, ByVal dwSamplesPerSec As Integer, ByVal wBitsPerSample As Short, ByVal wChannels As Short, ByVal callback As AUDIO_CAPTURE_STDCALL_CALLBACK, ByVal pParam As IntPtr) As IntPtr
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWCreateAudioCaptureWithStdCallBack(ByVal hChannel As IntPtr, ByVal captureNode As MWCAP_AUDIO_CAPTURE_NODE, ByVal dwSamplesPerSec As Integer, ByVal wBitsPerSample As Short, ByVal wChannels As Short, ByVal callback As AUDIO_CAPTURE_STDCALL_CALLBACK, ByVal pParam As IntPtr) As IntPtr
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWDestoryAudioCapture(ByVal hAudio As IntPtr) As IntPtr
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWDestoryAudioCapture(ByVal hAudio As IntPtr) As IntPtr
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWCaptureSetSDIANCType(ByVal hChannel As IntPtr, ByVal byIndex As Byte, ByVal bHANC As Byte, ByVal bVANC As Byte, ByVal byDID As Byte, ByVal bySDID As Byte) As MW_RESULT
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWCaptureSetSDIANCType(ByVal hChannel As IntPtr, ByVal byIndex As Byte, ByVal bHANC As Byte, ByVal bVANC As Byte, ByVal byDID As Byte, ByVal bySDID As Byte) As MW_RESULT
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWCaptureGetSDIANCPacket(ByVal hChannel As IntPtr, ByRef pPacket As MWCAP_SDI_ANC_PACKET) As MW_RESULT
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWCaptureGetSDIANCPacket(ByVal hChannel As IntPtr, ByRef pPacket As MWCAP_SDI_ANC_PACKET) As MW_RESULT
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWGetVersion(ByRef pbyMaj As Byte, ByRef pbyMin As Byte, ByRef pwBuild As Integer) As MW_RESULT
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWGetVersion(ByRef pbyMaj As Byte, ByRef pbyMin As Byte, ByRef pwBuild As Integer) As MW_RESULT
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWCaptureInitInstance() As Byte
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWCaptureInitInstance() As Byte
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Sub MWCaptureExitInstance()
    End Sub
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Sub MWCaptureExitInstance()
    End Sub
#End If

#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWRefreshDevice() As MW_RESULT
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWRefreshDevice() As MW_RESULT
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWGetChannelCount() As Integer
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWGetChannelCount() As Integer
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWGetChannelInfoByIndex(ByVal nIndex As Integer, ByRef channelInfo As MWCAP_CHANNEL_INFO) As MW_RESULT
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWGetChannelInfoByIndex(ByVal nIndex As Integer, ByRef channelInfo As MWCAP_CHANNEL_INFO) As MW_RESULT
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWGetFamilyInfoByIndex(ByVal nIndex As Integer, ByVal pFamilyInfo As IntPtr, ByVal dwSize As ULong) As MW_RESULT
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWGetFamilyInfoByIndex(ByVal nIndex As Integer, ByVal pFamilyInfo As IntPtr, ByVal dwSize As ULong) As MW_RESULT
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWGetDevicePath(ByVal nIndex As Integer, ByVal pDevicePath As IntPtr) As MW_RESULT
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWGetDevicePath(ByVal nIndex As Integer, ByVal pDevicePath As IntPtr) As MW_RESULT
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWOpenChannel(ByVal nBoardValue As Integer, ByVal nChannelIndex As Integer) As IntPtr
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWOpenChannel(ByVal nBoardValue As Integer, ByVal nChannelIndex As Integer) As IntPtr
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWOpenChannelByPath(ByVal pszDevicePath As IntPtr) As IntPtr
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWOpenChannelByPath(ByVal pszDevicePath As IntPtr) As IntPtr
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Sub MWCloseChannel(ByVal hChannel As IntPtr)
    End Sub
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Sub MWCloseChannel(ByVal hChannel As IntPtr)
    End Sub
#End If

#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWGetChannelInfo(ByVal hChannel As IntPtr, ByRef channelInfo As MWCAP_CHANNEL_INFO) As MW_RESULT
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWGetChannelInfo(ByVal hChannel As IntPtr, ByRef channelInfo As MWCAP_CHANNEL_INFO) As MW_RESULT
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWGetFamilyInfo(ByVal hChannel As IntPtr, ByVal pFamilyInfo As IntPtr, ByVal dwSize As ULong) As MW_RESULT
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWGetFamilyInfo(ByVal hChannel As IntPtr, ByVal pFamilyInfo As IntPtr, ByVal dwSize As ULong) As MW_RESULT
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWGetVideoCaps(ByVal hChannel As IntPtr, ByVal pVideoCaps As IntPtr) As MW_RESULT
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWGetVideoCaps(ByVal hChannel As IntPtr, ByVal pVideoCaps As IntPtr) As MW_RESULT
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWGetAudioCaps(ByVal hChannel As IntPtr, ByVal pAudioCaps As IntPtr) As MW_RESULT
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWGetAudioCaps(ByVal hChannel As IntPtr, ByVal pAudioCaps As IntPtr) As MW_RESULT
    End Function
#End If
#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWGetInputSpecificStatus(ByVal hChannel As IntPtr, ByRef pInputStatus As MWCAP_INPUT_SPECIFIC_STATUS) As MW_RESULT
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWGetInputSpecificStatus(ByVal hChannel As IntPtr, ByRef pInputStatus As MWCAP_INPUT_SPECIFIC_STATUS) As MW_RESULT
    End Function
#End If
#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWGetVideoSignalStatus(ByVal hChannel As IntPtr, ByVal pSignalStatus As IntPtr) As MW_RESULT
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWGetVideoSignalStatus(ByVal hChannel As IntPtr, ByVal pSignalStatus As IntPtr) As MW_RESULT
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWGetAudioSignalStatus(ByVal hChannel As IntPtr, ByRef pSignalStatus As MWCAP_AUDIO_SIGNAL_STATUS) As MW_RESULT
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWGetAudioSignalStatus(ByVal hChannel As IntPtr, ByRef pSignalStatus As MWCAP_AUDIO_SIGNAL_STATUS) As MW_RESULT
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWGetDeviceTime(ByVal hChannel As IntPtr, ByRef pllTime As ULong) As MW_RESULT
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWGetDeviceTime(ByVal hChannel As IntPtr, ByRef pllTime As ULong) As MW_RESULT
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWRegisterTimer(ByVal hChannel As IntPtr, ByVal hEvent As IntPtr) As ULong
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWRegisterTimer(ByVal hChannel As IntPtr, ByVal hEvent As IntPtr) As ULong
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWUnregisterTimer(ByVal hChannel As IntPtr, ByVal hTimer As ULong) As MW_RESULT
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWUnregisterTimer(ByVal hChannel As IntPtr, ByVal hTimer As ULong) As MW_RESULT
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWScheduleTimer(ByVal hChannel As IntPtr, ByVal hTimer As ULong, ByVal llExpireTime As ULong) As MW_RESULT
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWScheduleTimer(ByVal hChannel As IntPtr, ByVal hTimer As ULong, ByVal llExpireTime As ULong) As MW_RESULT
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWRegisterNotify(ByVal hChannel As IntPtr, ByVal hEvent As IntPtr, ByVal dwEnableBits As ULong) As ULong
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWRegisterNotify(ByVal hChannel As IntPtr, ByVal hEvent As IntPtr, ByVal dwEnableBits As ULong) As ULong
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWUnregisterNotify(ByVal hChannel As IntPtr, ByVal hNotify As ULong) As MW_RESULT
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWUnregisterNotify(ByVal hChannel As IntPtr, ByVal hNotify As ULong) As MW_RESULT
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWGetNotifyStatus(ByVal hChannel As IntPtr, ByVal hNotify As ULong, ByRef pullStatus As ULong) As MW_RESULT
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWGetNotifyStatus(ByVal hChannel As IntPtr, ByVal hNotify As ULong, ByRef pullStatus As ULong) As MW_RESULT
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWPinVideoBuffer(ByVal hChannel As IntPtr,ByVal pbFrame As IntPtr,ByVal cbFrame As UInteger) As MW_RESULT
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWPinVideoBuffer(ByVal hChannel As IntPtr, ByVal pbFrame As IntPtr, ByVal cbFrame As UInteger) As MW_RESULT
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWUnpinVideoBuffer(ByVal hChannel As IntPtr, ByVal pbFrame As IntPtr) As MW_RESULT
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWUnpinVideoBuffer(ByVal hChannel As IntPtr, ByVal pbFrame As IntPtr) As MW_RESULT
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWStartVideoCapture(ByVal hChannel As IntPtr, ByVal hEvent As IntPtr) As MW_RESULT
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWStartVideoCapture(ByVal hChannel As IntPtr, ByVal hEvent As IntPtr) As MW_RESULT
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWStopVideoCapture(ByVal hChannel As IntPtr) As MW_RESULT
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWStopVideoCapture(ByVal hChannel As IntPtr) As MW_RESULT
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWCaptureVideoFrameToVirtualAddress(ByVal hChannel As IntPtr, ByVal iFrame As Integer, ByVal pbFrame As IntPtr,
                                                                                                    ByVal cbFrame As UInteger, ByVal cbStride As UInteger, ByVal bBottomUp As Byte,
                                                                                                    ByVal pvContext As Long, ByVal dwFOURCC As UInteger, ByVal cx As Integer, ByVal cy As Integer) As MW_RESULT
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWCaptureVideoFrameToVirtualAddress(ByVal hChannel As IntPtr, ByVal iFrame As Integer, ByVal pbFrame As IntPtr,
                                                                                                    ByVal cbFrame As UInteger, ByVal cbStride As UInteger, ByVal bBottomUp As Byte,
                                                                                                    ByVal pvContext As Long, ByVal dwFOURCC As UInteger, ByVal cx As Integer, ByVal cy As Integer) As MW_RESULT
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWCaptureVideoFrameToVirtualAddressEx(ByVal hChannel As IntPtr, ByVal iFrame As Integer, ByVal pbFrame As IntPtr,
                                                                                                    ByVal cbFrame As Integer, ByVal cbStride As Integer, ByVal bBottomUp As Byte,
                                                                                                    ByVal pvContext As IntPtr, ByVal dwFOURCC As UInteger, ByVal cx As Integer, ByVal cy As Integer,
                                                                                                    ByVal dwProcessSwitchs As UInteger, ByVal cyParitalNotify As Integer, ByVal hOSDImage As IntPtr,
                                                                                                    ByVal pOSDRects As IntPtr, ByVal cOSDRects As Integer, ByVal sContrast As Short,
                                                                                                    ByVal sBrightness As Short, ByVal sSaturation As Short, ByVal sHue As Short, ByVal deinterlaceMode As MWCAP_VIDEO_DEINTERLACE_MODE,
                                                                                                    ByVal aspectRatioConvertMode As MWCAP_VIDEO_ASPECT_RATIO_CONVERT_MODE, ByVal pRectSrc As IntPtr, ByVal pRectDest As IntPtr,
                                                                                                    ByVal nAspectX As Integer, ByVal nAspectY As Integer, ByVal colorFormat As MWCAP_VIDEO_COLOR_FORMAT,
                                                                                                    ByVal quantRange As MWCAP_VIDEO_QUANTIZATION_RANGE, ByVal satRange As MWCAP_VIDEO_SATURATION_RANGE) As MW_RESULT
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWCaptureVideoFrameToVirtualAddressEx(ByVal hChannel As IntPtr, ByVal iFrame As Integer, ByVal pbFrame As IntPtr,
                                                                                                    ByVal cbFrame As Integer, ByVal cbStride As Integer, ByVal bBottomUp As Byte,
                                                                                                    ByVal pvContext As IntPtr, ByVal dwFOURCC As UInteger, ByVal cx As Integer, ByVal cy As Integer,
                                                                                                    ByVal dwProcessSwitchs As UInteger, ByVal cyParitalNotify As Integer, ByVal hOSDImage As IntPtr,
                                                                                                    ByVal pOSDRects As IntPtr, ByVal cOSDRects As Integer, ByVal sContrast As Short,
                                                                                                    ByVal sBrightness As Short, ByVal sSaturation As Short, ByVal sHue As Short, ByVal deinterlaceMode As MWCAP_VIDEO_DEINTERLACE_MODE,
                                                                                                    ByVal aspectRatioConvertMode As MWCAP_VIDEO_ASPECT_RATIO_CONVERT_MODE, ByVal pRectSrc As IntPtr, ByVal pRectDest As IntPtr,
                                                                                                    ByVal nAspectX As Integer, ByVal nAspectY As Integer, ByVal colorFormat As MWCAP_VIDEO_COLOR_FORMAT,
                                                                                                    ByVal quantRange As MWCAP_VIDEO_QUANTIZATION_RANGE, ByVal satRange As MWCAP_VIDEO_SATURATION_RANGE) As MW_RESULT
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWGetVideoBufferInfo(ByVal hChannel As IntPtr, ByRef pVideoBufferInfo As MWCAP_VIDEO_BUFFER_INFO) As MW_RESULT
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWGetVideoBufferInfo(ByVal hChannel As IntPtr, ByRef pVideoBufferInfo As MWCAP_VIDEO_BUFFER_INFO) As MW_RESULT
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWGetVideoFrameInfo(ByVal hChannel As IntPtr, ByVal i As Byte, ByRef videoFrameInfo As MWCAP_VIDEO_FRAME_INFO) As MW_RESULT
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWGetVideoFrameInfo(ByVal hChannel As IntPtr, ByVal i As Byte, ByRef videoFrameInfo As MWCAP_VIDEO_FRAME_INFO) As MW_RESULT
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWGetVideoCaptureStatus(ByVal hChannel As IntPtr, ByRef pStatus As MWCAP_VIDEO_CAPTURE_STATUS) As MW_RESULT
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWGetVideoCaptureStatus(ByVal hChannel As IntPtr, ByRef pStatus As MWCAP_VIDEO_CAPTURE_STATUS) As MW_RESULT
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWStartAudioCapture(ByVal hChannel As IntPtr) As MW_RESULT
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWStartAudioCapture(ByVal hChannel As IntPtr) As MW_RESULT
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWStopAudioCapture(ByVal hChannel As IntPtr) As MW_RESULT
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWStopAudioCapture(ByVal hChannel As IntPtr) As MW_RESULT
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWCaptureAudioFrame(ByVal hChannel As IntPtr, ByRef pAudioCaptureFrame As MWCAP_AUDIO_CAPTURE_FRAME) As MW_RESULT
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWCaptureAudioFrame(ByVal hChannel As IntPtr, ByRef pAudioCaptureFrame As MWCAP_AUDIO_CAPTURE_FRAME) As MW_RESULT
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWSetVideoInputAspectRatio(ByVal hChannel As IntPtr, ByVal nAspectX As Integer, ByVal nAspectY As Integer) As MW_RESULT
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWSetVideoInputAspectRatio(ByVal hChannel As IntPtr, ByVal nAspectX As Integer, ByVal nAspectY As Integer) As MW_RESULT
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWGetVideoSignalStatus(ByVal hChannel As IntPtr, ByRef signalStatus As MWCAP_VIDEO_SIGNAL_STATUS) As MW_RESULT
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWGetVideoSignalStatus(ByVal hChannel As IntPtr, ByRef signalStatus As MWCAP_VIDEO_SIGNAL_STATUS) As MW_RESULT
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWGetVideoInputSourceArray(ByVal hChannel As IntPtr, ByVal pdwInputSource As IntPtr, ByRef pdwInputCount As Integer) As MW_RESULT
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWGetVideoInputSourceArray(ByVal hChannel As IntPtr, ByVal pdwInputSource As IntPtr, ByRef pdwInputCount As Integer) As MW_RESULT
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWGetVideoInputSource(ByVal hChannel As IntPtr, ByRef pdwSource As Integer) As MW_RESULT
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWGetVideoInputSource(ByVal hChannel As IntPtr, ByRef pdwSource As Integer) As MW_RESULT
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWGetAudioInputSourceArray(ByVal hChannel As IntPtr, ByVal pdwInputSource As IntPtr, ByRef pdwInputCount As Integer) As MW_RESULT
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWGetAudioInputSourceArray(ByVal hChannel As IntPtr, ByVal pdwInputSource As IntPtr, ByRef pdwInputCount As Integer) As MW_RESULT
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWGetAudioInputSource(ByVal hChannel As IntPtr, ByRef pdwSource As Integer) As MW_RESULT
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWGetAudioInputSource(ByVal hChannel As IntPtr, ByRef pdwSource As Integer) As MW_RESULT
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWGetHDMIInfoFrameValidFlag(ByVal hChannel As IntPtr, ByRef pdwValidFlag As Integer) As MW_RESULT
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWGetHDMIInfoFrameValidFlag(ByVal hChannel As IntPtr, ByRef pdwValidFlag As Integer) As MW_RESULT
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWCaptured.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWGetHDMIInfoFramePacket(ByVal hChannel As IntPtr, ByVal id As MWCAP_HDMI_INFOFRAME_ID, ByRef pPacket As HDMI_INFOFRAME_PACKET) As MW_RESULT
    End Function
#Else
    <DllImport("LibMWCapture.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWGetHDMIInfoFramePacket(ByVal hChannel As IntPtr, ByVal id As MWCAP_HDMI_INFOFRAME_ID, ByRef pPacket As HDMI_INFOFRAME_PACKET) As MW_RESULT
    End Function
#End If

End Class
