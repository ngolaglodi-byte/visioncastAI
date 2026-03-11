Imports System.Threading
Imports System
Imports System.Runtime.InteropServices
Imports System.Collections.Generic
Imports System.Linq
Imports System.Text
Public Class CaptureThread
    Public Enum CLIP
        NO_CLIP = 0
        TOP_LEFT = 1
        TOP_RIGHT = 2
        BOTTOM_LEFT = 3
        BOTTOM_RIGHT = 4
    End Enum

    Protected m_hChannel As IntPtr
    Protected m_nIndex As Integer

    ''video
    Protected m_dwWidth As Integer = 1920
    Protected m_dwHeight As Integer = 1080
    Protected m_dwFourcc As UInteger
    Protected m_dwFrameDuration As Integer
    Protected m_hWnd As IntPtr = IntPtr.Zero
    Protected m_hRender As IntPtr
    Protected m_hThread As Threading.Thread
    Protected m_strFps As String
    Protected m_hExitEvent As IntPtr

    ''audio
    Protected m_hAudioRender As IntPtr
    Protected m_hAudioThread As Threading.Thread
    Protected m_hAudioExitEvent As IntPtr
    Protected m_llAudioSample As ULong
    Protected m_dSamplePerSecond As Double
    Protected m_bAudioValid As Boolean
    Protected m_bLPCM As Byte
    Protected m_cBitsPerSample As Byte
    Protected m_dwSampleRate As Integer = 48000

    Protected m_ullStatusBits As ULong



    Public Sub CaptureThread()
        Dim mwcapFourcc As New LibVBMWCapture.MWcap_FOURCC
        Dim dwMinStride As UInteger = mwcapFourcc.FOURCC_CalcMinStride(m_dwFourcc, m_dwWidth, 2)
        Dim dwImageSize As UInteger = mwcapFourcc.FOURCC_CalcImageSize(m_dwFourcc, m_dwWidth, m_dwHeight, dwMinStride)

        Dim pbImage As IntPtr
        pbImage = Marshal.AllocCoTaskMem(dwImageSize)
        If (pbImage = IntPtr.Zero) Then
            Return
        End If

        Dim hCaptureEvent As IntPtr
        hCaptureEvent = LibVBKernel32.LibKernel32.CreateEvent(IntPtr.Zero, False, False, IntPtr.Zero)
        If (hCaptureEvent = IntPtr.Zero) Then
            Return
        End If

        Dim hTimerEvent As IntPtr
        hTimerEvent = LibVBKernel32.LibKernel32.CreateEvent(IntPtr.Zero, False, False, IntPtr.Zero)
        If (hTimerEvent = IntPtr.Zero) Then
            Return
        End If

        Dim hTimer As ULong
        hTimer = LibVBMWCapture.LibMWCapture.MWRegisterTimer(m_hChannel, hTimerEvent)
        If (hTimer = 0) Then
            Return
        End If

        Dim mr As LibVBMWCapture.LibMWCapture.MW_RESULT
        mr = LibVBMWCapture.LibMWCapture.MWStartVideoCapture(m_hChannel, hCaptureEvent)

        Dim llStartTime As ULong
        mr = LibVBMWCapture.LibMWCapture.MWGetDeviceTime(m_hChannel, llStartTime)

        Dim captureStatus As LibVBMWCapture.LibMWCapture.MWCAP_VIDEO_CAPTURE_STATUS
        Dim llCaptureCount As ULong
        Dim llCaptureTime As ULong = llStartTime
        Dim llNow As ULong = 0
        Dim llRefTime As ULong = 0
        Dim events() As IntPtr = {m_hExitEvent, hTimerEvent}
        Dim events2() As IntPtr = {m_hExitEvent, hCaptureEvent}

        'LibVBMWCapture.LibMWCapture.MWPinVideoBuffer(m_hChannel, pbImage, dwImageSize)

        Do While (1)
            llCaptureTime += m_dwFrameDuration
            mr = LibVBMWCapture.LibMWCapture.MWGetDeviceTime(m_hChannel, llNow)
            If (llNow > llCaptureTime) Then
                Continue Do
            End If

            mr = LibVBMWCapture.LibMWCapture.MWScheduleTimer(m_hChannel, hTimer, llCaptureTime)
            If (mr <> LibVBMWCapture.LibMWCapture.MW_RESULT.MW_SUCCEEDED) Then
                Exit Do
            End If
            Dim dwRet As Integer
            dwRet = LibVBKernel32.LibKernel32.WaitForMultipleObjects(2, events, 0, LibVBKernel32.LibKernel32.INFINITE)
            If (dwRet <> 1) Then
                Exit Do
            End If

            mr = LibVBMWCapture.LibMWCapture.MWCaptureVideoFrameToVirtualAddress(m_hChannel, -1, pbImage, dwImageSize, dwMinStride, 0, 0, m_dwFourcc, m_dwWidth, m_dwHeight)
            If (mr <> LibVBMWCapture.LibMWCapture.MW_RESULT.MW_SUCCEEDED) Then
                Exit Do
            End If

            dwRet = LibVBKernel32.LibKernel32.WaitForMultipleObjects(2, events2, 0, LibVBKernel32.LibKernel32.INFINITE)
            If (dwRet <> 1) Then
                Exit Do
            End If

            mr = LibVBMWCapture.LibMWCapture.MWGetVideoCaptureStatus(m_hChannel, captureStatus)
            If (mr <> LibVBMWCapture.LibMWCapture.MW_RESULT.MW_SUCCEEDED) Then
                Exit Do
            End If

            LibVBMWMedia.LibMWMedia.MWD3DRendererPushFrame(m_hRender, pbImage, dwMinStride)

            llCaptureCount += 1

            If (llCaptureCount >= 10) Then
                Dim llCurrentTime As ULong = 0
                LibVBMWCapture.LibMWCapture.MWGetDeviceTime(m_hChannel, llCaptureTime)
                m_strFps = ((llCaptureCount * 10000000) / (llCaptureTime - llRefTime)).ToString("0.00")
                llRefTime = llCaptureTime
                llCaptureCount = 0
            End If
        Loop

        If (pbImage <> IntPtr.Zero) Then
            'LibVBMWCapture.LibMWCapture.MWUnpinVideoBuffer(m_hChannel, pbImage)
            Marshal.FreeCoTaskMem(pbImage)
        End If

        mr = LibVBMWCapture.LibMWCapture.MWStopVideoCapture(m_hChannel)
        If (mr <> LibVBMWCapture.LibMWCapture.MW_RESULT.MW_SUCCEEDED) Then
            Return
        End If

        If (hTimer <> 0) Then
            LibVBMWCapture.LibMWCapture.MWUnregisterTimer(m_hChannel, hTimer)
        End If

        If (hTimerEvent <> IntPtr.Zero) Then
            LibVBKernel32.LibKernel32.CloseHandle(hTimerEvent)
        End If

        If (hCaptureEvent <> IntPtr.Zero) Then
            LibVBKernel32.LibKernel32.CloseHandle(hCaptureEvent)
        End If
    End Sub

    Public Sub AudioCaptureThread()

        Dim hAudioEvent As IntPtr = LibVBKernel32.LibKernel32.CreateEvent(IntPtr.Zero, False, False, IntPtr.Zero)
        Dim hAudioNotify As ULong = 0

        Dim xr As LibVBMWCapture.LibMWCapture.MW_RESULT

        Do
            xr = LibVBMWCapture.LibMWCapture.MWStartAudioCapture(m_hChannel)
            If (xr <> LibVBMWCapture.LibMWCapture.MW_RESULT.MW_SUCCEEDED) Then
                Exit Do
            End If
            Dim t As ULong = (LibVBMWCapture.LibMWCapture.MWCAP_NOTIFY_AUDIO_SIGNAL_CHANGE) Or (LibVBMWCapture.LibMWCapture.MWCAP_NOTIFY_AUDIO_FRAME_BUFFERED)
            hAudioNotify = LibVBMWCapture.LibMWCapture.MWRegisterNotify(m_hChannel, hAudioEvent, t)
            If (hAudioNotify = 0) Then
                Exit Do
            End If

            Dim llBegin As ULong = 0
            xr = LibVBMWCapture.LibMWCapture.MWGetDeviceTime(m_hChannel, llBegin)
            If (xr <> LibVBMWCapture.LibMWCapture.MW_RESULT.MW_SUCCEEDED) Then
                Exit Do
            End If

            Dim dwSampleCount As Integer = 0
            Dim llLast As ULong = llBegin

            Dim events() As IntPtr = {m_hAudioExitEvent, hAudioEvent}

            Dim asAudioSamples(LibVBMWCapture.LibMWCapture.MWCAP_AUDIO_SAMPLES_PER_FRAME * 2) As Short
            Dim as_handle As GCHandle = GCHandle.Alloc(asAudioSamples, GCHandleType.Pinned)
            Dim p_asAudioSamples As IntPtr = as_handle.AddrOfPinnedObject()

            Dim llCurrent As ULong = 0

            Do While (1)
                Dim dwRet As Integer
                dwRet = LibVBKernel32.LibKernel32.WaitForMultipleObjects(2, events, 0, LibVBKernel32.LibKernel32.INFINITE)
                If (dwRet <> 1) Then
                    'LibVBKernel32.LibKernel32.Sleep(100)
                    Exit Do
                End If

                Dim ullStatusBits As ULong = LibVBMWCapture.LibMWCapture.MWCAP_NOTIFY_AUDIO_FRAME_BUFFERED
                xr = LibVBMWCapture.LibMWCapture.MWGetNotifyStatus(m_hChannel, hAudioNotify, m_ullStatusBits)
                If (xr <> LibVBMWCapture.LibMWCapture.MW_RESULT.MW_SUCCEEDED) Then
                    Continue Do
                End If

                If (ullStatusBits And (LibVBMWCapture.LibMWCapture.MWCAP_NOTIFY_AUDIO_SIGNAL_CHANGE)) Then
                    Console.Write("Audio Signal Changed" + vbCrLf)
                    Exit Do
                End If

                If (ullStatusBits And (LibVBMWCapture.LibMWCapture.MWCAP_NOTIFY_AUDIO_FRAME_BUFFERED)) Then
                    Do
                        Dim audioFrame As LibVBMWCapture.LibMWCapture.MWCAP_AUDIO_CAPTURE_FRAME
                        xr = LibVBMWCapture.LibMWCapture.MWCaptureAudioFrame(m_hChannel, audioFrame)
                        If (xr = LibVBMWCapture.LibMWCapture.MW_RESULT.MW_SUCCEEDED) Then


                            For i As Integer = 0 To (LibVBMWCapture.LibMWCapture.MWCAP_AUDIO_SAMPLES_PER_FRAME - 1) Step 1
                                Dim sLeft As Short = (audioFrame.adwSamples(i * 8) >> 16)
                                Dim sRight As Short = (audioFrame.adwSamples(i * 8 + 4) >> 16)

                                asAudioSamples(i * 2) = sLeft
                                asAudioSamples(i * 2 + 1) = sRight
                            Next

                            LibVBMWMedia.LibMWMedia.MWDSoundRendererPushFrame(m_hAudioRender, p_asAudioSamples, LibVBMWCapture.LibMWCapture.MWCAP_AUDIO_SAMPLES_PER_FRAME * 4)

                            dwSampleCount += LibVBMWCapture.LibMWCapture.MWCAP_AUDIO_SAMPLES_PER_FRAME
                            m_llAudioSample += LibVBMWCapture.LibMWCapture.MWCAP_AUDIO_SAMPLES_PER_FRAME

                            LibVBMWCapture.LibMWCapture.MWGetDeviceTime(m_hChannel, llCurrent)

                        End If
                    Loop While (xr = LibVBMWCapture.LibMWCapture.MW_RESULT.MW_SUCCEEDED)
                End If
            Loop
            xr = LibVBMWCapture.LibMWCapture.MWUnregisterNotify(m_hChannel, hAudioNotify)
            xr = LibVBMWCapture.LibMWCapture.MWStopAudioCapture(m_hChannel)

            as_handle.Free()
        Loop While (False)
        LibVBKernel32.LibKernel32.CloseHandle(hAudioEvent)
    End Sub
    Public Function GetCurrentFPS() As String
        Return m_strFps
    End Function

    Public Function Create(ByVal nIndex As Integer, ByVal nWidth As Integer, ByVal nHeight As Integer, _
                           ByVal dwFourcc As UInteger, ByVal dwFrameDuration As Integer, ByVal hWnd As IntPtr) As Boolean
        m_nIndex = nIndex
        m_dwWidth = nWidth
        m_dwHeight = nHeight
        m_dwFourcc = dwFourcc
        m_dwFrameDuration = dwFrameDuration
        m_hWnd = hWnd

        Dim mr As LibVBMWCapture.LibMWCapture.MW_RESULT = LibVBMWCapture.LibMWCapture.MW_RESULT.MW_FAILED
        Dim channelInfo As New LibVBMWCapture.LibMWCapture.MWCAP_CHANNEL_INFO
        mr = LibVBMWCapture.LibMWCapture.MWGetChannelInfoByIndex(nIndex, channelInfo)
        m_hChannel = LibVBMWCapture.LibMWCapture.MWOpenChannel(channelInfo.byBoardIndex, channelInfo.byChannelIndex)
        If (m_hChannel = IntPtr.Zero) Then
            Return False
        End If

        Dim dwAudioInputCount As Integer = 0
        mr = LibVBMWCapture.LibMWCapture.MWGetAudioInputSourceArray(m_hChannel, IntPtr.Zero, dwAudioInputCount)
        If (mr <> LibVBMWCapture.LibMWCapture.MW_RESULT.MW_SUCCEEDED) Then
            Return False
        End If

        Dim audioSignalStatus As LibVBMWCapture.LibMWCapture.MWCAP_AUDIO_SIGNAL_STATUS
        mr = LibVBMWCapture.LibMWCapture.MWGetAudioSignalStatus(m_hChannel, audioSignalStatus)
        If (mr <> LibVBMWCapture.LibMWCapture.MW_RESULT.MW_SUCCEEDED) Then
            Return False
        End If

        m_bAudioValid = ((dwAudioInputCount > 0) And ((audioSignalStatus.wChannelValid And 1) <> 0))
        m_bLPCM = audioSignalStatus.bLPCM
        m_cBitsPerSample = audioSignalStatus.cBitsPerSample
        m_dwSampleRate = audioSignalStatus.dwSampleRate

        m_hExitEvent = LibVBKernel32.LibKernel32.CreateEvent(IntPtr.Zero, False, False, IntPtr.Zero)
        If (m_hExitEvent = IntPtr.Zero) Then
            Return False
        End If

        Dim t_b_reverse As Boolean
        t_b_reverse = False
        m_hRender = LibVBMWMedia.LibMWMedia.MWCreateD3DRenderer(m_dwWidth, m_dwHeight, m_dwFourcc, t_b_reverse, m_hWnd)
        If (m_hRender = IntPtr.Zero) Then
            Return False
        End If

        m_hAudioExitEvent = LibVBKernel32.LibKernel32.CreateEvent(IntPtr.Zero, False, False, IntPtr.Zero)
        If (m_hAudioExitEvent = IntPtr.Zero) Then
            Return False
        End If

        m_hAudioRender = LibVBMWMedia.LibMWMedia.MWCreateDSoundRenderer(m_dwSampleRate, 2, 192, 10)

        m_hThread = New Thread(AddressOf CaptureThread)
        m_hAudioThread = New Thread(AddressOf AudioCaptureThread)
        m_hThread.Start()
        m_hAudioThread.Start()

        Return True
    End Function

    Public Sub Destory()
        If (m_hExitEvent <> IntPtr.Zero) Then
            LibVBKernel32.LibKernel32.SetEvent(m_hExitEvent)
            m_hThread.Join()

            If (m_hExitEvent <> IntPtr.Zero) Then
                LibVBKernel32.LibKernel32.CloseHandle(m_hExitEvent)
            End If
        End If
        If (m_hAudioExitEvent <> IntPtr.Zero) Then
            LibVBKernel32.LibKernel32.SetEvent(m_hAudioExitEvent)
            m_hAudioThread.Join()

            If (m_hAudioExitEvent <> IntPtr.Zero) Then
                LibVBKernel32.LibKernel32.CloseHandle(m_hAudioExitEvent)
            End If
        End If

        If (m_hChannel <> IntPtr.Zero) Then
            LibVBMWCapture.LibMWCapture.MWCloseChannel(m_hChannel)
        End If

        If (m_hRender <> IntPtr.Zero) Then
            LibVBMWMedia.LibMWMedia.MWDestroyD3DRenderer(m_hRender)
            m_hRender = IntPtr.Zero
        End If

        If (m_hAudioRender <> IntPtr.Zero) Then
            LibVBMWMedia.LibMWMedia.MWDestroyDSoundRenderer(m_hAudioRender)
            m_hAudioRender = IntPtr.Zero
        End If
    End Sub
End Class
