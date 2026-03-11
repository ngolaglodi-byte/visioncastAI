using System;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace AVCaptureCSharp
{
    public class LibMWMedia
    {
//////////////////////////////////////////////////////////////////////////////////////////
// video renderer
#if DEBUG
        [DllImport("LibMWMediad.dll", CallingConvention = CallingConvention.Cdecl)]
#else
        [DllImport("LibMWMedia.dll", CallingConvention = CallingConvention.Cdecl)]
#endif
        public static extern IntPtr MWCreateD3DRenderer(int cx, int cy, UInt32 dwFourcc, bool bReberse, bool bMirror, IntPtr hWnd);


#if DEBUG
        [DllImport("LibMWMediad.dll", CallingConvention = CallingConvention.Cdecl)]
#else
        [DllImport("LibMWMedia.dll", CallingConvention = CallingConvention.Cdecl)]
#endif
        public static extern void MWDestroyD3DRenderer(IntPtr hRenderer);


#if DEBUG
        [DllImport("LibMWMediad.dll", CallingConvention = CallingConvention.Cdecl)]
#else
        [DllImport("LibMWMedia.dll", CallingConvention = CallingConvention.Cdecl)]
#endif
        public static extern int MWD3DRendererPushFrame(IntPtr hRenderer, IntPtr pbyBuffer, int cbStride);
    }
}
