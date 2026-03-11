using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;
using MWModle;
namespace AVViewQuad
{
    static class Program
    {
        /// <summary>
        /// 应用程序的主入口点。
        /// </summary>
        [STAThread]

        static void Main()
        {
            CMWCaptureQuad.Init();
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new mainForm());
            CMWCaptureQuad.Exit();
        }
    }
}
