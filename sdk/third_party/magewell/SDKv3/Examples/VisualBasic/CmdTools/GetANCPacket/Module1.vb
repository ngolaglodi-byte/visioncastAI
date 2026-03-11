Imports LibVBMWCapture.LibMWCapture
Imports LibVBKernel32.LibKernel32
Imports System.Runtime.InteropServices
Imports System.Threading
Imports System.IO

Module Module1
    Private Declare Function PathFileExists Lib "shlwapi.dll" Alias "PathFileExistsA" (ByVal pszPath As String) As Long

    Dim hChannel As IntPtr
    Dim hExitThreadEvent As IntPtr
    Dim hGetInputThreadExited As IntPtr
    Dim hGetANCPacketThreadExited As IntPtr
    Dim hGetANCThread As Threading.Thread
    Dim hGetInputThread As Threading.Thread
    Dim arrProDevChannel(10) As Integer
    Dim nChannelCount As Integer
    Sub ShowInfo()
        Dim byMaj As Byte, byMin As Byte
        Dim wBuild As Integer
        MWGetVersion(byMaj, byMin, wBuild)
        Console.Write("Magewell MWCapture SDK {0}.{1}.1.{2} - GetANCPacketVB" + vbCrLf, byMaj, byMin, wBuild)
        Console.Write("Devices with SDI interface are supported" + vbCrLf)
        Console.Write(" " + vbCrLf)
        Console.Write("Usage:" + vbCrLf)
        Console.Write("GetANCPacketVB.exe <channel index>" + vbCrLf)
        Console.Write("GetANCPacketVB.exe <board id>:<channel id>" + vbCrLf)
    End Sub
    Sub Initialize()
        hChannel = IntPtr.Zero
        hExitThreadEvent = IntPtr.Zero
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
        Dim filename = "\anc_data.bin"
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
            If InStr(info.szProductName, "USB") > 0 Then
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
                If t_input_type = MWCAP_VIDEO_INPUT_TYPE.MWCAP_VIDEO_INPUT_TYPE_SDI Then
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
    Function OpenChannel(ByVal cmdargs() As String) As Boolean
        Dim t_n_dev_index As Integer = -1
        For i As Integer = 0 To nChannelCount - 1
            If arrProDevChannel(i) <> -1 Then
                t_n_dev_index = arrProDevChannel(i)
                Exit For
            End If
        Next

        If t_n_dev_index = -1 Then
            Console.Write("ERROR: Can't find Devices with SDI interface!" + vbCrLf)
            Return False
        End If

        Dim byBoardId As Integer = -1
        Dim byChannelId As Integer = -1
        Dim nDeviceIndex As Integer = -1
        Dim bIndex As Boolean = False

        Dim channelInfo As New MWCAP_CHANNEL_INFO

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

        If (MW_RESULT.MW_SUCCEEDED <> MWGetChannelInfo(hChannel, channelInfo)) Then
            Console.Write("ERROR: Can't get channel info!" + vbCrLf)
            Return False
        End If

        Console.Write("Open channel - BoardIndex = {0:x}, ChannelIndex = {1}." + vbCrLf, channelInfo.byBoardIndex, channelInfo.byChannelIndex)
        Console.Write("Product Name:{0}" + vbCrLf, channelInfo.szProductName)
        Console.Write("Board SerialNo:{0}" + vbCrLf + vbCrLf, channelInfo.szBoardSerialNo)

        Return True
    End Function
    Private Sub GetANCPacketThread()

        For i As Integer = 0 To 3 Step 1
            MWCaptureSetSDIANCType(hChannel, i, 0, 0, &H0, &H0)
        Next

        Dim ret As MW_RESULT
        ret = MWCaptureSetSDIANCType(hChannel, 0, 0, 1, &H61, &H1)
        ret = MWCaptureSetSDIANCType(hChannel, 1, 0, 1, &H61, &H2)

        Dim ancPacket As MWCAP_SDI_ANC_PACKET
        Do
            ret = MWCaptureGetSDIANCPacket(hChannel, ancPacket)
        Loop While ((ret = MW_RESULT.MW_SUCCEEDED) And (ancPacket.byDID <> 0))

        Dim hNotifyEvent As IntPtr = CreateEvent(IntPtr.Zero, False, False, IntPtr.Zero)
        Dim hNotify As ULong = MWRegisterNotify(hChannel, hNotifyEvent, MWCAP_NOTIFY_NEW_SDI_ANC_PACKET)

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
        Dim filename = "\anc_data.bin"
        Dim filePath As String
        filePath = folderpath + filename
        Dim fs As New FileStream(filePath, FileMode.Create, FileAccess.ReadWrite)
        Dim bw As New System.IO.BinaryWriter(fs)
        Console.Write(vbCrLf + "The file is saved in {0}." + vbCrLf + vbCrLf, filePath)

        Dim bytes(1) As Byte
        Dim t_arr_event(2) As IntPtr
        t_arr_event(0) = hNotifyEvent
        t_arr_event(1) = hExitThreadEvent

        For i As Integer = 0 To 599 Step 1
            Dim t_dw_ret As UInteger
            t_dw_ret = WaitForMultipleObjects(2, t_arr_event, 0, INFINITE)
            If t_dw_ret <> 0 Then
                Exit For
            End If

            Dim ullStatusBits As ULong = MWCAP_NOTIFY_NEW_SDI_ANC_PACKET
            ret = MWGetNotifyStatus(hChannel, hNotify, ullStatusBits)
            If (ret <> MW_RESULT.MW_SUCCEEDED) Then
                Continue For
            End If

            If (ullStatusBits And MWCAP_NOTIFY_NEW_SDI_ANC_PACKET) Then

                Do
                    ret = MWCaptureGetSDIANCPacket(hChannel, ancPacket)
                    If ((ret = MW_RESULT.MW_SUCCEEDED) And (ancPacket.byDID <> 0)) Then
                        StructToByte(ancPacket, bytes)
                        bw.Write(bytes, 0, ancPacket.byDC + 3)
                        Console.Write("GetSDIANCPacket[{0}], DID[0x{1:x}], SDID[0x{2:x}], Length[{3}]" + vbCrLf, i, ancPacket.byDID, ancPacket.bySDID, ancPacket.byDC)
                    End If
                Loop While ((ret = MW_RESULT.MW_SUCCEEDED) And (ancPacket.byDID <> 0))
            End If
        Next

        bw.Close()
        MWUnregisterNotify(hChannel, hNotify)
        CloseHandle(hNotifyEvent)

        SetEvent(hGetANCPacketThreadExited)
    End Sub
    Private Sub GetInputThread()
        Console.Write("Press ""Enter"" key to exit geting ANC." + vbCrLf)
        While Console.ReadKey().KeyChar = vbCr
            Exit While
        End While

        SetEvent(hGetInputThreadExited)
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
    Private Sub StructToByte(ByVal objStruct As Object, ByRef bytes() As Byte)
        Dim size As Integer = Marshal.SizeOf(objStruct)
        ReDim bytes(size - 1) ''size should minus 1,or it would be get 1 more byte
        Dim ptrStruct As IntPtr = Marshal.AllocHGlobal(size - 1)
        Marshal.StructureToPtr(objStruct, ptrStruct, False)
        Marshal.Copy(ptrStruct, bytes, 0, size)
        Marshal.FreeHGlobal(ptrStruct)
    End Sub
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

        hExitThreadEvent = CreateEvent(IntPtr.Zero, False, False, IntPtr.Zero)
        hGetInputThreadExited = CreateEvent(IntPtr.Zero, False, False, IntPtr.Zero)
        hGetANCPacketThreadExited = CreateEvent(IntPtr.Zero, False, False, IntPtr.Zero)
        hGetANCThread = New Thread(AddressOf GetANCPacketThread)
        hGetInputThread = New Thread(AddressOf GetInputThread)

        hGetANCThread.Start()
        hGetInputThread.Start()

        Dim t_array_thread(2) As IntPtr
        t_array_thread(0) = hGetANCPacketThreadExited
        t_array_thread(1) = hGetInputThreadExited
        Dim t_dw_ret As Integer = -1
        t_dw_ret = WaitForMultipleObjects(2, t_array_thread, 0, INFINITE)
        If t_dw_ret = 0 Then
            hGetInputThread.Abort()
        ElseIf t_dw_ret = 1 Then
            SetEvent(hExitThreadEvent)
            hGetANCThread.Join()
        End If

        CloseDevice()
        ExitProgram()
    End Sub

End Module
