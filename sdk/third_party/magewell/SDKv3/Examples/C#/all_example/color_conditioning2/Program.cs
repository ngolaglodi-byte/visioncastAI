using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;
using MWModle;

namespace ColorConditioning2
{
    static class Program
    {
        static void Main(string[] arg)
        {
            int index = 0;
            if (arg.Length == 1) {
                index = Convert.ToInt32(arg[0]);
            }
            CColorConditioning2.init();
            CColorConditioning2.show_info();
            CColorConditioning2 color_conditioning = new CColorConditioning2();
            if (!color_conditioning.open_channel(index)){
                CColorConditioning2.deinit();
                return;
            }

            color_conditioning.start_capture();
            color_conditioning.close_channel();
            CColorConditioning2.deinit();
        }
    }
}
