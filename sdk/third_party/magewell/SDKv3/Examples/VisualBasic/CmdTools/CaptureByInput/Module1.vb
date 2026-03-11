Imports LibVBMWCapture.LibMWCapture
Imports LibVBMWCapture.MWcap_FOURCC
Imports LibVBKernel32.LibKernel32
Imports System.Runtime.InteropServices
Imports System
Imports System.IO
Imports System.Drawing
Imports System.Drawing.Imaging

Module Module1

    Dim num_capture As Integer = 100
    Dim hChannel As IntPtr
    Dim arrProDevChannel(10) As Integer
    Dim xr As MW_RESULT
    Dim videoSignalStatus As MWCAP_VIDEO_SIGNAL_STATUS
    Dim nChannelCount As Integer
    Dim hNotifyEvent As IntPtr
    Dim hCaptureEvent As IntPtr

    Private Declare Function PathFileExists Lib "shlwapi.dll" Alias "PathFileExistsA" (ByVal pszPath As String) As Long
    Sub ShowInfo()
        Dim byMaj As Byte, byMin As Byte
        Dim wBuild As Integer
        MWGetVersion(byMaj, byMin, wBuild)
        Console.Write("Magewell MWCapture SDK {0}.{1}.1.{2} - CaptureByInputVB" + vbCrLf, byMaj, byMin, wBuild)
        Console.Write("USB and Eco Devices are not supported" + vbCrLf)
        Console.Write("Usage:" + vbCrLf)
        Console.Write("CaptureByInputVB.exe <channel index>" + vbCrLf)
        Console.Write("CaptureByInputVB.exe <board id>:<channel id>" + vbCrLf)
    End Sub
    Sub Initialize()
        hChannel = IntPtr.Zero
        nChannelCount = 0

        MWCaptureInitInstance()
    End Sub
    Function WriteFilePermissionTest() As Boolean
        Dim bret As Boolean = True

        ''test file write persion
        Dim homedrive As String
        Dim homepath As String
        Dim foldername As String
        foldername = "\Magewell"
        Dim folderpath As String
        homedrive = Environ("HOMEDRIVE")
        homepath = Environ("HOMEPATH")
        folderpath = homedrive + homepath + foldername
        Dim lret As Long
        lret = PathFileExists(folderpath)
        If lret = 0 Then
            MkDir(folderpath)
        End If
        Dim filename = "\temp.bmp"
        Dim filePath As String
        filePath = folderpath + filename
        Dim fs As New FileStream(filePath, FileMode.Create, FileAccess.ReadWrite)
        lret = PathFileExists(filePath)
        If lret = False Then
            Console.Write("Please restart the application with administrator permission." + vbCrLf)
            Console.Write("Press ENTER to exit..." + vbCrLf)
            Console.ReadKey()
            bret = False
        Else
            fs.Close()
            LibVBKernel32.LibKernel32.DeleteFileA(filePath)
            bret = True
        End If
        Return bret
    End Function
    Function ConfigValidChannel() As Boolean
        nChannelCount = MWGetChannelCount()
        If nChannelCount = 0 Then
            Console.Write("ERROR: Can't find channels!" + vbCrLf)
            Return False
        End If

        ReDim arrProDevChannel(nChannelCount)
        Dim info As MWCAP_CHANNEL_INFO
        Dim mr As MW_RESULT
        For i As Integer = 0 To nChannelCount - 1
            arrProDevChannel(i) = -1
            mr = MWGetChannelInfoByIndex(i, info)
            If mr = MW_RESULT.MW_SUCCEEDED Then
                If StrComp(info.szFamilyName, "Pro Capture") = 0 Then
                    arrProDevChannel(i) = i
                End If
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
        Dim t_n_pro_index As Integer = -1
        For i As Integer = 0 To nChannelCount
            If arrProDevChannel(i) <> -1 Then
                t_n_pro_index = arrProDevChannel(i)
                Exit For
            End If
        Next

        If t_n_pro_index = -1 Then
            Console.Write("ERROR: Can't find Pro Capture!" + vbCrLf)
            Return False
        End If

        Dim byBoardId As Integer = -1
        Dim byChannelId As Integer = -1
        Dim nDeviceIndex As Integer = -1
        Dim bIndex As Boolean = False

        Dim channelInfo As New MWCAP_CHANNEL_INFO
        If (cmdargs.Length() = 0) Then
            bIndex = True
            nDeviceIndex = t_n_pro_index
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
                    nDeviceIndex = -1
                ElseIf (cmdargs(0).Length() = 2) Then
                    Dim byte_2 As Byte = AscW(c_array(1))
                    If (((byte_1 >= Asc("0")) And (byte_1 <= Asc("9"))) And ((byte_2 >= Asc("0")) And (byte_2 <= Asc("9")))) Then
                        nDeviceIndex = CInt(byte_1) * 10 + CInt(byte_2)
                    Else
                        nDeviceIndex = -1
                    End If
                ElseIf (cmdargs(0).Length() = 1) Then
                    If ((byte_1 >= Asc("0")) And (byte_1 <= Asc("9"))) Then
                        nDeviceIndex = CInt(byte_1) - Asc("0")
                    Else
                        nDeviceIndex = -1
                    End If
                End If

                If ((nDeviceIndex < 0) Or (nDeviceIndex > nChannelCount)) Then
                    Console.Write(vbCrLf + "ERROR: Invalid params!" + vbCrLf)
                    Return False
                End If
            End If
        End If

        If (bIndex = True) Then
            Dim path(256) As Integer
            Dim path_gchandle As GCHandle = GCHandle.Alloc(path, GCHandleType.Pinned)
            Dim p_path As IntPtr = path_gchandle.AddrOfPinnedObject()
            MWGetDevicePath(nDeviceIndex, p_path)
            hChannel = MWOpenChannelByPath(p_path)
            path_gchandle.Free()
            If (hChannel = IntPtr.Zero) Then
                Console.Write("ERROR: Open channel {0} error!" + vbCrLf, nDeviceIndex)
                Return False
            End If
        Else
            hChannel = MWOpenChannel(byBoardId, byChannelId)
            If (hChannel = IntPtr.Zero) Then
                Console.Write("ERROR: Open channel {0:x}:{1} error!" + vbCrLf, byBoardId, byChannelId)
                Return False
            End If
        End If

        If (MW_RESULT.MW_SUCCEEDED <> MWGetChannelInfo(hChannel, channelInfo)) Then
            Console.Write("ERROR: Can't get channel info!" + vbCrLf)
            Return False
        End If

        Console.Write("Open channel - BoardIndex = {0:x}, ChannelIndex = {1}." + vbCrLf, channelInfo.byBoardIndex, channelInfo.byChannelIndex)
        Console.Write("Product Name:{0}" + vbCrLf, channelInfo.szProductName)
        Console.Write("Board SerialNo:{0}" + vbCrLf + vbCrLf, channelInfo.szBoardSerialNo)

        Return True
    End Function
    Function CheckSignal() As Boolean
        MWGetVideoSignalStatus(hChannel, videoSignalStatus)

        Select Case (videoSignalStatus.state)
            Case MWCAP_VIDEO_SIGNAL_STATE.MWCAP_VIDEO_SIGNAL_NONE
                Console.Write("Input signal status: NONE" + vbCrLf)
            Case MWCAP_VIDEO_SIGNAL_STATE.MWCAP_VIDEO_SIGNAL_UNSUPPORTED
                Console.Write("Input signal status: Unsupported" + vbCrLf)
            Case MWCAP_VIDEO_SIGNAL_STATE.MWCAP_VIDEO_SIGNAL_LOCKING
                Console.Write("Input signal status: Locking" + vbCrLf)
            Case MWCAP_VIDEO_SIGNAL_STATE.MWCAP_VIDEO_SIGNAL_LOCKED
                Console.Write("Input signal status: Locked" + vbCrLf)
        End Select

        If (videoSignalStatus.state = MWCAP_VIDEO_SIGNAL_STATE.MWCAP_VIDEO_SIGNAL_LOCKED) Then
            Console.Write("Input signal resolution: {0} x {1}" + vbCrLf, videoSignalStatus.cx, videoSignalStatus.cy)
            Dim fps As Double
            If (videoSignalStatus.bInterlaced = True) Then
                fps = 20000000 / videoSignalStatus.dwFrameDuration
            Else
                fps = 10000000 / videoSignalStatus.dwFrameDuration
            End If

            Console.Write("Input signal fps: {0:f2}" + vbCrLf, fps)
            Console.Write("Input signal interlaced: {0}" + vbCrLf, videoSignalStatus.bInterlaced)
            Console.Write("Input signal frame segmented: {0}" + vbCrLf, videoSignalStatus.bSegmentedFrame)

            Return True
        End If
        Return False
    End Function
    Sub StartCapture()
        If videoSignalStatus.bInterlaced = 1 Then
            CaptureIFrame()
        Else
            CapturePFrame()
        End If
    End Sub
    Sub CapturePFrame()
        hNotifyEvent = CreateEvent(IntPtr.Zero, False, False, IntPtr.Zero)
        hCaptureEvent = CreateEvent(IntPtr.Zero, False, False, IntPtr.Zero)
        Dim videoBufferInfo As MWCAP_VIDEO_BUFFER_INFO
        Dim videoFrameInfo As New MWCAP_VIDEO_FRAME_INFO

        xr = MWStartVideoCapture(hChannel, hCaptureEvent)
        If (xr <> MW_RESULT.MW_SUCCEEDED) Then
            CloseHandle(hNotifyEvent)
            CloseHandle(hCaptureEvent)
            Console.Write("ERROR: Open Video Capture error!" + vbCrLf)
        Else

            MWGetVideoBufferInfo(hChannel, videoBufferInfo)

            'MWGetVideoFrameInfo(hChannel, videoBufferInfo.iNewestBufferedFullFrame, videoFrameInfo)

            If (videoSignalStatus.state = MWCAP_VIDEO_SIGNAL_STATE.MWCAP_VIDEO_SIGNAL_LOCKED) Then
                Dim cx As Integer = videoSignalStatus.cx
                Dim cy As Integer = videoSignalStatus.cy

                Dim bitmap As New Bitmap(cx, cy, Imaging.PixelFormat.Format24bppRgb)
                Dim rect As New Rectangle(0, 0, cx, cy)
                Dim bitmapData As New BitmapData
                bitmap.LockBits(rect, Imaging.ImageLockMode.WriteOnly, Imaging.PixelFormat.Format24bppRgb, bitmapData)

                Dim bBottomUp As Boolean = False
                If (bitmapData.Stride < 0) Then
                    bitmapData.Scan0 = bitmapData.Scan0 + bitmapData.Stride * (cy - 1)
                    bitmapData.Stride = -bitmapData.Stride
                    bBottomUp = True
                End If

                Dim hNotify As ULong = MWRegisterNotify(hChannel, hNotifyEvent, MWCAP_NOTIFY_VIDEO_FRAME_BUFFERED)
                Dim size As Integer = Marshal.SizeOf(hNotify)
                If (hNotify = 0) Then
                    Console.Write("ERROR: Register Notify error." + vbCrLf)
                Else
                    Dim fps As Double = 10000000 / videoSignalStatus.dwFrameDuration
                    Console.Write("Begin to capture {0} frames by {1:f2} fps..." + vbCrLf, num_capture, fps)

                    MWPinVideoBuffer(hChannel, bitmapData.Scan0, bitmapData.Stride * cy)


                    Dim llTotalTime As Long = 0
                    For i As Integer = 0 To num_capture - 1 Step 1

                        WaitForSingleObject(hNotifyEvent, 1000)

                        Dim ullStatusBits As ULong = MWCAP_NOTIFY_VIDEO_FRAME_BUFFERED
                        size = Marshal.SizeOf(ullStatusBits)
                        xr = MWGetNotifyStatus(hChannel, hNotify, ullStatusBits)
                        If (xr <> MW_RESULT.MW_SUCCEEDED) Then
                            Continue For
                        End If

                        xr = MWGetVideoBufferInfo(hChannel, videoBufferInfo)
                        If (xr <> MW_RESULT.MW_SUCCEEDED) Then
                            Continue For
                        End If

                        xr = MWGetVideoFrameInfo(hChannel, videoBufferInfo.iNewestBufferedFullFrame, videoFrameInfo)
                        If (xr <> MW_RESULT.MW_SUCCEEDED) Then
                            Continue For
                        End If

                        If (ullStatusBits And MWCAP_NOTIFY_VIDEO_FRAME_BUFFERED) Then
                            Dim fourcc As New LibVBMWCapture.MWcap_FOURCC
                            xr = MWCaptureVideoFrameToVirtualAddress(hChannel, videoBufferInfo.iNewestBufferedFullFrame, _
                                                                     bitmapData.Scan0, bitmapData.Stride * cy, bitmapData.Stride, _
                                                                     bBottomUp, IntPtr.Zero, fourcc.MWCAP_FOURCC_BGR24, cx, cy)

                            WaitForSingleObject(hCaptureEvent, 1000)

                            Dim captureStatus As MWCAP_VIDEO_CAPTURE_STATUS
                            xr = MWGetVideoCaptureStatus(hChannel, captureStatus)

                            Dim llCurrent As Long = 0
                            xr = MWGetDeviceTime(hChannel, llCurrent)

                            Dim t_time As Long = 0
                            If (videoSignalStatus.bInterlaced) Then
                                t_time = videoFrameInfo.allFieldBufferedTimes(1)
                            Else
                                t_time = videoFrameInfo.allFieldBufferedTimes(0)
                            End If
                            llTotalTime += (llCurrent - t_time)

                        End If

                    Next

                    Console.Write("End capture." + vbCrLf)
                    Dim t_duration As Integer = llTotalTime / (num_capture * 10000)
                    Console.Write("Each frame average capture duration is {0} ms." + vbCrLf, t_duration)

                    MWUnpinVideoBuffer(hChannel, bitmapData.Scan0)

                    xr = MWUnregisterNotify(hChannel, hNotify)

                    xr = MWStopVideoCapture(hChannel)

                    bitmap.UnlockBits(bitmapData)

                    Dim pngClsid As System.Drawing.Imaging.ImageCodecInfo
                    pngClsid = GetEncoderInfo("image/bmp")
                    Dim encoder As Encoder
                    encoder = System.Drawing.Imaging.Encoder.Quality
                    Dim param As New System.Drawing.Imaging.EncoderParameters
                    Dim encode_param As System.Drawing.Imaging.EncoderParameter
                    encode_param = New EncoderParameter(encoder, CType(50L, Int32))
                    param.Param(0) = encode_param

                    Dim homedrive As String
                    Dim homepath As String
                    Dim foldername As String
                    foldername = "\Magewell"
                    Dim folderpath As String
                    homedrive = Environ("HOMEDRIVE")
                    homepath = Environ("HOMEPATH")
                    folderpath = homedrive + homepath + foldername
                    Dim lret As Long
                    lret = PathFileExists(folderpath)
                    If lret = 0 Then
                        MkDir(folderpath)
                    End If
                    Dim filename = "\CaptureByInput.bmp"
                    Dim filePath As String
                    filePath = folderpath + filename

                    bitmap.Save(filePath, pngClsid, param)
                    Console.Write(vbCrLf + "The last frame is saved in {0}." + vbCrLf + vbCrLf, filePath)

                End If

            End If

        End If
        CloseHandle(hNotifyEvent)
        CloseHandle(hCaptureEvent)

        MWStopVideoCapture(hChannel)
    End Sub
    Sub CaptureIFrame()
        Dim hNotifyEvent As IntPtr = CreateEvent(IntPtr.Zero, False, False, IntPtr.Zero)
        Dim hCaptureEvent As IntPtr = CreateEvent(IntPtr.Zero, False, False, IntPtr.Zero)

        xr = MWStartVideoCapture(hChannel, hCaptureEvent)
        If (xr <> MW_RESULT.MW_SUCCEEDED) Then
            CloseHandle(hNotifyEvent)
            CloseHandle(hCaptureEvent)
            Console.Write("ERROR: Open Video Capture error!" + vbCrLf)
        Else
            Dim videoBufferInfo As MWCAP_VIDEO_BUFFER_INFO
            MWGetVideoBufferInfo(hChannel, videoBufferInfo)

            Dim videoFrameInfo As MWCAP_VIDEO_FRAME_INFO
            MWGetVideoFrameInfo(hChannel, videoBufferInfo.iNewestBufferedFullFrame, videoFrameInfo)

            If (videoSignalStatus.state = MWCAP_VIDEO_SIGNAL_STATE.MWCAP_VIDEO_SIGNAL_LOCKED) Then
                Dim cx As Integer = videoSignalStatus.cx
                Dim cy As Integer = videoSignalStatus.cy

                Dim bitmap As New Bitmap(cx, cy, Imaging.PixelFormat.Format24bppRgb)
                Dim rect As New Rectangle(0, 0, cx, cy)
                Dim bitmapData As New BitmapData
                bitmap.LockBits(rect, Imaging.ImageLockMode.WriteOnly, Imaging.PixelFormat.Format24bppRgb, bitmapData)

                Dim bBottomUp As Boolean = False
                If (bitmapData.Stride < 0) Then
                    bitmapData.Scan0 = bitmapData.Scan0 + bitmapData.Stride * (cy - 1)
                    bitmapData.Stride = -bitmapData.Stride
                    bBottomUp = True
                End If

                Dim hNotify As ULong = MWRegisterNotify(hChannel, hNotifyEvent, MWCAP_NOTIFY_VIDEO_FIELD_BUFFERED)
                Dim size As Integer = Marshal.SizeOf(hNotify)
                If (hNotify = 0) Then
                    Console.Write("ERROR: Register Notify error." + vbCrLf)
                Else
                    Dim fps As Double = 20000000 / videoSignalStatus.dwFrameDuration
                    Console.Write("Begin to capture {0} frames by {1:f2} fps..." + vbCrLf, num_capture, fps)

                    MWPinVideoBuffer(hChannel, bitmapData.Scan0, bitmapData.Stride * cy)

                    Dim llTotalTime As Long = 0
                    For i As Integer = 0 To num_capture - 1 Step 1

                        WaitForSingleObject(hNotifyEvent, 1000)

                        Dim ullStatusBits As ULong = MWCAP_NOTIFY_VIDEO_FRAME_BUFFERED
                        size = Marshal.SizeOf(ullStatusBits)
                        xr = MWGetNotifyStatus(hChannel, hNotify, ullStatusBits)
                        If (xr <> MW_RESULT.MW_SUCCEEDED) Then
                            Continue For
                        End If

                        xr = MWGetVideoBufferInfo(hChannel, videoBufferInfo)
                        If (xr <> MW_RESULT.MW_SUCCEEDED) Then
                            Continue For
                        End If

                        xr = MWGetVideoFrameInfo(hChannel, videoBufferInfo.iNewestBufferedFullFrame, videoFrameInfo)
                        If (xr <> MW_RESULT.MW_SUCCEEDED) Then
                            Continue For
                        End If

                        If (ullStatusBits And MWCAP_NOTIFY_VIDEO_FIELD_BUFFERED) Then
                            Dim fourcc As New LibVBMWCapture.MWcap_FOURCC
                            Dim mode As MWCAP_VIDEO_DEINTERLACE_MODE
                            If videoBufferInfo.iBufferedFieldIndex = 0 Then
                                mode = MWCAP_VIDEO_DEINTERLACE_MODE.MWCAP_VIDEO_DEINTERLACE_TOP_FIELD
                            Else
                                mode = MWCAP_VIDEO_DEINTERLACE_MODE.MWCAP_VIDEO_DEINTERLACE_BOTTOM_FIELD
                            End If
                            xr = MWCaptureVideoFrameToVirtualAddressEx(hChannel, videoBufferInfo.iBufferedFieldIndex, _
                                                                     bitmapData.Scan0, bitmapData.Stride * cy, bitmapData.Stride, _
                                                                     bBottomUp, IntPtr.Zero, fourcc.MWCAP_FOURCC_BGR24, cx, cy,
                                                                     0, 0, IntPtr.Zero, IntPtr.Zero, 0, 100, 0, 100, 0, mode, MWCAP_VIDEO_ASPECT_RATIO_CONVERT_MODE.MWCAP_VIDEO_ASPECT_RATIO_IGNORE,
                                                                     IntPtr.Zero, IntPtr.Zero, 0, 0, MWCAP_VIDEO_COLOR_FORMAT.MWCAP_VIDEO_COLOR_FORMAT_UNKNOWN,
                                                                     MWCAP_VIDEO_QUANTIZATION_RANGE.MWCAP_VIDEO_QUANTIZATION_UNKNOWN, MWCAP_VIDEO_SATURATION_RANGE.MWCAP_VIDEO_SATURATION_UNKNOWN)

                            WaitForSingleObject(hCaptureEvent, 1000)

                            Dim captureStatus As MWCAP_VIDEO_CAPTURE_STATUS
                            xr = MWGetVideoCaptureStatus(hChannel, captureStatus)

                            Dim llCurrent As Long = 0
                            xr = MWGetDeviceTime(hChannel, llCurrent)

                            Dim t_time As Long = 0
                            If (videoSignalStatus.bInterlaced) Then
                                t_time = videoFrameInfo.allFieldBufferedTimes(1)
                            Else
                                t_time = videoFrameInfo.allFieldBufferedTimes(0)
                            End If
                            llTotalTime += (llCurrent - t_time)

                        End If

                    Next

                    Console.Write("End capture." + vbCrLf)
                    Dim t_duration As Integer = llTotalTime / (num_capture * 10000)
                    Console.Write("Each frame average capture duration is {0} ms." + vbCrLf, t_duration)

                    MWUnpinVideoBuffer(hChannel, bitmapData.Scan0)

                    xr = MWUnregisterNotify(hChannel, hNotify)

                    xr = MWStopVideoCapture(hChannel)

                    bitmap.UnlockBits(bitmapData)

                    Dim pngClsid As System.Drawing.Imaging.ImageCodecInfo
                    pngClsid = GetEncoderInfo("image/bmp")
                    Dim encoder As Encoder
                    encoder = System.Drawing.Imaging.Encoder.Quality
                    Dim param As New System.Drawing.Imaging.EncoderParameters
                    Dim encode_param As System.Drawing.Imaging.EncoderParameter
                    encode_param = New EncoderParameter(encoder, CType(50L, Int32))
                    param.Param(0) = encode_param

                    Dim homedrive As String
                    Dim homepath As String
                    Dim foldername As String
                    foldername = "\Magewell"
                    Dim folderpath As String
                    homedrive = Environ("HOMEDRIVE")
                    homepath = Environ("HOMEPATH")
                    folderpath = homedrive + homepath + foldername
                    Dim lret As Long
                    lret = PathFileExists(folderpath)
                    If lret = 0 Then
                        MkDir(folderpath)
                    End If
                    Dim filename = "\CaptureByInput.bmp"
                    Dim filePath As String
                    filePath = folderpath + filename

                    bitmap.Save(filePath, pngClsid, param)
                    Console.Write(vbCrLf + "The last frame is saved in {0}." + vbCrLf + vbCrLf, filePath)

                End If

            End If

        End If
        CloseHandle(hNotifyEvent)
        CloseHandle(hCaptureEvent)

        MWStopVideoCapture(hChannel)
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
    Public Function GetEncoderInfo(ByVal type As String) As System.Drawing.Imaging.ImageCodecInfo
        Dim all_encoders() As ImageCodecInfo
        all_encoders = ImageCodecInfo.GetImageEncoders()
        Dim num_encoders As Integer = all_encoders.Length()

        Dim i As Integer = 0
        For i = 0 To num_encoders - 1 Step 1
            If (String.Compare(all_encoders(i).MimeType, type) = 0) Then
                Exit For
            End If
        Next
        Return all_encoders(i)
    End Function
    Sub Main(ByVal cmdargs() As String)
        ShowInfo()

        Initialize()

        If WriteFilePermissionTest() <> True Then
            MWCaptureExitInstance()
            Return
        End If

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

        If CheckSignal() <> True Then
            CloseDevice()
            ExitProgram()
            Return
        End If

        StartCapture()

        CloseDevice()

        ExitProgram()
    End Sub

End Module
