Imports LibVBMWCapture.LibMWCapture
Imports LibVBKernel32.LibKernel32
Imports System.Runtime.InteropServices
Imports System.IO
Module Module1
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Pack:=1)> _
    Public Structure WAVEFORMAT
        Dim wFormatTag As Short '';        /* format type */
        Dim nChannels As Short '';         /* number of channels (i.e. mono, stereo, etc.) */
        Dim nSamplesPerSec As Integer '';    /* sample rate */
        Dim nAvgBytesPerSec As Integer '';   /* for buffer estimation */
        Dim nBlockAlign As Short '';       /* block size of data */
    End Structure

    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Pack:=1)> _
    Public Structure PCMWAVEFORMAT
        Dim wf As WAVEFORMAT '';
        Dim wBitsPerSample As Short '';
    End Structure
    <StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Ansi, Pack:=1)> _
    Public Structure WAVE_FILE_HEADER
        Dim dwRiffFlag As Integer '' ; // 'RIFF'
        Dim dwFileSize As Integer '' ;
        Dim dwWaveFlag As Integer ''; // 'WAVE'
        Dim dwFmtFlag As Integer '';  // 'fmt'
        Dim dwFmtSize As Integer '';
        Dim pcmFormat As PCMWAVEFORMAT '';
        Dim dwDataFlag As Integer ''; // 'data'
        Dim dwDataSize As Integer '';
    End Structure
    Private Declare Function PathFileExists Lib "shlwapi.dll" Alias "PathFileExistsA" (ByVal pszPath As String) As Long

    Dim hChannel As System.IntPtr
    Dim arrayProDevChannel(10) As Integer
    Dim xr As MW_RESULT
    Dim audioSignalStatus As MWCAP_AUDIO_SIGNAL_STATUS
    Dim nChannelCount As Integer

    Private Sub StructToByte(ByVal objStruct As Object, ByRef bytes() As Byte)
        Dim size As Integer = Marshal.SizeOf(objStruct)
        ReDim bytes(size - 1) ''size should minus 1,or it would be get 1 more byte
        Dim ptrStruct As IntPtr = Marshal.AllocHGlobal(size - 1)
        Marshal.StructureToPtr(objStruct, ptrStruct, False)
        Marshal.Copy(ptrStruct, bytes, 0, size)
        Marshal.FreeHGlobal(ptrStruct)
    End Sub

    Sub ShowInfo()
        Dim byMaj As Byte, byMin As Byte
        Dim wBuild As Integer
        MWGetVersion(byMaj, byMin, wBuild)

        Console.Write("Magewell MWCapture SDK {0}.{1}.1.{2} - AudioCaptureVB" + vbCrLf, byMaj, byMin, wBuild)
        Console.Write("USB and Eco Devices are not supported" + vbCrLf)
        Console.Write("Usage:" + vbCrLf)
        Console.Write("AudioCaptureVB.exe <channel index>" + vbCrLf)
        Console.Write("AudioCaptureVB.exe <board id>:<channel id>" + vbCrLf + vbCrLf)
    End Sub

    Sub Initialize()
        hChannel = IntPtr.Zero
        nChannelCount = 0

        MWCaptureInitInstance()
    End Sub

    Function WriteFilePerssionTest() As Boolean
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
        Dim filename = "\temp.wav"
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
        Dim bret As Boolean = False
        nChannelCount = MWGetChannelCount()
        If nChannelCount = 0 Then
            Console.Write("ERROR: Can't find channels!" + vbCrLf)
            Return False
        End If

        ReDim arrayProDevChannel(nChannelCount)
        For i As Integer = 0 To nChannelCount - 1
            arrayProDevChannel(i) = -1
        Next

        Dim pro_name As String = "Pro Capture"
        Dim nProCount As Integer = 0
        For i As Integer = 0 To nChannelCount - 1
            Dim info As MWCAP_CHANNEL_INFO
            Dim mr As MW_RESULT = MW_RESULT.MW_SUCCEEDED
            mr = MWGetChannelInfoByIndex(i, info)
            If StrComp(pro_name, info.szFamilyName) = 0 Then
                arrayProDevChannel(i) = i
                nProCount = nProCount + 1
            End If
        Next

        If nChannelCount < 2 Then
            Console.Write("Find {0} channel." + vbCrLf, nProCount)
        Else
            Console.Write("Find {0} channels." + vbCrLf, nProCount)
        End If
        bret = True
        Return (bret)
    End Function

    Function OpenChannel(ByVal cmdargs() As String) As Boolean
        Dim bret As Boolean = False
        Dim t_n_pro_index As Integer = -1
        For i As Integer = 0 To nChannelCount - 1
            If arrayProDevChannel(i) <> -1 Then
                t_n_pro_index = arrayProDevChannel(i)
                Exit For
            End If
        Next
        If t_n_pro_index = -1 Then
            Console.Write("ERROR: Can't find Pro Capture!" + vbCrLf)
            Return False
        End If

        Dim byBoardID As Integer = -1
        Dim byChannelId As Integer = -1
        Dim nDevIndex As Integer = -1
        Dim bIndex As Boolean = False

        Dim videoInfo As MWCAP_CHANNEL_INFO

        If (cmdargs.Length() > 1) Then
            Console.Write("ERROR：Too many params!" + vbCrLf)
            Console.Write(vbCrLf + "Press 'Enter' to exit!" + vbCrLf)
            Console.ReadKey()
            Return False
        End If

        If (cmdargs.Length() = 0) Then
            Dim t_mr As MW_RESULT = MW_RESULT.MW_FAILED
            t_mr = MWGetChannelInfoByIndex(arrayProDevChannel(t_n_pro_index), videoInfo)
            If t_mr <> MW_RESULT.MW_SUCCEEDED Then
                Console.Write("ERROR: Can't get channel info!" + vbCrLf)
                Return False
            End If
            bIndex = True
            nDevIndex = t_n_pro_index
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
                            byBoardID = CInt(byte_1) - Asc("0")
                        ElseIf (((byte_1 >= Asc("a")) And (byte_1 <= Asc("f")))) Then
                            byBoardID = CInt(byte_1) - Asc("a") + 10
                        Else
                            byBoardID = CInt(byte_1) - Asc("A") + 10
                        End If
                    Else
                        byBoardID = -1
                    End If
                    If ((byte_2 >= Asc("0")) And (byte_2 <= Asc("3"))) Then
                        byChannelId = CInt(byte_2) - Asc("0")
                    Else
                        byChannelId = -1
                    End If
                Else
                    byBoardID = -1
                    byChannelId = -1
                End If
                If ((byBoardID = -1) Or (byChannelId = -1)) Then
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
                Console.Write("ERROR: Open channel {0} error!" + vbCrLf, nDevIndex)
                Return False
            End If
        Else
            hChannel = MWOpenChannel(byBoardID, byChannelId)
            If (hChannel = IntPtr.Zero) Then
                Console.Write("ERROR: Open channel {0:x}:{1} error!" + vbCrLf, byBoardID, byChannelId)
                Return False
            End If
        End If

        If (MW_RESULT.MW_SUCCEEDED <> MWGetChannelInfo(hChannel, videoInfo)) Then
            Console.Write("ERROR: Can't get channel info!" + vbCrLf)
            Return False
        End If

        Console.Write("Open channel - BoardIndex = {0:x}, ChannelIndex = {1}." + vbCrLf, videoInfo.byBoardIndex, videoInfo.byChannelIndex)
        Console.Write("Product Name:{0}" + vbCrLf, videoInfo.szProductName)
        Console.Write("Board SerialNo:{0}" + vbCrLf + vbCrLf, videoInfo.szBoardSerialNo)

        Return True
    End Function

    Function CheckSignal() As Boolean
        Dim dwInputCount As Integer = 0
        xr = MWGetAudioInputSourceArray(hChannel, IntPtr.Zero, dwInputCount)
        If (dwInputCount = 0) Then
            Console.Write("ERROR: Can't find audio input!" + vbCrLf)
            Return False
        Else
            MWGetAudioSignalStatus(hChannel, audioSignalStatus)
            If (audioSignalStatus.wChannelValid = 0) Then
                Console.Write("ERROR: Audio signal is invalid" + vbCrLf)
                Return False
            Else
                Dim nSupChannelCount As Integer = 0
                Dim nBitDepthInByte As Integer = audioSignalStatus.cBitsPerSample / 8
                Dim bIsLPCM As Byte = audioSignalStatus.bLPCM

                Dim chSupChannels As String
                For i As Integer = 0 To 3 Step 1
                    If (audioSignalStatus.wChannelValid And (&H1& << i)) Then
                        nSupChannelCount += 2
                        chSupChannels += CStr(i * 2 + 1)
                        chSupChannels += "&"
                        chSupChannels += CStr(i * 2 + 2)
                    End If
                Next

                Console.Write("Audio Signal:bValid={0} SampleRate={1} ChannelValid={2}" + vbCrLf, _
                              audioSignalStatus.bChannelStatusValid, audioSignalStatus.dwSampleRate, chSupChannels)

                Return True
            End If
        End If
    End Function

    Sub StartCapture()

        Dim xr As LibVBMWCapture.LibMWCapture.MW_RESULT

        xr = MWStartAudioCapture(hChannel)
        If (xr <> MW_RESULT.MW_SUCCEEDED) Then
            Console.Write("ERROR: Open Audio Capture error!" + vbCrLf)
            Return
        Else
            Dim hEvent As IntPtr = LibVBKernel32.LibKernel32.CreateEvent(IntPtr.Zero, False, False, IntPtr.Zero)
            Dim hNotify As ULong = MWRegisterNotify(hChannel, hEvent, MWCAP_NOTIFY_AUDIO_SIGNAL_CHANGE Or MWCAP_NOTIFY_AUDIO_FRAME_BUFFERED)

            Dim llBeigin As Long = 0
            xr = MWGetDeviceTime(hChannel, llBeigin)

            Dim nSampleRate As Integer = audioSignalStatus.dwSampleRate
            Dim nBitDepthInByte As Integer = audioSignalStatus.cBitsPerSample / 8
            Dim bIsLPCM As Byte = audioSignalStatus.bLPCM
            Dim chSupChannels As String
            Dim nSupChannelCount As Integer = 0
            For i As Integer = 0 To 3 Step 1
                If (audioSignalStatus.wChannelValid And (&H1& << i)) Then
                    nSupChannelCount += 2
                    chSupChannels += CStr(i * 2 + 1)
                    chSupChannels += "&"
                    chSupChannels += CStr(i * 2 + 2)
                End If
            Next

            ''init header
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
            Dim filename = "\AudioCapture.wav"
            Dim filePath As String
            filePath = folderpath + filename
            Dim fs As New FileStream(filePath, FileMode.Create, FileAccess.ReadWrite)
            Dim bw As New System.IO.BinaryWriter(fs)

            Dim m_wfHeader As WAVE_FILE_HEADER
            m_wfHeader.pcmFormat.wBitsPerSample = nBitDepthInByte * 8 ''bits per sample
            m_wfHeader.pcmFormat.wf.wFormatTag = 1 ''PCM
            m_wfHeader.pcmFormat.wf.nChannels = nSupChannelCount  ''
            m_wfHeader.pcmFormat.wf.nSamplesPerSec = audioSignalStatus.dwSampleRate  ''
            m_wfHeader.pcmFormat.wf.nBlockAlign = (m_wfHeader.pcmFormat.wBitsPerSample) * (m_wfHeader.pcmFormat.wf.nChannels) / 8
            m_wfHeader.pcmFormat.wf.nAvgBytesPerSec = (m_wfHeader.pcmFormat.wf.nBlockAlign) * (m_wfHeader.pcmFormat.wf.nSamplesPerSec)

            m_wfHeader.dwDataFlag = 1635017060 ''atad
            m_wfHeader.dwDataSize = 0
            Dim bytes(1) As Byte
            StructToByte(m_wfHeader, bytes)
            bw.Write(bytes)

            Console.Write(vbCrLf + "The file is saved in {0}." + vbCrLf + vbCrLf, filePath)
            Console.Write("Begin capture 1000 frames..." + vbCrLf)
            For i As Integer = 0 To 999 Step 1
                WaitForSingleObject(hEvent, 1000)

                Dim ullStatusBits As ULong = MWCAP_NOTIFY_AUDIO_FRAME_BUFFERED
                xr = MWGetNotifyStatus(hChannel, hNotify, ullStatusBits)
                If (xr <> MW_RESULT.MW_SUCCEEDED) Then
                    Continue For
                End If

                If (ullStatusBits And MWCAP_NOTIFY_AUDIO_SIGNAL_CHANGE) Then
                    MWGetAudioSignalStatus(hChannel, audioSignalStatus)
                    If (audioSignalStatus.wChannelValid <> 0) Then
                        chSupChannels = ""
                        For j As Integer = 0 To 3 Step 1
                            If (audioSignalStatus.wChannelValid And (&H1& << i)) Then
                                chSupChannels += CStr(i * 2 + 1)
                                chSupChannels += "&"
                                chSupChannels += CStr(i * 2 + 2)
                            End If
                        Next
                        Console.Write("Audio Signal:bValid={0} SampleRate={1} ChannelValid={2}" + vbCrLf, _
                      audioSignalStatus.bChannelStatusValid, audioSignalStatus.dwSampleRate, chSupChannels)
                    Else
                        Console.Write("ERROR: Audio signal is invalid !" + vbCrLf)
                    End If
                    Exit For
                End If

                If (ullStatusBits And MWCAP_NOTIFY_AUDIO_FRAME_BUFFERED) Then
                    Do
                        Dim audioFrame As MWCAP_AUDIO_CAPTURE_FRAME
                        xr = MWCaptureAudioFrame(hChannel, audioFrame)
                        If (xr = MW_RESULT.MW_SUCCEEDED And bIsLPCM) Then
                            Dim asAudioSamples(MWCAP_AUDIO_SAMPLES_PER_FRAME * MWCAP_AUDIO_MAX_NUM_CHANNELS * 4) As Byte
                            Dim len = audioFrame.adwSamples.Length()
                            Dim bAdwSamples(len * 4) As Byte
                            Dim ptr As IntPtr = Marshal.UnsafeAddrOfPinnedArrayElement(bAdwSamples, 0)
                            Marshal.Copy(audioFrame.adwSamples, 0, ptr, len)

                            For j As Integer = 0 To nSupChannelCount / 2 - 1 Step 1
                                For k As Integer = 0 To MWCAP_AUDIO_SAMPLES_PER_FRAME - 1 Step 1
                                    Dim nWritePos As Integer = (k * nSupChannelCount + j * 2) * nBitDepthInByte
                                    Dim nReadPos As Integer = (k * MWCAP_AUDIO_MAX_NUM_CHANNELS + j) * 4
                                    Dim nReadPos2 As Integer = (k * MWCAP_AUDIO_MAX_NUM_CHANNELS + j + MWCAP_AUDIO_MAX_NUM_CHANNELS / 2) * 4

                                    For m As Integer = 0 To nBitDepthInByte - 1 Step 1
                                        asAudioSamples(nWritePos + m) = bAdwSamples(nReadPos + 4 - nBitDepthInByte + m)
                                        asAudioSamples(nWritePos + nBitDepthInByte + m) = bAdwSamples(nReadPos2 + 4 - nBitDepthInByte + m)
                                    Next
                                Next
                            Next
                            Dim w_size = MWCAP_AUDIO_SAMPLES_PER_FRAME * nSupChannelCount * nBitDepthInByte
                            bw.Write(asAudioSamples, 0, w_size)
                            m_wfHeader.dwDataSize += w_size
                        End If
                    Loop While (xr = MW_RESULT.MW_SUCCEEDED)
                End If
            Next

            ''file exit
            m_wfHeader.dwFileSize = m_wfHeader.dwDataSize + 32
            bw.Seek(0, SeekOrigin.Begin)
            Dim bytes2(1) As Byte
            StructToByte(m_wfHeader, bytes2)
            bw.Write(bytes2)
            bw.Close()

            Dim llEnd As Long = 0
            xr = MWGetDeviceTime(hChannel, llEnd)
            Dim t_duration As Integer = (llEnd - llBeigin) / 10000
            Console.Write("End capture, samples is {0}, the duration is {1:d} ms." + vbCrLf, MWCAP_AUDIO_SAMPLES_PER_FRAME * 1000, t_duration)
            Console.Write("Write audio samples to AudioCapture.wav ." + vbCrLf)

            MWUnregisterNotify(hChannel, hNotify)
            CloseHandle(hEvent)

            MWStopAudioCapture(hChannel)
            bw.Close()
        End If

        If (hChannel <> IntPtr.Zero) Then
            MWCloseChannel(hChannel)
            hChannel = IntPtr.Zero
        End If

    End Sub
    Sub CloseDevice()
        If hChannel <> IntPtr.Zero Then
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

        Dim bret As Boolean = False
        bret = WriteFilePerssionTest()
        If bret = False Then
            MWCaptureExitInstance()
            Return
        End If

        bret = ConfigValidChannel()
        If bret = False Then
            ExitProgram()
            MWCaptureExitInstance()
            Return
        End If

        bret = OpenChannel(cmdargs)
        If bret = False Then
            ExitProgram()
            MWCaptureExitInstance()
            Return
        End If

        bret = CheckSignal()
        If bret = False Then
            CloseDevice()
            ExitProgram()
            Return
        End If

        StartCapture()

        CloseDevice()

        ExitProgram()
      
    End Sub

End Module
