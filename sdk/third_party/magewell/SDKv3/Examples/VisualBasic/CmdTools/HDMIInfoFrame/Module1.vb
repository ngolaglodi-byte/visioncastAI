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
        Console.Write("Magewell MWCapture SDK {0}.{1}.1.{2} - HDMIInfoFrameVB" + vbCrLf, byMaj, byMin, wBuild)
        Console.Write("Devices with HDMI interface are supported." + vbCrLf)
        Console.Write("Usage:" + vbCrLf)
        Console.Write("HDMIInfoFrameVB.exe <channel index>" + vbCrLf)
        Console.Write("Pro and Eco capture serial devices: HDMIInfoFrame.exe <board id>:<channel id>" + vbCrLf)

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

        Dim info As MWCAP_CHANNEL_INFO
        Dim mr As MW_RESULT
        For i As Integer = 0 To nChannelCount - 1
            mr = MWGetChannelInfoByIndex(i, info)
            If mr <> MW_RESULT.MW_SUCCEEDED Then
                Continue For
            End If
            Dim path(256) As Integer
            Dim path_gchandle As GCHandle = GCHandle.Alloc(path, GCHandleType.Pinned)
            Dim p_path As IntPtr = path_gchandle.AddrOfPinnedObject()
            mr = MWGetDevicePath(i, p_path)
            If mr <> MW_RESULT.MW_SUCCEEDED Then
                path_gchandle.Free()
                Continue For
            End If
            hChannel = MWOpenChannelByPath(p_path)
            path_gchandle.Free()

            Dim t_dw_count As Integer = 0
            mr = MWGetVideoInputSourceArray(hChannel, IntPtr.Zero, t_dw_count)
            If mr <> MW_RESULT.MW_SUCCEEDED Then
                MWCloseChannel(hChannel)
                Continue For
            End If

            Dim t_arr_source(16) As Integer
            Dim arr_source_gchandle As GCHandle = GCHandle.Alloc(t_arr_source, GCHandleType.Pinned)
            Dim p_arr_source As IntPtr = arr_source_gchandle.AddrOfPinnedObject()
            mr = MWGetVideoInputSourceArray(hChannel, p_arr_source, t_dw_count)
            If mr <> MW_RESULT.MW_SUCCEEDED Then
                arr_source_gchandle.Free()
                MWCloseChannel(hChannel)
                Continue For
            End If
            arr_source_gchandle.Free()
            MWCloseChannel(hChannel)
            For j As Integer = 0 To t_dw_count - 1
                Dim t_input_type As MWCAP_VIDEO_INPUT_TYPE = t_arr_source(j) >> 8
                If t_input_type = MWCAP_VIDEO_INPUT_TYPE.MWCAP_VIDEO_INPUT_TYPE_HDMI Then
                    arrProDevChannel(i) = i
                End If
            Next
        Next

        If nChannelCount < 2 Then
            Console.Write("Find {0} channel." + vbCrLf, nChannelCount)
        Else
            Console.Write("Find {0} channels." + vbCrLf, nChannelCount)
        End If
        Return True
    End Function
    Function OpenChannel(ByVal cmdargs() As String)
        Dim t_n_dev_index As Integer = -1
        For i As Integer = 0 To nChannelCount - 1
            If arrProDevChannel(i) <> -1 Then
                t_n_dev_index = arrProDevChannel(i)
                Exit For
            End If
        Next
        If t_n_dev_index = -1 Then
            Console.Write("ERROR: Can't find Devices with HDMI interface!" + vbCrLf)
            Return False
        End If

        Dim byBoardId As Integer = -1
        Dim byChannelId As Integer = -1
        Dim nDeviceIndex As Integer = -1
        Dim bIndex As Boolean = False

        Dim videoInfo As New MWCAP_CHANNEL_INFO

        If (cmdargs.Length() = 0) Then
            bIndex = True
            nDeviceIndex = t_n_dev_index
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

        If (MW_RESULT.MW_SUCCEEDED <> MWGetChannelInfo(hChannel, videoInfo)) Then
            Console.Write("ERROR: Can't get channel info!" + vbCrLf)
            Return False
        End If

        Console.Write("Open channel - BoardIndex = {0:x}, ChannelIndex = {1}." + vbCrLf, videoInfo.byBoardIndex, videoInfo.byChannelIndex)
        Console.Write("Product Name: {0}" + vbCrLf, videoInfo.szProductName)
        Console.Write("Board SerialNo: {0}" + vbCrLf + vbCrLf, videoInfo.szBoardSerialNo)
        Return True
    End Function
    Sub PrintHDMIInfo()
        Dim t_ret As MW_RESULT = MW_RESULT.MW_FAILED
        Dim status As MWCAP_INPUT_SPECIFIC_STATUS
        t_ret = MWGetInputSpecificStatus(hChannel, status)
        If (t_ret = MW_RESULT.MW_SUCCEEDED) Then
            If (status.bValid <> 1) Then
                Console.Write("ERROR: Input signal is invalid!" + vbCrLf)
            ElseIf (status.dwVideoInputType <> 1) Then
                Console.Write("ERROR: Input signal is not HDMI!" + vbCrLf)
            Else
                Dim dwValidFlag As Integer = 0
                t_ret = MWGetHDMIInfoFrameValidFlag(hChannel, dwValidFlag)
                If (t_ret = MW_RESULT.MW_SUCCEEDED) Then
                    If (dwValidFlag = 0) Then
                        Console.Write("No HDMI InfoFrame!" + vbCrLf)
                    Else
                        Dim packet As HDMI_INFOFRAME_PACKET
                        If (dwValidFlag And MWCAP_HDMI_INFOFRAME_MASK.MWCAP_HDMI_INFOFRAME_MASK_AVI) Then
                            t_ret = MWGetHDMIInfoFramePacket(hChannel, MWCAP_HDMI_INFOFRAME_ID.MWCAP_HDMI_INFOFRAME_ID_AVI, packet)
                            If (t_ret = MW_RESULT.MW_SUCCEEDED) Then
                                Console.Write("Get HDMI InfoFrame AVI OK!" + vbCrLf)
                                Console.Write("Length = {0}, Buffer is ", packet.header.byLength)
                                For i As Integer = 0 To (packet.header.byLength - 1) Step 1
                                    Console.Write("{0:x2} ", packet.union.abyPayload(i))
                                Next
                                Console.Write(vbCrLf + vbCrLf)
                            End If
                        End If
                        If (dwValidFlag And MWCAP_HDMI_INFOFRAME_MASK.MWCAP_HDMI_INFOFRAME_MASK_AUDIO) Then
                            t_ret = MWGetHDMIInfoFramePacket(hChannel, MWCAP_HDMI_INFOFRAME_ID.MWCAP_HDMI_INFOFRAME_ID_AUDIO, packet)
                            If (t_ret = MW_RESULT.MW_SUCCEEDED) Then
                                Console.Write("Get HDMI InfoFrame Audio OK!" + vbCrLf)
                                Console.Write("Length = {0}, Buffer is ", packet.header.byLength)
                                For i As Integer = 0 To (packet.header.byLength - 1) Step 1
                                    Console.Write("{0:x2} ", packet.union.abyPayload(i))
                                Next
                                Console.Write(vbCrLf + vbCrLf)
                            End If
                        End If
                        If (dwValidFlag And MWCAP_HDMI_INFOFRAME_MASK.MWCAP_HDMI_INFOFRAME_MASK_SPD) Then
                            t_ret = MWGetHDMIInfoFramePacket(hChannel, MWCAP_HDMI_INFOFRAME_ID.MWCAP_HDMI_INFOFRAME_ID_SPD, packet)
                            If (t_ret = MW_RESULT.MW_SUCCEEDED) Then
                                Console.Write("Get HDMI InfoFrame SPD OK!" + vbCrLf)
                                Console.Write("Length = {0}, Buffer is ", packet.header.byLength)
                                For i As Integer = 0 To (packet.header.byLength - 1) Step 1
                                    Console.Write("{0:x2} ", packet.union.abyPayload(i))
                                Next
                                Console.Write(vbCrLf + vbCrLf)
                            End If
                        End If
                        If (dwValidFlag And MWCAP_HDMI_INFOFRAME_MASK.MWCAP_HDMI_INFOFRAME_MASK_MS) Then
                            t_ret = MWGetHDMIInfoFramePacket(hChannel, MWCAP_HDMI_INFOFRAME_ID.MWCAP_HDMI_INFOFRAME_ID_MS, packet)
                            If (t_ret = MW_RESULT.MW_SUCCEEDED) Then
                                Console.Write("Get HDMI InfoFrame MS OK!" + vbCrLf)
                                Console.Write("Length = {0}, Buffer is ", packet.header.byLength)
                                For i As Integer = 0 To (packet.header.byLength - 1) Step 1
                                    Console.Write("{0:x2} ", packet.union.abyPayload(i))
                                Next
                                Console.Write(vbCrLf + vbCrLf)
                            End If
                        End If
                        If (dwValidFlag And MWCAP_HDMI_INFOFRAME_MASK.MWCAP_HDMI_INFOFRAME_MASK_VS) Then
                            t_ret = MWGetHDMIInfoFramePacket(hChannel, MWCAP_HDMI_INFOFRAME_ID.MWCAP_HDMI_INFOFRAME_ID_VS, packet)
                            If (t_ret = MW_RESULT.MW_SUCCEEDED) Then
                                Console.Write("Get HDMI InfoFrame VS OK!" + vbCrLf)
                                Console.Write("Length = {0}, Buffer is ", packet.header.byLength)
                                For i As Integer = 0 To (packet.header.byLength - 1) Step 1
                                    Console.Write("{0:x2} ", packet.union.abyPayload(i))
                                Next
                                Console.Write(vbCrLf + vbCrLf)
                            End If
                        End If
                        If (dwValidFlag And MWCAP_HDMI_INFOFRAME_MASK.MWCAP_HDMI_INFOFRAME_MASK_ACP) Then
                            t_ret = MWGetHDMIInfoFramePacket(hChannel, MWCAP_HDMI_INFOFRAME_ID.MWCAP_HDMI_INFOFRAME_ID_ACP, packet)
                            If (t_ret = MW_RESULT.MW_SUCCEEDED) Then
                                Console.Write("Get HDMI InfoFrame ACP OK!" + vbCrLf)
                                Console.Write("Length = {0}, Buffer is ", packet.header.byLength)
                                For i As Integer = 0 To (packet.header.byLength - 1) Step 1
                                    Console.Write("{0:x2} ", packet.union.abyPayload(i))
                                Next
                                Console.Write(vbCrLf + vbCrLf)
                            End If
                        End If
                        If (dwValidFlag And MWCAP_HDMI_INFOFRAME_MASK.MWCAP_HDMI_INFOFRAME_MASK_ISRC1) Then
                            t_ret = MWGetHDMIInfoFramePacket(hChannel, MWCAP_HDMI_INFOFRAME_ID.MWCAP_HDMI_INFOFRAME_ID_ISRC1, packet)
                            If (t_ret = MW_RESULT.MW_SUCCEEDED) Then
                                Console.Write("Get HDMI InfoFrame ISRC1 OK!" + vbCrLf)
                                Console.Write("Length = {0}, Buffer is ", packet.header.byLength)
                                For i As Integer = 0 To (packet.header.byLength - 1) Step 1
                                    Console.Write("{0:x2} ", packet.union.abyPayload(i))
                                Next
                                Console.Write(vbCrLf + vbCrLf)
                            End If
                        End If
                        If (dwValidFlag And MWCAP_HDMI_INFOFRAME_MASK.MWCAP_HDMI_INFOFRAME_MASK_ISRC2) Then
                            t_ret = MWGetHDMIInfoFramePacket(hChannel, MWCAP_HDMI_INFOFRAME_ID.MWCAP_HDMI_INFOFRAME_ID_ISRC2, packet)
                            If (t_ret = MW_RESULT.MW_SUCCEEDED) Then
                                Console.Write("Get HDMI InfoFrame ISRC2 OK!" + vbCrLf)
                                Console.Write("Length = {0}, Buffer is ", packet.header.byLength)
                                For i As Integer = 0 To (packet.header.byLength - 1) Step 1
                                    Console.Write("{0:x2} ", packet.union.abyPayload(i))
                                Next
                                Console.Write(vbCrLf + vbCrLf)
                            End If
                        End If
                        If (dwValidFlag And MWCAP_HDMI_INFOFRAME_MASK.MWCAP_HDMI_INFOFRAME_MASK_GAMUT) Then
                            t_ret = MWGetHDMIInfoFramePacket(hChannel, MWCAP_HDMI_INFOFRAME_ID.MWCAP_HDMI_INFOFRAME_ID_GAMUT, packet)
                            If (t_ret = MW_RESULT.MW_SUCCEEDED) Then
                                Console.Write("Get HDMI InfoFrame GAMUT OK!" + vbCrLf)
                                Console.Write("Length = {0}, Buffer is ", packet.header.byLength)
                                For i As Integer = 0 To (packet.header.byLength - 1) Step 1
                                    Console.Write("{0:x2} ", packet.union.abyPayload(i))
                                Next
                                Console.Write(vbCrLf + vbCrLf)
                            End If
                        End If
                        If (dwValidFlag And MWCAP_HDMI_INFOFRAME_MASK.MWCAP_HDMI_INFOFRAME_MASK_VBI) Then
                            t_ret = MWGetHDMIInfoFramePacket(hChannel, MWCAP_HDMI_INFOFRAME_ID.MWCAP_HDMI_INFOFRAME_ID_VBI, packet)
                            If (t_ret = MW_RESULT.MW_SUCCEEDED) Then
                                Console.Write("Get HDMI InfoFrame VBI OK!" + vbCrLf)
                                Console.Write("Length = {0}, Buffer is ", packet.header.byLength)
                                For i As Integer = 0 To (packet.header.byLength - 1) Step 1
                                    Console.Write("{0:x2} ", packet.union.abyPayload(i))
                                Next
                                Console.Write(vbCrLf + vbCrLf)
                            End If
                        End If
                        If (dwValidFlag And MWCAP_HDMI_INFOFRAME_MASK.MWCAP_HDMI_INFOFRAME_MASK_HDR) Then
                            t_ret = MWGetHDMIInfoFramePacket(hChannel, MWCAP_HDMI_INFOFRAME_ID.MWCAP_HDMI_INFOFRAME_ID_HDR, packet)
                            If (t_ret = MW_RESULT.MW_SUCCEEDED) Then
                                Console.Write("Get HDMI InfoFrame HDR OK!" + vbCrLf)
                                Console.Write("Length = {0}, Buffer is ", packet.header.byLength)
                                For i As Integer = 0 To (packet.header.byLength - 1) Step 1
                                    Console.Write("{0:x2} ", packet.union.abyPayload(i))
                                Next
                                Console.Write(vbCrLf + vbCrLf)
                            End If
                        End If
                    End If
                Else
                    Console.Write("ERROR: Get HDMI InfoFrame Flag!" + vbCrLf)
                End If
            End If
        Else
            Console.Write("ERROR: Get Specific Status error!" + vbCrLf)
        End If
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

        PrintHDMIInfo()

        CloseDevice()

        ExitProgram()
    End Sub

End Module
