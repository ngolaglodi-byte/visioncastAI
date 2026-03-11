Imports System.Runtime.InteropServices
Imports LibVBMWCapture.LibMWCapture
Imports System.Timers
Imports System.Windows.Forms
Imports System.Threading

Public Class AVCaptureVB
    Protected m_nChannelCount As Integer
    Protected m_n_VChannelCount As Integer
    Protected m_hThread As New CaptureThread
    Protected m_MWcapFourcc As New LibVBMWCapture.MWcap_FOURCC
    Protected m_nIndex As Integer = 0
    Protected m_nWidth As Integer = 1920
    Protected m_nHeight As Integer = 1080
    Protected m_dwFourcc As UInteger = m_MWcapFourcc.MWCAP_FOURCC_YUY2
    Protected m_nFrameDuration As Integer = 400000
    Protected m_hWnd As Long = 0

    Protected m_hTimer As New System.Timers.Timer

    Protected Delegate Sub InvokeDelegate()

    Protected Sub InitMWCapture()
        LibVBMWCapture.LibMWCapture.MWCaptureInitInstance()
        LibVBMWCapture.LibMWCapture.MWRefreshDevice()

        m_nChannelCount = 0
        m_n_VChannelCount = 0
        m_nChannelCount = LibVBMWCapture.LibMWCapture.MWGetChannelCount()
        If (m_nChannelCount <= 0) Then
            ExitMWCapture()
            Me.Close()
        End If
    End Sub

    Protected Sub ExitMWCapture()
        LibVBMWCapture.LibMWCapture.MWCaptureExitInstance()
    End Sub

    Protected Function OnDeviceItemClicked(sender As Object, e As EventArgs) As Boolean
        Dim i As Integer
        For i = 0 To (m_n_VChannelCount - 1) Step 1
            Dim item1 As ToolStripMenuItem
            item1 = DeviceToolStripMenuItem.DropDownItems(i)
            item1.Checked = False
        Next

        Dim item As ToolStripMenuItem = CType(sender, ToolStripMenuItem)
        m_hThread.Destory()

        item.Checked = True
        m_nIndex = item.Tag
        m_hThread.Create(m_nIndex, m_nWidth, m_nHeight, m_dwFourcc, m_nFrameDuration, m_hWnd)
        Return True
    End Function

    Protected Function AddDeviceItem() As Boolean
        Dim mr As LibVBMWCapture.LibMWCapture.MW_RESULT = MW_RESULT.MW_SUCCEEDED
        Dim i As Integer
        Dim deviceName As String
        m_n_VChannelCount = 0
        For i = 0 To (m_nChannelCount - 1) Step 1
            Dim channelInfo As New LibVBMWCapture.LibMWCapture.MWCAP_CHANNEL_INFO
            mr = LibVBMWCapture.LibMWCapture.MWGetChannelInfoByIndex(i, channelInfo)
            If (channelInfo.szProductName.Contains("USB")) Then
                Continue For
            End If
            m_n_VChannelCount = m_n_VChannelCount + 1
            deviceName = channelInfo.byBoardIndex.ToString("00") & "-" & channelInfo.byChannelIndex.ToString() & " " & channelInfo.szProductName
            Dim item As ToolStripMenuItem
            item = DeviceToolStripMenuItem.DropDownItems.Add(deviceName, Nothing, AddressOf OnDeviceItemClicked)
            item.Tag = i
            If (i = 0) Then
                item.Checked = True
            Else
                item.Checked = False
            End If
        Next
        Return True
    End Function

    Protected Sub SetStatusBarInfo()
        Dim strFps As String
        strFps = "1920x1080 RGBA " + m_hThread.GetCurrentFPS()
        ToolStripStatusLabel1.Text = strFps
    End Sub

    Protected Sub OnTimeOut(sender As Object, e As System.Timers.ElapsedEventArgs)
        StatusStrip1.BeginInvoke(New InvokeDelegate(AddressOf SetStatusBarInfo))
    End Sub

    Protected Overrides Sub OnCreateControl()
        MyBase.OnCreateControl()

        InitMWCapture()
        If (m_nChannelCount <= 0) Then
            Return
        End If

        AddDeviceItem()

        PreviewWnd.Location = New Point(ClientRectangle.Left, ClientRectangle.Top + MenuStrip1.Height)
        PreviewWnd.Width = ClientRectangle.Width
        PreviewWnd.Height = ClientRectangle.Height - MenuStrip1.Height - StatusStrip1.Height

        m_nIndex = 0
        m_hWnd = PreviewWnd.Handle()
        m_hThread.Create(0, m_nWidth, m_nHeight, m_dwFourcc, m_nFrameDuration, m_hWnd)

        m_hTimer.Interval = 1000
        m_hTimer.AutoReset = True
        AddHandler m_hTimer.Elapsed, AddressOf OnTimeOut
        m_hTimer.Start()

        StatusBarToolStripMenuItem.Checked = True
        StatusStrip1.Visible = True

    End Sub

    Protected Overrides Sub OnClosed(e As EventArgs)
        MyBase.OnClosed(e)

        m_hTimer.Stop()
        m_hTimer.Close()

        m_hThread.Destory()
        ExitMWCapture()
    End Sub

    Protected Overrides Sub OnSizeChanged(e As EventArgs)
        MyBase.OnSizeChanged(e)

        PreviewWnd.Location = New Point(ClientRectangle.Left, ClientRectangle.Top + MenuStrip1.Height)
        PreviewWnd.Width = ClientRectangle.Width

        If (StatusStrip1.Visible = True) Then
            PreviewWnd.Height = ClientRectangle.Height - MenuStrip1.Height - StatusStrip1.Height
        Else
            PreviewWnd.Height = ClientRectangle.Height - MenuStrip1.Height
        End If
    End Sub

    Protected Overrides Sub OnLocationChanged(e As EventArgs)
        MyBase.OnLocationChanged(e)

        PreviewWnd.Location = New Point(ClientRectangle.Left, ClientRectangle.Top + MenuStrip1.Height)
        PreviewWnd.Width = ClientRectangle.Width

        If (StatusStrip1.Visible = True) Then
            PreviewWnd.Height = ClientRectangle.Height - MenuStrip1.Height - StatusStrip1.Height
        Else
            PreviewWnd.Height = ClientRectangle.Height - MenuStrip1.Height
        End If
    End Sub

    Private Sub ExitToolStripMenuItem_Click(sender As Object, e As EventArgs) Handles ExitToolStripMenuItem.Click
        Me.Close()
    End Sub

    Private Sub AboutAVCaptureVBToolStripMenuItem_Click(sender As Object, e As EventArgs) Handles AboutAVCaptureVBToolStripMenuItem.Click
        MsgBox("AVCapture,Version 1,0" & vbCrLf & "Copyright(C) 2011-2020,Nanjing Magewell Electronics Co., Ltd." & vbCrLf & "All rights reserved.", vbInformation)
    End Sub

    Private Sub StatusBarToolStripMenuItem_Click(sender As Object, e As EventArgs) Handles StatusBarToolStripMenuItem.Click
        StatusBarToolStripMenuItem.Checked = Not StatusBarToolStripMenuItem.Checked

        If (StatusStrip1.Visible = True) Then
            StatusStrip1.Visible = False
            PreviewWnd.Height = ClientRectangle.Height - MenuStrip1.Height
        Else
            StatusStrip1.Visible = True
            PreviewWnd.Height = ClientRectangle.Height - MenuStrip1.Height - StatusStrip1.Height
        End If
    End Sub
End Class
