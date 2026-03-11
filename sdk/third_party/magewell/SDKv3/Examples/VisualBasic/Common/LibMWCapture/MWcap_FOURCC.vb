Imports System
Imports System.Runtime.InteropServices
Imports System.Collections.Generic
Imports System.Linq
Imports System.Text
Public Class MWcap_FOURCC


    Public MWCAP_FOURCC_UNK As UInteger = MWcap_FOURCC("U", "N", "K", "N")
    Public MWCAP_FOURCC_GREY As UInteger = MWcap_FOURCC("G", "R", "E", "Y")
    Public MWCAP_FOURCC_Y800 As UInteger = MWcap_FOURCC("Y", "8", "0", "0")
    Public MWCAP_FOURCC_Y8 As UInteger = MWcap_FOURCC("Y", "8", " ", " ")


    Public MWCAP_FOURCC_Y16 As UInteger = MWcap_FOURCC("Y", "1", "6", " ")

    Public MWCAP_FOURCC_RGB15 As UInteger = MWcap_FOURCC("R", "G", "B", "5")
    Public MWCAP_FOURCC_RGB16 As UInteger = MWcap_FOURCC("R", "G", "B", "6")
    Public MWCAP_FOURCC_RGB24 As UInteger = MWcap_FOURCC("R", "G", "B", " ")
    Public MWCAP_FOURCC_RGBA As UInteger = MWcap_FOURCC("R", "G", "B", "A")
    Public MWCAP_FOURCC_ARGB As UInteger = MWcap_FOURCC("A", "R", "G", "B")

    Public MWCAP_FOURCC_BGR15 As UInteger = MWcap_FOURCC("B", "G", "R", "5")
    Public MWCAP_FOURCC_BGR16 As UInteger = MWcap_FOURCC("B", "G", "R", "6")
    Public MWCAP_FOURCC_BGR24 As UInteger = MWcap_FOURCC("B", "G", "R", " ")
    Public MWCAP_FOURCC_BGRA As UInteger = MWcap_FOURCC("B", "G", "R", "A")
    Public MWCAP_FOURCC_ABGR As UInteger = MWcap_FOURCC("A", "B", "G", "R")

    Public MWCAP_FOURCC_YUY2 As UInteger = MWcap_FOURCC("Y", "U", "Y", "2")
    Public MWCAP_FOURCC_YUYV As UInteger = MWcap_FOURCC("Y", "U", "Y", "V")
    Public MWCAP_FOURCC_UYVY As UInteger = MWcap_FOURCC("U", "Y", "V", "Y")

    Public MWCAP_FOURCC_YVYU As UInteger = MWcap_FOURCC("Y", "V", "Y", "U")
    Public MWCAP_FOURCC_VYUY As UInteger = MWcap_FOURCC("V", "Y", "U", "Y")

    Public MWCAP_FOURCC_I420 As UInteger = MWcap_FOURCC("I", "4", "2", "0")
    Public MWCAP_FOURCC_IYUV As UInteger = MWcap_FOURCC("I", "Y", "U", "V")
    Public MWCAP_FOURCC_NV12 As UInteger = MWcap_FOURCC("N", "V", "1", "2")

    Public MWCAP_FOURCC_YV12 As UInteger = MWcap_FOURCC("Y", "V", "1", "2")
    Public MWCAP_FOURCC_NV21 As UInteger = MWcap_FOURCC("N", "V", "2", "1")

    Public MWCAP_FOURCC_IYU2 As UInteger = MWcap_FOURCC("I", "Y", "U", "2")
    Public MWCAP_FOURCC_V308 As UInteger = MWcap_FOURCC("v", "3", "0", "8")

    Public MWCAP_FOURCC_AYUV As UInteger = MWcap_FOURCC("A", "Y", "U", "V")
    Public MWCAP_FOURCC_UYVA As UInteger = MWcap_FOURCC("U", "Y", "V", "A")
    Public MWCAP_FOURCC_V408 As UInteger = MWcap_FOURCC("v", "4", "0", "8")
    Public MWCAP_FOURCC_VYUA As UInteger = MWcap_FOURCC("V", "Y", "U", "A")

    Public MWCAP_FOURCC_Y410 As UInteger = MWcap_FOURCC("Y", "4", "1", "0")
    Public MWCAP_FOURCC_V410 As UInteger = MWcap_FOURCC("v", "4", "1", "0")

    Public MWCAP_FOURCC_RGB10 As UInteger = MWcap_FOURCC("R", "G", "1", "0")
    Public MWCAP_FOURCC_BGR10 As UInteger = MWcap_FOURCC("B", "G", "1", "0")



    Protected Function MWCAP_FOURCC(ByVal ch0 As Char, ByVal ch1 As Char, ByVal ch2 As Char, ByVal ch3 As Char) As UInteger
        Dim dwFourcc As UInteger
        Dim aa As Integer = AscW(ch0)
        Dim bb As Integer = (AscW(ch1) << 8)
        Dim cc As Integer = (AscW(ch2) << 16)
        Dim ee As Integer = (AscW(ch3))
        Dim dd As Integer = (AscW(ch3) << 24)
        dwFourcc = AscW(ch0) Or (AscW(ch1) << 8) Or (AscW(ch2) << 16) Or (AscW(ch3) << 24)
        Return dwFourcc
    End Function

    Protected Function FOURCC_IsRGB(ByVal dwFourcc As UInteger) As Byte
        If (dwFourcc = MWCAP_FOURCC_RGB15 Or
            dwFourcc = MWCAP_FOURCC_BGR15 Or
            dwFourcc = MWCAP_FOURCC_RGB16 Or
            dwFourcc = MWCAP_FOURCC_BGR16 Or
            dwFourcc = MWCAP_FOURCC_RGB24 Or
            dwFourcc = MWCAP_FOURCC_BGR24 Or
            dwFourcc = MWCAP_FOURCC_RGBA Or
            dwFourcc = MWCAP_FOURCC_BGRA Or
            dwFourcc = MWCAP_FOURCC_ARGB Or
            dwFourcc = MWCAP_FOURCC_ABGR Or
            dwFourcc = MWCAP_FOURCC_RGB10 Or
            dwFourcc = MWCAP_FOURCC_BGR10) Then
            Return True
        Else
            Return False
        End If
    End Function

    Protected Function FOURCC_IsPacked(ByVal dwFourcc As UInteger) As Byte
        If (dwFourcc = MWCAP_FOURCC_NV12 Or
            dwFourcc = MWCAP_FOURCC_NV21 Or
            dwFourcc = MWCAP_FOURCC_YV12 Or
            dwFourcc = MWCAP_FOURCC_IYUV Or
            dwFourcc = MWCAP_FOURCC_I420) Then
            Return False
        Else
            Return True
        End If
    End Function

    Public Function FOURCC_GetBpp(ByVal dwFourcc As UInteger) As Integer
        If (dwFourcc = MWCAP_FOURCC_GREY Or
            dwFourcc = MWCAP_FOURCC_Y800 Or
            dwFourcc = MWCAP_FOURCC_Y8) Then
            Return 8
        ElseIf dwFourcc = MWCAP_FOURCC_I420 Or
            dwFourcc = MWCAP_FOURCC_IYUV Or
            dwFourcc = MWCAP_FOURCC_YV12 Or
            dwFourcc = MWCAP_FOURCC_NV12 Or
            dwFourcc = MWCAP_FOURCC_NV21 Then
            Return 12
        ElseIf dwFourcc = MWCAP_FOURCC_Y16 Or
            dwFourcc = MWCAP_FOURCC_RGB15 Or
            dwFourcc = MWCAP_FOURCC_BGR15 Or
            dwFourcc = MWCAP_FOURCC_RGB16 Or
            dwFourcc = MWCAP_FOURCC_BGR16 Or
            dwFourcc = MWCAP_FOURCC_YUY2 Or
            dwFourcc = MWCAP_FOURCC_YUYV Or
            dwFourcc = MWCAP_FOURCC_UYVY Or
            dwFourcc = MWCAP_FOURCC_YVYU Or
            dwFourcc = MWCAP_FOURCC_VYUY Then
            Return 16
        ElseIf dwFourcc = MWCAP_FOURCC_IYU2 Or
            dwFourcc = MWCAP_FOURCC_V308 Or
            dwFourcc = MWCAP_FOURCC_RGB24 Or
            dwFourcc = MWCAP_FOURCC_BGR24 Then
            Return 24
        ElseIf dwFourcc = MWCAP_FOURCC_AYUV Or
           dwFourcc = MWCAP_FOURCC_UYVA Or
           dwFourcc = MWCAP_FOURCC_V408 Or
           dwFourcc = MWCAP_FOURCC_VYUA Or
           dwFourcc = MWCAP_FOURCC_RGBA Or
           dwFourcc = MWCAP_FOURCC_BGRA Or
           dwFourcc = MWCAP_FOURCC_ARGB Or
           dwFourcc = MWCAP_FOURCC_ABGR Or
           dwFourcc = MWCAP_FOURCC_Y410 Or
           dwFourcc = MWCAP_FOURCC_V410 Or
           dwFourcc = MWCAP_FOURCC_RGB10 Or
           dwFourcc = MWCAP_FOURCC_BGR10 Then
            Return 32
        Else
            Return 0

        End If
    End Function

    Public Function FOURCC_CalcMinStride(ByVal dwFourcc As UInteger, ByVal cx As Integer, ByVal dwAlign As UInteger) As UInteger
        Dim bPacked As Boolean
        bPacked = False
        bPacked = FOURCC_IsPacked(dwFourcc)

        Dim cbLine As UInteger
        If (bPacked = True) Then
            Dim nBpp As Integer
            nBpp = FOURCC_GetBpp(dwFourcc)
            cbLine = CInt(cx * nBpp) / 8
        Else
            cbLine = CInt(cx)
        End If

        Dim nRet As Integer
        nRet = (cbLine + dwAlign - 1) And (Not (dwAlign - 1))

        Return nRet

    End Function

    Public Function FOURCC_CalcImageSize(ByVal dwFourcc As UInteger, ByVal cx As Integer, ByVal cy As Integer, ByVal cbStride As UInteger) As UInteger
        Dim bPacked As Boolean
        bPacked = False
        bPacked = FOURCC_IsPacked(dwFourcc)
        If bPacked = True Then
            Dim nBpp As Integer
            nBpp = FOURCC_GetBpp(dwFourcc)
            Dim cbLine As UInteger
            cbLine = CInt(cx * nBpp) / 8
            If cbStride < cbLine Then
                Return 0
            Else
                Return CInt(cbStride * CInt(cy))
            End If
        Else
            If cbStride < CInt(cx) Then
                Return 0
            End If

            If dwFourcc = MWCAP_FOURCC_NV12 Or
                dwFourcc = MWCAP_FOURCC_NV21 Or
                dwFourcc = MWCAP_FOURCC_YV12 Or
                dwFourcc = MWCAP_FOURCC_IYUV Or
                dwFourcc = MWCAP_FOURCC_I420 Then
                Dim nRet1 As Integer
                Dim nRet2 As Integer
                nRet1 = cbStride And &H1
                nRet2 = cy And &H1
                If nRet1 = 1 Or nRet2 = 1 Then
                    Return 0
                Else
                    Return cbStride * CInt(cy) * 3 / 2
                End If
            Else
                Return 0
            End If

        End If

    End Function
End Class
