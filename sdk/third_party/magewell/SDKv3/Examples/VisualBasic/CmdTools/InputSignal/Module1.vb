Imports LibVBMWCapture.LibMWCapture
Imports System.Runtime.InteropServices
Module Module1
    Dim hChannel As IntPtr
    Dim arrProDevChannel() As Integer
    Dim nChannelCount As Integer
    Sub ShowInfo()
        Dim byMaj As Byte, byMin As Byte
        Dim wBuild As Integer
        MWGetVersion(byMaj, byMin, wBuild)
        Console.Write("Magewell MWCapture SDK {0}.{1}.1.{2} - InputSignalVB" + vbCrLf, byMaj, byMin, wBuild)
        Console.Write("All Devices are supported." + vbCrLf)
        Console.Write("Usage:" + vbCrLf)
        Console.Write("InputSignal.exe <channel index>" + vbCrLf)
        Console.Write("Pro and Eco capture serial devices: InputSignalVB.exe <board id>:<channel id>" + vbCrLf)
    End Sub
    Sub Initialize()
        hChannel = IntPtr.Zero
        nChannelCount = 0

        MWCaptureInitInstance()
    End Sub
    Function ConfigValidChannel() As Boolean
        nChannelCount = MWGetChannelCount()
        If nChannelCount = 0 Then
            Console.Write("ERROR: Can't find channels!" + vbCrLf)
            Return False
        End If
        ReDim arrProDevChannel(nChannelCount)
        Dim info As MWCAP_CHANNEL_INFO
        Dim t_mr As MW_RESULT = MW_RESULT.MW_FAILED
        For i As Integer = 0 To nChannelCount - 1
            t_mr = MWGetChannelInfoByIndex(i, info)
            If t_mr <> MW_RESULT.MW_SUCCEEDED Then
                arrProDevChannel(i) = -1
            Else
                arrProDevChannel(i) = i
            End If
        Next

        If nChannelCount < 2 Then
            Console.Write("Find {0} channel." + vbCrLf, nChannelCount)
        Else
            Console.Write("Find {0} channels." + vbCrLf, nChannelCount)
        End If
        Return True
    End Function
    Function OpenChannel(ByVal cmdargs() As String) As Boolean
        Dim t_n_dev_index As Integer = -1
        For i As Integer = 0 To nChannelCount - 1
            If arrProDevChannel(i) <> -1 Then
                t_n_dev_index = i
                Exit For
            End If
        Next
        If t_n_dev_index = -1 Then
            Console.Write("ERROR: Can't find any device!" + vbCrLf)
            Return False
        End If

        Dim byBoardId As Integer = -1
        Dim byChannelId As Integer = -1
        Dim nDevIndex As Integer = -1
        Dim bIndex As Boolean = False

        Dim videoInfo As New MWCAP_CHANNEL_INFO

        If (cmdargs.Length() = 0) Then
            bIndex = True
            nDevIndex = t_n_dev_index
        Else
            If (cmdargs(0).Contains(":")) Then
                bIndex = False
                If (cmdargs(0).Length() = 3) Then
                    Dim c_array() As Char = cmdargs(0).ToCharArray()
                    Dim byte_1 As Byte = AscW(c_array(0))
                    Dim byte_2 As Byte = AscW(c_array(2))
                    If (((byte_1 >= Asc("0")) And (byte_1 <= Asc("9"))) Or _
                        ((byte_1 >= Asc("a")) And (byte_1 <= Asc("f"))) Or _
                        ((byte_1 >= Asc("A")) And (byte_1 <= Asc("F")))) Then
                        If (((byte_1 >= Asc("0")) And (byte_1 <= Asc("9")))) Then
                            byBoardId = CInt(byte_1) - Asc("0")
                        ElseIf (((byte_1 >= Asc("a")) And (byte_1 <= Asc("f")))) Then
                            byBoardId = CInt(byte_1) - Asc("a") + 10
                        Else
                            byBoardId = CInt(byte_1) - Asc("A") + 10
                        End If
                    Else
                        byBoardId = -1
                    End If
                    If ((byte_2 >= Asc("0")) And (byte_2 <= Asc("3"))) Then
                        byChannelId = CInt(byte_2) - Asc("0")
                    Else
                        byChannelId = -1
                    End If
                Else
                    byBoardId = -1
                    byChannelId = -1
                End If
                If ((byBoardId = -1) Or (byChannelId = -1)) Then
                    Console.Write(vbCrLf + "ERROR: Invalid params!" + vbCrLf)
                    Return False
                End If
            Else
                bIndex = True
                Dim c_array() As Char = cmdargs(0).ToCharArray()
                Dim byte_1 As Byte = AscW(c_array(0))
                If (cmdargs(0).Length() > 2) Then
                    nDevIndex = -1
                ElseIf (cmdargs(0).Length() = 2) Then
                    Dim byte_2 As Byte = AscW(c_array(1))
                    If (((byte_1 >= Asc("0")) And (byte_1 <= Asc("9"))) And ((byte_2 >= Asc("0")) And (byte_2 <= Asc("9")))) Then
                        nDevIndex = CInt(byte_1) * 10 + CInt(byte_2)
                    Else
                        nDevIndex = -1
                    End If
                ElseIf (cmdargs(0).Length() = 1) Then
                    If ((byte_1 >= Asc("0")) And (byte_1 <= Asc("9"))) Then
                        nDevIndex = CInt(byte_1) - Asc("0")
                    Else
                        nDevIndex = -1
                    End If
                End If

                If ((nDevIndex < 0) Or (nDevIndex > nChannelCount)) Then
                    Console.Write(vbCrLf + "ERROR: Invalid params!" + vbCrLf)
                    Return False
                End If
            End If
        End If

        If (bIndex = True) Then
            Dim path(256) As Integer
            Dim path_gchandle As GCHandle = GCHandle.Alloc(path, GCHandleType.Pinned)
            Dim p_path As IntPtr = path_gchandle.AddrOfPinnedObject()
            MWGetDevicePath(arrProDevChannel(nDevIndex), p_path)
            hChannel = MWOpenChannelByPath(p_path)
            path_gchandle.Free()
            If (hChannel = IntPtr.Zero) Then
                Console.Write("ERROR: Open channel {0} error!" + vbCrLf, nDevIndex)
                Return False
            End If
        Else
            hChannel = MWOpenChannel(byBoardId, byChannelId)
            If (hChannel = IntPtr.Zero) Then
                Console.Write("ERROR: Open channel {0:x}:{1:d} error!" + vbCrLf, byBoardId, byChannelId)
                Return False
            End If
        End If

        If (MW_RESULT.MW_SUCCEEDED <> MWGetChannelInfo(hChannel, videoInfo)) Then
            Console.Write("ERROR: Can't get channel info!" + vbCrLf)
            Return False
        End If

        Console.Write("Open channel - BoardIndex = {0:x}, ChannelIndex = {1:d}." + vbCrLf, videoInfo.byBoardIndex, videoInfo.byChannelIndex)
        Console.Write("Product Name:{0}" + vbCrLf, videoInfo.szProductName)
        Console.Write("Board SerialNo:{0}" + vbCrLf + vbCrLf, videoInfo.szBoardSerialNo)

        Return True
    End Function
    Sub PrintInputSignalInfo()
        Dim t_ret As MW_RESULT = MW_RESULT.MW_FAILED
        Dim status As MWCAP_INPUT_SPECIFIC_STATUS
        t_ret = MWGetInputSpecificStatus(hChannel, status)
        If (t_ret = MW_RESULT.MW_SUCCEEDED) Then
            Dim strPre As String
            If status.bValid = 0 Then
                strPre = "Not Present"
            Else
                strPre = "Present"
            End If
            Console.Write("Input Signal Valid:{0}" + vbCrLf, strPre)
            If (status.bValid) Then
                Select Case (status.dwVideoInputType)
                    Case MWCAP_VIDEO_INPUT_TYPE.MWCAP_VIDEO_INPUT_TYPE_NONE
                        Console.Write("---Type: NONE" + vbCrLf)
                    Case MWCAP_VIDEO_INPUT_TYPE.MWCAP_VIDEO_INPUT_TYPE_HDMI
                        Console.Write("---Type: HDMI" + vbCrLf)
                    Case MWCAP_VIDEO_INPUT_TYPE.MWCAP_VIDEO_INPUT_TYPE_VGA
                        Console.Write("---Type: VGA" + vbCrLf)
                    Case MWCAP_VIDEO_INPUT_TYPE.MWCAP_VIDEO_INPUT_TYPE_SDI
                        Console.Write("---Type: SDI" + vbCrLf)
                    Case MWCAP_VIDEO_INPUT_TYPE.MWCAP_VIDEO_INPUT_TYPE_COMPONENT
                        Console.Write("---Type: COMPONENT" + vbCrLf)
                    Case MWCAP_VIDEO_INPUT_TYPE.MWCAP_VIDEO_INPUT_TYPE_CVBS
                        Console.Write("---Type: CVBS" + vbCrLf)
                    Case MWCAP_VIDEO_INPUT_TYPE.MWCAP_VIDEO_INPUT_TYPE_YC
                        Console.Write("---Type: YC" + vbCrLf)
                    Case Else

                End Select
                If (status.dwVideoInputType = MWCAP_VIDEO_INPUT_TYPE.MWCAP_VIDEO_INPUT_TYPE_HDMI) Then
                    Dim strHDCP As String
                    If status.uion.hdmiStatus.bHDCP = 0 Then
                        strHDCP = "No"
                    Else
                        strHDCP = "Yes"
                    End If
                    Console.Write("---HDMI HDCP:{0}" + vbCrLf, strHDCP)
                    Console.Write("---HDMI HDMI Mode:{0}" + vbCrLf, status.uion.hdmiStatus.bHDMIMode)
                    Console.Write("---HDMI Bit Depth:{0}" + vbCrLf, status.uion.hdmiStatus.byBitDepth)
                ElseIf (status.dwVideoInputType = MWCAP_VIDEO_INPUT_TYPE.MWCAP_VIDEO_INPUT_TYPE_SDI) Then
                    Dim strType As String
                    GetVideoSDIType(status.uion.sdiStatus.sdiType, strType)

                    Dim strScanfmt As String
                    GetVideoScanFmt(status.uion.sdiStatus.sdiScanningFormat, strScanfmt)

                    Dim strBitDepth As String
                    GetVideoBitDepth(status.uion.sdiStatus.sdiBitDepth, strBitDepth)

                    Dim strSampleStruct As String
                    GetVideoSamplingStruct(status.uion.sdiStatus.sdiSamplingStruct, strSampleStruct)
                    Console.Write("---SDI Type:{0}" + vbCrLf, strType)
                    Console.Write("---SDI Scanning Format:{0}" + vbCrLf, strScanfmt)
                    Console.Write("---SDI Bit Depth:{0}" + vbCrLf, strBitDepth)
                    Console.Write("---SDI Sampling Struct:{0}" + vbCrLf, strSampleStruct)
                ElseIf (status.dwVideoInputType = MWCAP_VIDEO_INPUT_TYPE.MWCAP_VIDEO_INPUT_TYPE_VGA) Then
                    Dim strSyncType As String
                    GetVideoSyncType(status.uion.vgaComponentStatus.syncInfo.bySyncType, strSyncType)

                    Dim dFrameDuration As Double = 0.0
                    If (status.uion.vgaComponentStatus.syncInfo.bInterlaced) Then
                        dFrameDuration = 20000000 / status.uion.vgaComponentStatus.syncInfo.dwFrameDuration
                    Else
                        dFrameDuration = 10000000 / status.uion.vgaComponentStatus.syncInfo.dwFrameDuration
                    End If
                    Console.Write("---VGA SyncType:{0}" + vbCrLf, strSyncType)
                    Console.Write("---VGA bHSPolarity:{0}" + vbCrLf, status.uion.vgaComponentStatus.syncInfo.bHSPolarity)
                    Console.Write("---VGA bVSPolarity:{0}" + vbCrLf, status.uion.vgaComponentStatus.syncInfo.bVSPolarity)
                    Console.Write("---VGA bInterlaced:{0}" + vbCrLf, status.uion.vgaComponentStatus.syncInfo.bInterlaced)
                    Console.Write("---VGA FrameDuration:{0:f2}" + vbCrLf, dFrameDuration)
                ElseIf (status.dwVideoInputType = MWCAP_VIDEO_INPUT_TYPE.MWCAP_VIDEO_INPUT_TYPE_CVBS) Then
                    Dim strSDStandard As String
                    GetVideoSDStandard(status.uion.cvbsYcStatus.standard, strSDStandard)

                    Console.Write("---CVBS standard:{0}" + vbCrLf, strSDStandard)
                    Console.Write("---CVBS b50Hz:{0}" + vbCrLf, status.uion.cvbsYcStatus.b50Hz)
                End If
            End If
        End If

        Console.Write(vbCrLf)

        Dim vStatus As MWCAP_VIDEO_SIGNAL_STATUS
        t_ret = MWGetVideoSignalStatus(hChannel, vStatus)
        If (t_ret = MW_RESULT.MW_SUCCEEDED) Then
            Select Case (vStatus.state)
                Case MWCAP_VIDEO_SIGNAL_STATE.MWCAP_VIDEO_SIGNAL_LOCKED
                    Console.Write("Video Signal status: LOCKED" + vbCrLf)
                Case MWCAP_VIDEO_SIGNAL_STATE.MWCAP_VIDEO_SIGNAL_LOCKING
                    Console.Write("Video Signal status: LOCKING" + vbCrLf)
                Case MWCAP_VIDEO_SIGNAL_STATE.MWCAP_VIDEO_SIGNAL_UNSUPPORTED
                    Console.Write("Video Signal status: UNSUPPORTED" + vbCrLf)
                Case MWCAP_VIDEO_SIGNAL_STATE.MWCAP_VIDEO_SIGNAL_NONE
                    Console.Write("Video Signal status: Not present" + vbCrLf)
                Case Else

            End Select
            If (vStatus.state = MWCAP_VIDEO_SIGNAL_STATE.MWCAP_VIDEO_SIGNAL_LOCKED) Then
                Dim strColorName As String
                GetVideoColorName(vStatus.colorFormat, strColorName)

                Dim dFrameDuration As Double = 0.0
                Dim strYN As String
                If (vStatus.bInterlaced) Then
                    dFrameDuration = 20000000 / (vStatus.dwFrameDuration)
                    strYN = "Yes"
                Else
                    dFrameDuration = 10000000 / (vStatus.dwFrameDuration)
                    strYN = "No"
                End If

                Console.Write("Video Signal:" + vbCrLf)
                Console.Write("---x, y: ({0}, {1})" + vbCrLf, vStatus.x, vStatus.y)
                Console.Write("---cx x cy: ({0} x {1})" + vbCrLf, vStatus.cx, vStatus.cy)
                Console.Write("---cxTotal x cyTotal: ({0} x {1})" + vbCrLf, vStatus.cxTotal, vStatus.cyTotal)
                Console.Write("---bInterlaced: {0}" + vbCrLf, strYN)
                Console.Write("---dwFrameRate: {0:f2}" + vbCrLf, dFrameDuration)
                Console.Write("---nAspectX: {0}" + vbCrLf, vStatus.nAspectX)
                Console.Write("---nAspectY: {0}" + vbCrLf, vStatus.nAspectY)
                If vStatus.bSegmentedFrame Then
                    strYN = "Yes"
                Else
                    strYN = "No"
                End If
                Console.Write("---bSegmentedFrame: {0}" + vbCrLf, strYN)
                Console.Write("---colorFormat: {0}" + vbCrLf, strColorName)
            End If

            Select Case (vStatus.satRange)
                Case MWCAP_VIDEO_SATURATION_RANGE.MWCAP_VIDEO_SATURATION_UNKNOWN
                    Console.Write("---saturationRange : {0}" + vbCrLf, "MWCAP_VIDEO_SATURATION_UNKNOWN")
                Case MWCAP_VIDEO_SATURATION_RANGE.MWCAP_VIDEO_SATURATION_FULL
                    Console.Write("---saturationRange : {0}" + vbCrLf, "MWCAP_VIDEO_SATURATION_FULL")
                Case MWCAP_VIDEO_SATURATION_RANGE.MWCAP_VIDEO_SATURATION_LIMITED
                    Console.Write("---saturationRange : {0}" + vbCrLf, "MWCAP_VIDEO_SATURATION_LIMITED")
                Case MWCAP_VIDEO_SATURATION_RANGE.MWCAP_VIDEO_SATURATION_EXTENDED_GAMUT
                    Console.Write("---saturationRange : {0}" + vbCrLf, "MWCAP_VIDEO_SATURATION_EXTENDED_GAMUT")
                Case Else
                    Console.Write("---saturationRange : {0}" + vbCrLf, "MWCAP_VIDEO_SATURATION_UNKNOWN")
            End Select

            Select Case (vStatus.satRange)
                Case MWCAP_VIDEO_QUANTIZATION_RANGE.MWCAP_VIDEO_QUANTIZATION_UNKNOWN
                    Console.Write("---quantizationRange : {0}" + vbCrLf, "MWCAP_VIDEO_QUANTIZATION_UNKNOWN")
                Case MWCAP_VIDEO_QUANTIZATION_RANGE.MWCAP_VIDEO_QUANTIZATION_FULL
                    Console.Write("---quantizationRange : {0}" + vbCrLf, "MWCAP_VIDEO_QUANTIZATION_FULL")
                Case MWCAP_VIDEO_QUANTIZATION_RANGE.MWCAP_VIDEO_QUANTIZATION_LIMITED
                    Console.Write("---quantizationRange : {0}" + vbCrLf, "MWCAP_VIDEO_QUANTIZATION_LIMITED")
                Case Else
                    Console.Write("---quantizationRange :{0}" + vbCrLf, "MWCAP_VIDEO_QUANTIZATION_UNKNOWN")
            End Select
        End If

        Console.Write(vbCrLf)

        Dim aStatus As MWCAP_AUDIO_SIGNAL_STATUS
        t_ret = MWGetAudioSignalStatus(hChannel, aStatus)
        If (t_ret = MW_RESULT.MW_SUCCEEDED) Then
            Console.Write("Audio Signal Valid:{0}" + vbCrLf, aStatus.bChannelStatusValid)
            If (aStatus.bChannelStatusValid = 1) Then
                Dim strSupChannels As String
                For i As Integer = 0 To 3 Step 1
                    If ((aStatus.wChannelValid) And (&H1& << i)) Then
                        strSupChannels += CStr(i * 2 + 1)
                        strSupChannels += "&"
                        strSupChannels += CStr(i * 2 + 2)
                        strSupChannels += ";"
                    End If
                Next
                Dim strYN As String
                If aStatus.bLPCM Then
                    strYN = "Yes"
                Else
                    strYN = "No"
                End If
                Console.Write("Audio Signal:" + vbCrLf)
                Console.Write("---wChannelValid: {0}" + vbCrLf, strSupChannels)
                Console.Write("---bLPCM: {0}" + vbCrLf, strYN)
                Console.Write("---cBitsPerSample: {0}" + vbCrLf, aStatus.cBitsPerSample)
                Console.Write("---dwSampleRate: {0}" + vbCrLf, aStatus.dwSampleRate)
            End If
        Else
            Console.Write("Can't get audio signal status." + vbCrLf)
        End If
    End Sub
    Public Sub GetVideoSDIType(ByVal type As SDI_TYPE, ByRef pTypeName As String)
        Select Case (type)
            Case SDI_TYPE.SDI_TYPE_SD
                pTypeName = "SD"
            Case SDI_TYPE.SDI_TYPE_HD
                pTypeName = "HD"
            Case SDI_TYPE.SDI_TYPE_3GA
                pTypeName = "3GA"
            Case SDI_TYPE.SDI_TYPE_3GB_DL
                pTypeName = "3GB_DL"
            Case SDI_TYPE.SDI_TYPE_3GB_DS
                pTypeName = "3GB_DS"
            Case SDI_TYPE.SDI_TYPE_DL_CH1
                pTypeName = "DL_CH1"
            Case SDI_TYPE.SDI_TYPE_DL_CH2
                pTypeName = "DL_CH2"
            Case SDI_TYPE.SDI_TYPE_6G_MODE1
                pTypeName = "6G_MODE1"
            Case SDI_TYPE.SDI_TYPE_6G_MODE2
                pTypeName = "6G_MODE2"
            Case Else
                pTypeName = "Unknown"
        End Select
    End Sub

    Public Sub GetVideoScanFmt(ByVal type As SDI_SCANNING_FORMAT, ByRef pFmtName As String)
        Select Case (type)
            Case SDI_SCANNING_FORMAT.SDI_SCANING_INTERLACED
                pFmtName = "INTERLACED"
            Case SDI_SCANNING_FORMAT.SDI_SCANING_SEGMENTED_FRAME
                pFmtName = "SEGMENTED_FRAME"
            Case SDI_SCANNING_FORMAT.SDI_SCANING_PROGRESSIVE
                pFmtName = "SEGMENTED_PROGRESSIVE"
            Case Else
                pFmtName = "Unknown"
        End Select
    End Sub

    Public Sub GetVideoBitDepth(ByVal type As SDI_BIT_DEPTH, ByRef pTypeName As String)
        Select Case (type)
            Case SDI_BIT_DEPTH.SDI_BIT_DEPTH_8BIT
                pTypeName = "8bit"
            Case SDI_BIT_DEPTH.SDI_BIT_DEPTH_10BIT
                pTypeName = "10bit"
            Case SDI_BIT_DEPTH.SDI_BIT_DEPTH_12BIT
                pTypeName = "12bit"
            Case Else
                pTypeName = "Unknown"
        End Select
    End Sub

    Public Sub GetVideoSamplingStruct(ByVal type As SDI_SAMPLING_STRUCT, ByRef pStructName As String)
        Select Case (type)
            Case SDI_SAMPLING_STRUCT.SDI_SAMPLING_422_YCbCr
                pStructName = "422_YCbCr"
            Case SDI_SAMPLING_STRUCT.SDI_SAMPLING_444_YCbCr
                pStructName = "444_YCbCr"
            Case SDI_SAMPLING_STRUCT.SDI_SAMPLING_444_RGB
                pStructName = "444_RGB"
            Case SDI_SAMPLING_STRUCT.SDI_SAMPLING_420_YCbCr
                pStructName = "420_YCbCr"
            Case SDI_SAMPLING_STRUCT.SDI_SAMPLING_4224_YCbCrA
                pStructName = "4224_YCbCrA"
            Case SDI_SAMPLING_STRUCT.SDI_SAMPLING_4444_YCbCrA
                pStructName = "4444_YCbCrA"
            Case SDI_SAMPLING_STRUCT.SDI_SAMPLING_4444_RGBA
                pStructName = "4444_RGBA"
            Case SDI_SAMPLING_STRUCT.SDI_SAMPLING_4224_YCbCrD
                pStructName = "4224_YCbCrD"
            Case SDI_SAMPLING_STRUCT.SDI_SAMPLING_4444_YCbCrD
                pStructName = "4444_YCbCrD"
            Case SDI_SAMPLING_STRUCT.SDI_SAMPLING_4444_RGBD
                pStructName = "4444_RGBD"
            Case SDI_SAMPLING_STRUCT.SDI_SAMPLING_444_XYZ
                pStructName = "444_XYZ"
            Case Else
                pStructName = "Unknown"
        End Select
    End Sub

    Public Sub GetVideoSyncType(ByVal type As MWCAP_VIDEO_SYNC_TYPE, ByRef pTypeName As String)
        Select Case (type)
            Case MWCAP_VIDEO_SYNC_TYPE.VIDEO_SYNC_ALL
                pTypeName = "ALL"
            Case MWCAP_VIDEO_SYNC_TYPE.VIDEO_SYNC_HS_VS
                pTypeName = "HS_VS"
            Case MWCAP_VIDEO_SYNC_TYPE.VIDEO_SYNC_CS
                pTypeName = "CS"
            Case MWCAP_VIDEO_SYNC_TYPE.VIDEO_SYNC_EMBEDDED
                pTypeName = "EMBEDDED"
            Case Else
                pTypeName = "Unknown"
        End Select
    End Sub

    Public Sub GetVideoSDStandard(ByVal type As MWCAP_SD_VIDEO_STANDARD, ByRef pTypeName As String)
        Select Case (type)
            Case MWCAP_SD_VIDEO_STANDARD.MWCAP_SD_VIDEO_NONE
                pTypeName = "NONE"
            Case MWCAP_SD_VIDEO_STANDARD.MWCAP_SD_VIDEO_NTSC_M
                pTypeName = "NTSC_M"
            Case MWCAP_SD_VIDEO_STANDARD.MWCAP_SD_VIDEO_NTSC_433
                pTypeName = "NTSC_433"
            Case MWCAP_SD_VIDEO_STANDARD.MWCAP_SD_VIDEO_PAL_M
                pTypeName = "PAL_M"
            Case MWCAP_SD_VIDEO_STANDARD.MWCAP_SD_VIDEO_PAL_60
                pTypeName = "PAL_60"
            Case MWCAP_SD_VIDEO_STANDARD.MWCAP_SD_VIDEO_PAL_COMBN
                pTypeName = "PAL_COMBN"
            Case MWCAP_SD_VIDEO_STANDARD.MWCAP_SD_VIDEO_PAL_BGHID
                pTypeName = "PAL_BGHID"
            Case MWCAP_SD_VIDEO_STANDARD.MWCAP_SD_VIDEO_SECAM
                pTypeName = "SECAM"
            Case MWCAP_SD_VIDEO_STANDARD.MWCAP_SD_VIDEO_SECAM_60
                pTypeName = "SECAM_60"
            Case Else
                pTypeName = "Unknown"
        End Select
    End Sub

    Public Sub GetVideoColorName(ByVal color As MWCAP_VIDEO_COLOR_FORMAT, ByRef pszName As String)
        Select Case (color)
            Case MWCAP_VIDEO_COLOR_FORMAT.MWCAP_VIDEO_COLOR_FORMAT_UNKNOWN
                pszName = "Unknown"
            Case MWCAP_VIDEO_COLOR_FORMAT.MWCAP_VIDEO_COLOR_FORMAT_RGB
                pszName = "RGB"
            Case MWCAP_VIDEO_COLOR_FORMAT.MWCAP_VIDEO_COLOR_FORMAT_YUV601
                pszName = "YUV BT.601"
            Case MWCAP_VIDEO_COLOR_FORMAT.MWCAP_VIDEO_COLOR_FORMAT_YUV709
                pszName = "YUV BT.709"
            Case MWCAP_VIDEO_COLOR_FORMAT.MWCAP_VIDEO_COLOR_FORMAT_YUV2020
                pszName = "YUV BT.2020"
            Case MWCAP_VIDEO_COLOR_FORMAT.MWCAP_VIDEO_COLOR_FORMAT_YUV2020C
                pszName = "YUV BT.2020C"
            Case Else
                pszName = "Unknown"
        End Select
    End Sub
    Sub CloseDevice()
        If (hChannel <> IntPtr.Zero) Then
            MWCloseChannel(hChannel)
            hChannel = IntPtr.Zero
        End If

        MWCaptureExitInstance()
    End Sub
    Sub ExitProgram()
        Console.Write("Press ""Enter"" key to exit..." + vbCrLf)
        Console.ReadKey()
    End Sub
    Sub Main(ByVal cmdargs() As String)
        ShowInfo()

        Initialize()

        If ConfigValidChannel() <> True Then
            ExitProgram()
            MWCaptureExitInstance()
            Return
        End If

        If OpenChannel(cmdargs) <> True Then
            ExitProgram()
            MWCaptureExitInstance()
            Return
        End If

        PrintInputSignalInfo()

        CloseDevice()

        ExitProgram()

    End Sub

End Module
