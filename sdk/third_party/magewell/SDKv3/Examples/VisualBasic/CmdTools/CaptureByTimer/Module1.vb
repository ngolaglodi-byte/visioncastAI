Imports LibVBMWCapture.LibMWCapture
Imports LibVBMWCapture.MWcap_FOURCC
Imports LibVBKernel32.LibKernel32
Imports System.Runtime.InteropServices
Imports System
Imports System.IO
Imports System.Drawing
Imports System.Drawing.Imaging

Module Module1
    Dim hChannel As IntPtr
    Dim arrProDevChannel(10) As Integer
    Dim nChannelCount As Integer

    Dim num_capture As Integer = 100
    Dim timer_duration As Integer = 400000

    Private Declare Function PathFileExists Lib "shlwapi.dll" Alias "PathFileExistsA" (ByVal pszPath As String) As Long

    Sub ShowInfo()
        Dim byMaj As Byte, byMin As Byte
        Dim wBuild As Integer
        MWGetVersion(byMaj, byMin, wBuild)
        Console.Write("Magewell MWCapture SDK {0}.{1}.1.{2}- CaptureByTimerVB" + vbCrLf, byMaj, byMin, wBuild)
        Console.Write("USB and Eco Devices are not supported" + vbCrLf)
        Console.Write("Usage:" + vbCrLf)
        Console.Write("CaptureByTimerVB.exe <channel index>" + vbCrLf)
        Console.Write("CaptureByTimerVB.exe <board id>:<channel id>" + vbCrLf)
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
    Sub StartCapture()
        Dim hNotifyEvent As IntPtr = CreateEvent(IntPtr.Zero, False, False, IntPtr.Zero)
        Dim hCaptureEvent As IntPtr = CreateEvent(IntPtr.Zero, False, False, IntPtr.Zero)

        Dim fourcc As New LibVBMWCapture.MWcap_FOURCC

        Dim xr As MW_RESULT
        xr = MWStartVideoCapture(hChannel, hCaptureEvent)
        If (xr <> MW_RESULT.MW_SUCCEEDED) Then
            CloseHandle(hNotifyEvent)
            CloseHandle(hCaptureEvent)
            Console.Write("ERROR: Open Video Capture error!" + vbCrLf)
        Else
            Dim cx As Integer = 1920
            Dim cy As Integer = 1080

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

            Dim hTimerEvent As ULong = MWRegisterTimer(hChannel, hNotifyEvent)
            Console.Write("Begin capture {0} frames in 1920 x 1080, RGB24,{1} fps..." + vbCrLf, num_capture, 25)
            Dim llBegin As Long = 0
            xr = MWGetDeviceTime(hChannel, llBegin)
            Dim llEnd As Long = 0

            MWPinVideoBuffer(hChannel, bitmapData.Scan0, bitmapData.Stride * cy)

            Dim i As Integer = 0
            For i = 0 To num_capture - 1
                xr = LibVBMWCapture.LibMWCapture.MWScheduleTimer(hChannel, hTimerEvent, llBegin + i * timer_duration)
                If (xr <> MW_RESULT.MW_SUCCEEDED) Then
                    Continue For
                End If

                WaitForSingleObject(hNotifyEvent, 1000)

                Dim videoBufferInfo As MWCAP_VIDEO_BUFFER_INFO
                xr = MWGetVideoBufferInfo(hChannel, videoBufferInfo)
                If (xr <> MW_RESULT.MW_SUCCEEDED) Then
                    Continue For
                End If

                Dim videoFrameInfo As MWCAP_VIDEO_FRAME_INFO
                xr = MWGetVideoFrameInfo(hChannel, videoBufferInfo.iNewestBufferedFullFrame, videoFrameInfo)
                If (xr <> MW_RESULT.MW_SUCCEEDED) Then
                    Continue For
                End If



                xr = MWCaptureVideoFrameToVirtualAddress(hChannel, videoBufferInfo.iNewestBufferedFullFrame, _
                                                         bitmapData.Scan0, bitmapData.Stride * cy, bitmapData.Stride, _
                                                         bBottomUp, IntPtr.Zero, fourcc.MWCAP_FOURCC_BGR24, cx, cy)

                WaitForSingleObject(hCaptureEvent, 1000)

                Dim captureStatus As MWCAP_VIDEO_CAPTURE_STATUS
                xr = MWGetVideoCaptureStatus(hChannel, captureStatus)
            Next

            xr = MWGetDeviceTime(hChannel, llEnd)

            Dim t_duration As Integer = (llEnd - llBegin) / 10000
            Console.Write("End capture, the duration is {0} ms." + vbCrLf, t_duration)

            t_duration = (llEnd - llBegin) / (num_capture * 10000)
            Console.Write("Each frame average capture duration is {0}ms." + vbCrLf, t_duration)

            MWUnpinVideoBuffer(hChannel, bitmapData.Scan0)

            xr = MWUnregisterTimer(hChannel, hTimerEvent)

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
            Dim filename = "\CaptureByTimer.bmp"
            Dim filePath As String
            filePath = folderpath + filename

            bitmap.Save(filePath, pngClsid, param)
            Console.Write(vbCrLf + "The last frame is saved in {0}." + vbCrLf + vbCrLf, filePath)

            CloseHandle(hNotifyEvent)
            CloseHandle(hCaptureEvent)
        End If

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

        StartCapture()

        CloseDevice()

        ExitProgram()

    End Sub

End Module
