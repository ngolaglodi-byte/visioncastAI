using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace AVCapture2CSharp
{
    public class MWCap_FOURCC
    {
        static public UInt32 MWCAP_FOURCC_UNK = MWCAP_FOURCC('U', 'N', 'K', 'N');

        // 8bits grey
        static public UInt32 MWCAP_FOURCC_GREY = MWCAP_FOURCC('G', 'R', 'E', 'Y');					// Y0, Y1, Y2, ...
        static public UInt32 MWCAP_FOURCC_Y800 = MWCAP_FOURCC('Y', '8', '0', '0');					// = GREY
        static public UInt32 MWCAP_FOURCC_Y8 = MWCAP_FOURCC('Y', '8', ' ', ' ');					// = GREY

        // 16bits grey
        static public UInt32 MWCAP_FOURCC_Y16 = MWCAP_FOURCC('Y', '1', '6', ' ');					// Y0, Y1, Y2

        // RGB 15-32bits
        static public UInt32 MWCAP_FOURCC_RGB15 = MWCAP_FOURCC('R', 'G', 'B', '5');					// R0, G0, B0, A0, ...
        static public UInt32 MWCAP_FOURCC_RGB16 = MWCAP_FOURCC('R', 'G', 'B', '6');					// R0, G0, B0, R1, ...
        static public UInt32 MWCAP_FOURCC_RGB24 = MWCAP_FOURCC('R', 'G', 'B', ' ');					// R0, G0, B0, R1, ...
        static public UInt32 MWCAP_FOURCC_RGBA = MWCAP_FOURCC('R', 'G', 'B', 'A');					// R0, G0, B0, A0, R1, ...
        static public UInt32 MWCAP_FOURCC_ARGB = MWCAP_FOURCC('A', 'R', 'G', 'B');					// A0, R0, G0, B0, A1, ...

        static public UInt32 MWCAP_FOURCC_BGR15 = MWCAP_FOURCC('B', 'G', 'R', '5');					// B0, G0, R0, A0, ...
        static public UInt32 MWCAP_FOURCC_BGR16 = MWCAP_FOURCC('B', 'G', 'R', '6');					// B0, G0, R0, B1, ...
        static public UInt32 MWCAP_FOURCC_BGR24 = MWCAP_FOURCC('B', 'G', 'R', ' ');					// B0, G0, R0, B1, ...
        static public UInt32 MWCAP_FOURCC_BGRA = MWCAP_FOURCC('B', 'G', 'R', 'A');					// B0, G0, R0, A0, B1, ...
        static public UInt32 MWCAP_FOURCC_ABGR = MWCAP_FOURCC('A', 'B', 'G', 'R');					// A0, B0, G0, R0, A1, ...

        // Packed YUV 16bits
        static public UInt32 MWCAP_FOURCC_YUY2 = MWCAP_FOURCC('Y', 'U', 'Y', '2');					// Y0, U01, Y1, V01, ...
        static public UInt32 MWCAP_FOURCC_YUYV = MWCAP_FOURCC('Y', 'U', 'Y', 'V');					// = YUY2
        static public UInt32 MWCAP_FOURCC_UYVY = MWCAP_FOURCC('U', 'Y', 'V', 'Y');					// U01, Y0, V01, Y1, ...

        static public UInt32 MWCAP_FOURCC_YVYU = MWCAP_FOURCC('Y', 'V', 'Y', 'U');					// Y0, V01, Y1, U01, ...
        static public UInt32 MWCAP_FOURCC_VYUY = MWCAP_FOURCC('V', 'Y', 'U', 'Y');					// V01, Y0, U01, Y1, ...

        // Planar YUV 12bits
        static public UInt32 MWCAP_FOURCC_I420 = MWCAP_FOURCC('I', '4', '2', '0');					// = IYUV (Y, U, V)
        static public UInt32 MWCAP_FOURCC_IYUV = MWCAP_FOURCC('I', 'Y', 'U', 'V');					// = I420 (Y, U, V)
        static public UInt32 MWCAP_FOURCC_NV12 = MWCAP_FOURCC('N', 'V', '1', '2');					// Y Plane, UV Plane

        static public UInt32 MWCAP_FOURCC_YV12 = MWCAP_FOURCC('Y', 'V', '1', '2');					// Y Plane, V Plane, U Plane
        static public UInt32 MWCAP_FOURCC_NV21 = MWCAP_FOURCC('N', 'V', '2', '1');					// Y Plane, VU Plane

        // Packed YUV 24bits
        static public UInt32 MWCAP_FOURCC_IYU2 = MWCAP_FOURCC('I', 'Y', 'U', '2');					// U0, Y0, V0, U1, Y1, V1, ...
        static public UInt32 MWCAP_FOURCC_V308 = MWCAP_FOURCC('v', '3', '0', '8');					// V0, Y0, U0, V1, Y1, U1, ...

        // Packed YUV 32bits
        static public UInt32 MWCAP_FOURCC_AYUV = MWCAP_FOURCC('A', 'Y', 'U', 'V');					// A0, Y0, U0, V0, ...
        static public UInt32 MWCAP_FOURCC_UYVA = MWCAP_FOURCC('U', 'Y', 'V', 'A');					// U0, Y0, V0, A0, U1, Y1, ...
        static public UInt32 MWCAP_FOURCC_V408 = MWCAP_FOURCC('v', '4', '0', '8');					// = MWFOURCC_UYVA
        static public UInt32 MWCAP_FOURCC_VYUA = MWCAP_FOURCC('V', 'Y', 'U', 'A');					// V0, Y0, U0, A0, V1, Y1, ...

        // Packed YUV 30bits deep color
        static public UInt32 MWCAP_FOURCC_Y410 = MWCAP_FOURCC('Y', '4', '1', '0');					// U0, Y0, V0, A0, ...
        static public UInt32 MWCAP_FOURCC_V410 = MWCAP_FOURCC('v', '4', '1', '0');					// A0, U0, Y0, V0, ...

        // Packed RGB 30bits deep color
        static public UInt32 MWCAP_FOURCC_RGB10 = MWCAP_FOURCC('R', 'G', '1', '0');					// R0, G0, B0, A0, ...
        static public UInt32 MWCAP_FOURCC_BGR10 = MWCAP_FOURCC('B', 'G', '1', '0');					// B0, G0, R0, A0, ...




        static private UInt32 MWCAP_FOURCC(char ch0, char ch1, char ch2, char ch3)
        {

            return (UInt32)ch0 | ((UInt32)ch1 << 8) | ((UInt32)ch2 << 16) | ((UInt32)ch3 << 24);
        }

        static public bool FOURCC_IsRGB(UInt32 dwFourcc)
        {

            if (dwFourcc == MWCAP_FOURCC_RGB15 ||
                    dwFourcc == MWCAP_FOURCC_BGR15 ||
                    dwFourcc == MWCAP_FOURCC_RGB16 ||
                    dwFourcc == MWCAP_FOURCC_BGR16 ||
                    dwFourcc == MWCAP_FOURCC_RGB24 ||
                    dwFourcc == MWCAP_FOURCC_BGR24 ||
                    dwFourcc == MWCAP_FOURCC_RGBA ||
                    dwFourcc == MWCAP_FOURCC_BGRA ||
                    dwFourcc == MWCAP_FOURCC_ARGB ||
                    dwFourcc == MWCAP_FOURCC_ABGR ||
                    dwFourcc == MWCAP_FOURCC_RGB10 ||
                    dwFourcc == MWCAP_FOURCC_BGR10)
                return true;


            return false;

        }


        static public bool FOURCC_IsPacked(UInt32 dwFourcc)
        {
            if (dwFourcc == MWCAP_FOURCC_NV12 ||
                dwFourcc == MWCAP_FOURCC_NV21 ||
                dwFourcc == MWCAP_FOURCC_YV12 ||
                dwFourcc == MWCAP_FOURCC_IYUV ||
                dwFourcc == MWCAP_FOURCC_I420)
                return false;

            return true;

        }


        static public int FOURCC_GetBpp(UInt32 dwFourcc)
        {

            if (dwFourcc == MWCAP_FOURCC_GREY ||
                dwFourcc == MWCAP_FOURCC_Y800 ||
                dwFourcc == MWCAP_FOURCC_Y8)
                return 8;
            else if (dwFourcc == MWCAP_FOURCC_I420 ||
                     dwFourcc == MWCAP_FOURCC_IYUV ||
                     dwFourcc == MWCAP_FOURCC_YV12 ||
                     dwFourcc == MWCAP_FOURCC_NV12 ||
                     dwFourcc == MWCAP_FOURCC_NV21)
                return 12;
            else if (dwFourcc == MWCAP_FOURCC_Y16 ||
                    dwFourcc == MWCAP_FOURCC_RGB15 ||
                    dwFourcc == MWCAP_FOURCC_BGR15 ||
                    dwFourcc == MWCAP_FOURCC_RGB16 ||
                    dwFourcc == MWCAP_FOURCC_BGR16 ||
                    dwFourcc == MWCAP_FOURCC_YUY2 ||
                    dwFourcc == MWCAP_FOURCC_YUYV ||
                    dwFourcc == MWCAP_FOURCC_UYVY ||
                    dwFourcc == MWCAP_FOURCC_YVYU ||
                    dwFourcc == MWCAP_FOURCC_VYUY)
                return 16;
            else if (dwFourcc == MWCAP_FOURCC_IYU2 ||
                    dwFourcc == MWCAP_FOURCC_V308 ||
                    dwFourcc == MWCAP_FOURCC_RGB24 ||
                    dwFourcc == MWCAP_FOURCC_BGR24)
                return 24;
            else if (dwFourcc == MWCAP_FOURCC_AYUV ||
                    dwFourcc == MWCAP_FOURCC_UYVA ||
                    dwFourcc == MWCAP_FOURCC_V408 ||
                    dwFourcc == MWCAP_FOURCC_VYUA ||
                    dwFourcc == MWCAP_FOURCC_RGBA ||
                    dwFourcc == MWCAP_FOURCC_BGRA ||
                    dwFourcc == MWCAP_FOURCC_ARGB ||
                    dwFourcc == MWCAP_FOURCC_ABGR ||
                    dwFourcc == MWCAP_FOURCC_Y410 ||
                    dwFourcc == MWCAP_FOURCC_V410 ||
                    dwFourcc == MWCAP_FOURCC_RGB10 ||
                    dwFourcc == MWCAP_FOURCC_BGR10)
                return 32;
            else
                return 0;

        }

        static public UInt32 FOURCC_CalcMinStride(UInt32 dwFOURCC, int cx, UInt32 dwAlign)
        {
            bool bPacked = FOURCC_IsPacked(dwFOURCC);

            UInt32 cbLine;

            if (bPacked)
            {
                int nBpp = FOURCC_GetBpp(dwFOURCC);
                cbLine = (UInt32)(cx * nBpp) / 8;
            }
            else
                cbLine = (UInt32)cx;

            return (cbLine + dwAlign - 1) & ~(dwAlign - 1);
        }

        static public UInt32 FOURCC_CalcImageSize(UInt32 dwFOURCC, int cx, int cy, UInt32 cbStride)
        {
            bool bPacked = FOURCC_IsPacked(dwFOURCC);

            if (bPacked)
            {
                int nBpp = FOURCC_GetBpp(dwFOURCC);
                UInt32 cbLine = (UInt32)(cx * nBpp) / 8;
                if (cbStride < cbLine)
                    return 0;

                return cbStride * (UInt32)cy;
            }
            else
            {
                if (cbStride < (UInt32)cx)
                    return 0;

                if (dwFOURCC == MWCAP_FOURCC_NV12 ||
                    dwFOURCC == MWCAP_FOURCC_NV21 ||
                    dwFOURCC == MWCAP_FOURCC_YV12 ||
                    dwFOURCC == MWCAP_FOURCC_IYUV ||
                    dwFOURCC == MWCAP_FOURCC_I420)
                {
                    if ((cbStride & 0x01) == 1 || (cy & 0x01) == 1)
                        return 0;
                    return cbStride * (UInt32)cy * 3 / 2;

                }
                else
                    return 0;

            }


        }
    }
}
