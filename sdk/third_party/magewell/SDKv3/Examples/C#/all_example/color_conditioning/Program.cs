using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;
using MWModle;

namespace ColorConditioning
{
    static class Program
    {
        static void Main(string[] arg)
        {
            int index = 0;
            if (arg.Length == 1) {
                index = Convert.ToInt32(arg[0]);
            }
            CColorConditioning.init();
            CColorConditioning.show_info();
            CColorConditioning color_conditioning = new CColorConditioning();
            if (!color_conditioning.open_channel(index))
            {
                CColorConditioning.deinit();
                return;
            }

            if (!color_conditioning.check_signal())
            {
                color_conditioning.close_channel();
                CColorConditioning.deinit();
                return;
            }

            color_conditioning.start_capture();
            color_conditioning.close_channel();
            CColorConditioning.deinit();
        }
    }
}
