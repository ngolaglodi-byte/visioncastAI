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

        Console.Write("Magewell MWCapture SDK {0}.{1}.1.{2} - InputSourceVB" + vbCrLf, byMaj, byMin, wBuild)
        Console.Write("All Devices are supported." + vbCrLf)
        Console.Write("Usage:" + vbCrLf)
        Console.Write("InputSourceVB.exe <channel index>" + vbCrLf)
        Console.Write("Pro and Eco capture serial devices: InputSource.exe <board id>:<channel id>" + vbCrLf)
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
        For i As Integer = 0 To nChannelCount - 1
            arrProDevChannel(i) = -1
        Next

        Dim t_mr As MW_RESULT
        Dim info As MWCAP_CHANNEL_INFO
        For i As Integer = 0 To nChannelCount - 1
            t_mr = MWGetChannelInfoByIndex(i, info)
            If t_mr = MW_RESULT.MW_SUCCEEDED Then
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
                t_n_dev_index = arrProDevChannel(i)
                Exit For
            End If
        Next

        If (cmdargs.Length() > 1) Then
            Console.Write("ERROR：Too many params!" + vbCrLf)
            Console.Write(vbCrLf + "Press 'Enter' to exit!" + vbCrLf)
            Console.ReadKey()
            Return False
        End If

        Dim byBoardId As Integer = -1
        Dim byChannelId As Integer = -1
        Dim nDevIndex As Integer = -1
        Dim bIndex As Boolean = False

        Dim videoInfo As MWCAP_CHANNEL_INFO
        If (cmdargs.Length() = 0) Then
            If MWGetChannelInfoByIndex(arrProDevChannel(t_n_dev_index), videoInfo) <> MW_RESULT.MW_SUCCEEDED Then
                Console.Write("ERROR: Can't get channel info!" + vbCrLf)
                Return False
            End If
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
            MWGetDevicePath(nDevIndex, p_path)
            hChannel = MWOpenChannelByPath(p_path)
            path_gchandle.Free()
            If (hChannel = IntPtr.Zero) Then
                Console.Write("ERROR: Open channel {0:x} error!" + vbCrLf, nDevIndex)
                Return False
            End If
        Else
            hChannel = MWOpenChannel(byBoardId, byChannelId)
            If (hChannel = IntPtr.Zero) Then
                Console.Write("ERROR: Open channel {0:x}:{1:d} error!" + vbCrLf, byBoardId, byChannelId)
                Return False
            End If
        End If

        If MWGetChannelInfo(hChannel, videoInfo) <> MW_RESULT.MW_SUCCEEDED Then
            Console.Write("ERROR: Can't get channel info!" + vbCrLf)
            Return False
        End If

        Console.Write("Open channel - BoardIndex = {0:x}, ChannelIndex = {1:d}." + vbCrLf, videoInfo.byBoardIndex, videoInfo.byChannelIndex)
        Console.Write("Product Name: {0}" + vbCrLf, videoInfo.szProductName)
        Console.Write("Board SerialNo: {0}" + vbCrLf + vbCrLf, videoInfo.szBoardSerialNo)

        Return True
    End Function
    Sub PrintInputSourceInfo()
        ''Video Input Source
        Dim t_ret As MW_RESULT = MW_RESULT.MW_FAILED
        Dim dwVideoInputCount As Integer = 0
        Dim ptr As IntPtr = IntPtr.Zero
        t_ret = MWGetVideoInputSourceArray(hChannel, ptr, dwVideoInputCount)
        If (t_ret = MW_RESULT.MW_SUCCEEDED) Then
            Console.Write("Video Input Count :{0}" + vbCrLf, dwVideoInputCount)
            Dim video_input() As Integer
            ReDim video_input(dwVideoInputCount)
            Dim video_input_gchandle As GCHandle = GCHandle.Alloc(video_input, GCHandleType.Pinned)
            Dim p_video_input As IntPtr = video_input_gchandle.AddrOfPinnedObject()
            t_ret = MWGetVideoInputSourceArray(hChannel, p_video_input, dwVideoInputCount)
            If (t_ret = MW_RESULT.MW_SUCCEEDED) Then
                Dim strInputName As String
                For i As Integer = 0 To (dwVideoInputCount - 1) Step 1
                    GetVideoInputName(video_input(i), strInputName)
                    Console.Write("[{0}] {1}" + vbCrLf, i, strInputName)
                Next
            End If
            video_input_gchandle.Free()
        End If

        Dim dwVideoInput As Integer = 0
        t_ret = MWGetVideoInputSource(hChannel, dwVideoInput)
        If (t_ret = MW_RESULT.MW_SUCCEEDED) Then
            Dim strInputName As String
            GetVideoInputName(dwVideoInput, strInputName)
            Console.Write("Current Video Input Source:{0}" + vbCrLf + vbCrLf, strInputName)
        End If

        '' Audio Input Source
        Dim dwAudioInputCount As Integer = 0
        t_ret = MWGetAudioInputSourceArray(hChannel, ptr, dwAudioInputCount)
        If (t_ret = MW_RESULT.MW_SUCCEEDED) Then
            Console.Write("Audio Input Count :{0}" + vbCrLf, dwAudioInputCount)
            Dim audio_input() As Integer
            ReDim audio_input(dwAudioInputCount)
            Dim audio_input_gchandle As GCHandle = GCHandle.Alloc(audio_input, GCHandleType.Pinned)
            Dim p_audio_input As IntPtr = audio_input_gchandle.AddrOfPinnedObject()
            t_ret = MWGetAudioInputSourceArray(hChannel, p_audio_input, dwAudioInputCount)
            If (t_ret = MW_RESULT.MW_SUCCEEDED) Then
                Dim strInputName As String
                For i As Integer = 0 To (dwAudioInputCount - 1) Step 1
                    GetAudioInputName(audio_input(i), strInputName)
                    Console.Write("[{0}] {1}" + vbCrLf, i, strInputName)
                Next
            End If
            audio_input_gchandle.Free()
        End If

        Dim dwAudioInput As Integer = 0
        t_ret = MWGetAudioInputSource(hChannel, dwAudioInput)
        If (t_ret = MW_RESULT.MW_SUCCEEDED) Then
            Dim strInputName As String
            GetAudioInputName(dwAudioInput, strInputName)
            Console.Write("Current Audio Input Source:{0}" + vbCrLf + vbCrLf, strInputName)
        End If
    End Sub
    Public Sub GetVideoInputName(ByVal dwVideoInput As Integer, ByRef pszName As String)
        Select Case (dwVideoInput >> 8)
            Case MWCAP_VIDEO_INPUT_TYPE.MWCAP_VIDEO_INPUT_TYPE_NONE
                pszName = "None"
            Case MWCAP_VIDEO_INPUT_TYPE.MWCAP_VIDEO_INPUT_TYPE_HDMI
                pszName = "HDMI"
            Case MWCAP_VIDEO_INPUT_TYPE.MWCAP_VIDEO_INPUT_TYPE_VGA
                pszName = "VGA"
            Case MWCAP_VIDEO_INPUT_TYPE.MWCAP_VIDEO_INPUT_TYPE_SDI
                pszName = "SDI"
            Case MWCAP_VIDEO_INPUT_TYPE.MWCAP_VIDEO_INPUT_TYPE_COMPONENT
                pszName = "Component"
            Case MWCAP_VIDEO_INPUT_TYPE.MWCAP_VIDEO_INPUT_TYPE_CVBS
                pszName = "CVBS"
            Case MWCAP_VIDEO_INPUT_TYPE.MWCAP_VIDEO_INPUT_TYPE_YC
                pszName = "YC"
        End Select
    End Sub

    Public Sub GetAudioInputName(ByVal dwAudioInput As Integer, ByRef pszName As String)
        Select Case (dwAudioInput >> 8)
            Case MWCAP_AUDIO_INPUT_TYPE.MWCAP_AUDIO_INPUT_TYPE_NONE
                pszName = "None"
            Case MWCAP_AUDIO_INPUT_TYPE.MWCAP_AUDIO_INPUT_TYPE_HDMI
                pszName = "HDMI"
            Case MWCAP_AUDIO_INPUT_TYPE.MWCAP_AUDIO_INPUT_TYPE_SDI
                pszName = "SDI"
            Case MWCAP_AUDIO_INPUT_TYPE.MWCAP_AUDIO_INPUT_TYPE_LINE_IN
                pszName = "Line In"
            Case MWCAP_AUDIO_INPUT_TYPE.MWCAP_AUDIO_INPUT_TYPE_MIC_IN
                pszName = "Mic In"
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

        PrintInputSourceInfo()

        CloseDevice()

        ExitProgram()
    End Sub

End Module
