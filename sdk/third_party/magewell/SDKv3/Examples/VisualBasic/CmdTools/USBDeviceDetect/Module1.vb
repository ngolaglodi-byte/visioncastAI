Imports LibVBMWCapture.LibMWCapture
Imports LibVBKernel32.LibKernel32
Imports System.Runtime.InteropServices

Module Module1
    Dim hChannel As IntPtr
    Dim nChannelCount As Integer
    Sub ShowInfo()
        Dim byMaj As Byte, byMin As Byte
        Dim wBuild As Integer
        MWGetVersion(byMaj, byMin, wBuild)
        Console.Write("Magewell MWCapture SDK {0}.{1}.1.{2} - USBDeviceDetectVB" + vbCrLf, byMaj, byMin, wBuild)
        Console.Write("Only USB devices are supported" + vbCrLf)
        Console.Write("Usage:" + vbCrLf)
        Console.Write("USBDeviceDetectVB.exe" + vbCrLf)
    End Sub
    Sub Initialize()
        hChannel = IntPtr.Zero
        nChannelCount = 0
        MWCaptureInitInstance()
    End Sub
    Sub USBDeviceDetect()
        Dim mr As MW_RESULT = MW_RESULT.MW_FAILED
        Dim HotplugCheckCallback As LPFN_HOT_PLUG_STDCALL_CALLBACK = Sub(usb_event As MWUSBHOT_PLUG_EVETN, pszDevicePath As IntPtr, param As IntPtr)
                                                                         Select Case (usb_event)
                                                                             Case MWUSBHOT_PLUG_EVETN.USBHOT_PLUG_EVENT_DEVICE_ARRIVED
                                                                                 Console.Write("Device reconnect" + vbCrLf)
                                                                             Case MWUSBHOT_PLUG_EVETN.USBHOT_PLUG_EVENT_DEVICE_LEFT
                                                                                 Console.Write("Device disconnect" + vbCrLf)
                                                                         End Select
                                                                     End Sub
        mr = MWUSBRegisterHotPlugWithStdCallBack(HotplugCheckCallback, IntPtr.Zero)
        If (mr <> MW_RESULT.MW_SUCCEEDED) Then
            Console.Write("Set usb device detect event failed" + vbCrLf)
            Return
        End If

        Console.Write("Please disconnect and reconnect the specific usb device" + vbCrLf)
        Console.Write("It will listen for USB devices connection change events for 10 seconds." + vbCrLf)
        Dim time As Integer = 1000
        For i As Integer = 0 To 9
            Console.Write("{0}s" + vbCrLf, i)
            Sleep(time)
        Next
        mr = MWUSBUnRegisterHotPlug()
    End Sub
    Sub ExitProgram()
        Console.Write("Press ""Enter"" key to exit..." + vbCrLf)
        Console.ReadKey()
    End Sub

    Sub Main(ByVal cmdargs() As String)
        ShowInfo()

        Initialize()

        USBDeviceDetect()

        ExitProgram()
    End Sub

End Module
