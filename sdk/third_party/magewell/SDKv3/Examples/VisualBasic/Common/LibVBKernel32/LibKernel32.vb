Imports System
Imports System.Runtime.InteropServices
Imports System.Collections.Generic
Imports System.Linq
Imports System.Text
Public Class LibKernel32
    Public Const INFINITE As ULong = &HFFFFFFFF&

    <DllImport("kernel32.dll")>
    Public Shared Function CreateEvent(ByVal lpEventAttributes As IntPtr, ByVal bManualReset As Boolean, ByVal bInitialState As Boolean, ByVal lpName As IntPtr) As IntPtr
    End Function

    <DllImport("kernel32.dll")>
    Public Shared Function WaitForSingleObject(ByVal hHandle As IntPtr, ByVal dwMilliseconds As UInteger) As UInteger
    End Function

    <DllImport("kernel32.dll")>
    Public Shared Function WaitForMultipleObjects(ByVal nCount As UInteger, ByVal hHandles() As IntPtr, ByVal bWaitAll As Integer, ByVal dwMilliseconds As UInteger) As UInteger
    End Function

    <DllImport("kernel32.dll")>
    Public Shared Function CloseHandle(ByVal hObject As IntPtr) As Integer
    End Function

    <DllImport("kernel32.dll")>
    Public Shared Function SetEvent(ByVal hEvent As IntPtr) As Integer
    End Function

    <DllImport("kernel32.dll", CallingConvention:=CallingConvention.StdCall)>
    Public Shared Sub Sleep(ByVal dwMilliseconds As Integer)
    End Sub

    <DllImport("kernel32.dll")>
    Public Shared Function GetTickCount() As Long
    End Function
    <DllImport("kernel32.dll")>
    Public Shared Function DeleteFileA(ByVal lpFileName As String) As Long
    End Function
End Class
