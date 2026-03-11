Imports System.Runtime.InteropServices
Imports System.Collections.Generic
Imports System.Linq
Imports System.Text
Public Class LibMWMedia
#If DEBUG Then
    <DllImport("LibMWMediad.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWCreateD3DRenderer(ByVal cx As Integer, ByVal cy As Integer, ByVal dwFourcc As UInteger, ByVal bReverse As Boolean, ByVal hWnd As IntPtr) As IntPtr
    End Function
#Else
    <DllImport("LibMWMedia.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWCreateD3DRenderer(ByVal cx As Integer, ByVal cy As Integer, ByVal dwFourcc As UInteger,ByVal bReverse As Boolean, ByVal hWnd As IntPtr) As IntPtr
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWMediad.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Sub MWDestroyD3DRenderer(ByVal hRenderer As Long)
    End Sub
#Else
    <DllImport("LibMWMedia.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Sub MWDestroyD3DRenderer(ByVal hRenderer As Long)
    End Sub
#End If

#If DEBUG Then
    <DllImport("LibMWMediad.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWD3DRendererPushFrame(ByVal hRenderer As IntPtr, ByVal pbyBuffer As IntPtr, ByVal cbStride As Integer) As Integer
    End Function
#Else
    <DllImport("LibMWMedia.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWD3DRendererPushFrame(ByVal hRenderer As IntPtr, ByVal pbyBuffer As IntPtr, ByVal cbStride As Integer) As Integer
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWMediad.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWCreateDSoundRenderer(ByVal nSamplesPerSec As Integer, ByVal nChannels As Integer, ByVal nSamplesPerFrame As Integer, ByVal nBufferFrameCount As Integer) As IntPtr
    End Function
#Else
    <DllImport("LibMWMedia.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWCreateDSoundRenderer(ByVal nSamplesPerSec As Integer, ByVal nChannels As Integer, ByVal nSamplesPerFrame As Integer, ByVal nBufferFrameCount As Integer) As IntPtr
    End Function
#End If

#If DEBUG Then
    <DllImport("LibMWMediad.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Sub MWDestroyDSoundRenderer(ByVal hRenderer As Long)
    End Sub
#Else
    <DllImport("LibMWMedia.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Sub MWDestroyDSoundRenderer(ByVal hRenderer As Long)
    End Sub
#End If

#If DEBUG Then
    <DllImport("LibMWMediad.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWDSoundRendererPushFrame(ByVal hRenderer As IntPtr, ByVal pbFrame As IntPtr, ByVal cbFram As Integer) As Integer
    End Function
#Else
    <DllImport("LibMWMedia.dll", CallingConvention:=CallingConvention.Cdecl)> Public Shared Function MWDSoundRendererPushFrame(ByVal hRenderer As IntPtr, ByVal pbFrame As IntPtr, ByVal cbFram As Integer) As Integer
    End Function
#End If


End Class
