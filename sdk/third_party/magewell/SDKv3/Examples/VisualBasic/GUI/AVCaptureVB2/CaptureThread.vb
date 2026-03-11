Imports System.Threading
Imports System
Imports System.Runtime.InteropServices
Imports System.Collections.Generic
Imports System.Linq
Imports System.Text
Imports LibVBMWCapture.LibMWCapture
Imports LibVBMWCapture.MWcap_FOURCC


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
    Protected m_nI As Integer

    Public m_pThis As IntPtr

    Protected Shared m_nfps As Single
    Protected Shared llCaptureCount As Integer
    Protected Shared m_strFps As String
    Protected Shared m_llCurrentTime As Long
    Protected Shared m_llRefTime As Long

    Dim video_callback As VIDEO_CAPTURE_STDCALL_CALLBACK
    Public Shared Sub video_callback_sub(pbFrame As IntPtr, cbFrame As Integer, u64TimeStamp As ULong, pParam As IntPtr)
        LibVBMWMedia.LibMWMedia.MWD3DRendererPushFrame(pParam, pbFrame, 720 * 4)
        llCaptureCount += 1

        If (llCaptureCount >= 10) Then
            m_llCurrentTime = LibVBKernel32.LibKernel32.GetTickCount()
            m_strFps = ((llCaptureCount * 1000) / (m_llCurrentTime - m_llRefTime)).ToString("0.00")
            m_llRefTime = m_llCurrentTime
            llCaptureCount = 0
        End If
    End Sub

    Dim audio_callback As AUDIO_CAPTURE_STDCALL_CALLBACK
    Public Shared Sub audio_callback_sub(pbFrame As IntPtr, vbFrame As Integer, u64TimeStamp As ULong, pParam As IntPtr)
        Dim asAudioSamples(vbFrame) As Byte
        Dim asAudio_gchandle As GCHandle = GCHandle.Alloc(asAudioSamples, GCHandleType.Pinned)
        Dim p_asAudioSamples As IntPtr = asAudio_gchandle.AddrOfPinnedObject()
        Dim bFrame(vbFrame) As Byte
        Marshal.Copy(pbFrame, asAudioSamples, 0, vbFrame)
        Dim ret As Integer = LibVBMWMedia.LibMWMedia.MWDSoundRendererPushFrame(pParam, p_asAudioSamples, vbFrame)
        asAudio_gchandle.Free()
    End Sub

    Protected m_ullStatusBits As ULong

    ''avcapture2
    ''video
    Protected m_hVideo As IntPtr
    Protected m_dwWidth As Integer = 1920
    Protected m_dwHeight As Integer = 1080
    Protected m_dwFourcc As UInteger
    Protected m_dwFrameDuration As Integer
    Protected m_hWnd As IntPtr = IntPtr.Zero
    Protected m_hRender As IntPtr

    ''audio
    Protected m_hAudio As IntPtr
    Protected m_hAudioRender As IntPtr
    Protected m_llAudioSample As ULong
    Protected m_dSamplePerSecond As Double
    Protected m_bAudioValid As Boolean
    Protected m_bLPCM As Byte
    Protected m_cBitsPerSample As Byte
    Protected m_dwSampleRate As Integer = 48000

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
        Dim wpath(512) As UShort
        Dim wpath_gchandle As GCHandle = GCHandle.Alloc(wpath, GCHandleType.Pinned)
        Dim pwpath As IntPtr = wpath_gchandle.AddrOfPinnedObject()
        MWGetDevicePath(m_nIndex, pwpath)
        'm_hChannel = LibVBMWCapture.LibMWCapture.MWOpenChannel(channelInfo.byBoardIndex, channelInfo.byChannelIndex)
        m_hChannel = LibVBMWCapture.LibMWCapture.MWOpenChannelByPath(pwpath)
        wpath_gchandle.Free()
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

        video_callback = New VIDEO_CAPTURE_STDCALL_CALLBACK(AddressOf video_callback_sub)
        audio_callback = New AUDIO_CAPTURE_STDCALL_CALLBACK(AddressOf audio_callback_sub)

        Dim t_b_reverse As Boolean
        Dim t_dw_fourcc As New LibVBMWCapture.MWcap_FOURCC
        t_b_reverse = False
        If (dwFourcc = t_dw_fourcc.MWCAP_FOURCC_BGRA Or dwFourcc = t_dw_fourcc.MWCAP_FOURCC_BGR24) Then
            t_b_reverse = True
        End If
        m_hRender = LibVBMWMedia.LibMWMedia.MWCreateD3DRenderer(m_dwWidth, m_dwHeight, dwFourcc, t_b_reverse, m_hWnd)
        If (m_hRender = IntPtr.Zero) Then
            Return False
        End If

        m_llCurrentTime = m_llRefTime = LibVBKernel32.LibKernel32.GetTickCount()
        llCaptureCount = 0
        m_hVideo = LibVBMWCapture.LibMWCapture.MWCreateVideoCaptureWithStdCallBack(m_hChannel, m_dwWidth, m_dwHeight, dwFourcc, m_dwFrameDuration, video_callback, m_hRender)
        If (m_hVideo = IntPtr.Zero) Then
            Return False
        End If

        m_hAudioRender = LibVBMWMedia.LibMWMedia.MWCreateDSoundRenderer(m_dwSampleRate, 2, 480, 10)
        If (m_hAudioRender = IntPtr.Zero) Then
            Return False
        End If
        m_hAudio = MWCreateAudioCaptureWithStdCallBack(m_hChannel, MWCAP_AUDIO_CAPTURE_NODE.MWCAP_AUDIO_CAPTURE_NODE_DEFAULT, m_dwSampleRate, 16, 2, audio_callback, m_hAudioRender)
        If (m_hAudio = IntPtr.Zero) Then
            Return False
        End If


        Return True
    End Function

    Public Sub Destory()
        If (m_hVideo <> IntPtr.Zero) Then
            MWDestoryVideoCapture(m_hVideo)
            m_hVideo = IntPtr.Zero
        End If

        If (m_hAudio <> IntPtr.Zero) Then
            MWDestoryAudioCapture(m_hAudio)
            m_hAudio = IntPtr.Zero
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

