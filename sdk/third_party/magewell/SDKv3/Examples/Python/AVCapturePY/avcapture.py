import sys
from PyQt5 import QtWidgets
from PyQt5.QtGui import QSurfaceFormat
from avcapturewid import *

if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)
    #t_dft_fmt = QSurfaceFormat.defaultFormat()
    #t_dft_fmt.setProfile(QSurfaceFormat.CoreProfile)
    #t_dft_fmt.setMajorVersion(4)
    #t_dft_fmt.setMinorVersion(0)
    #QSurfaceFormat.setDefaultFormat(t_dft_fmt)
    win = CAVCaptureWid()
    win.show()
    res = app.exec_()
    win.m_video_render.clean_up()
    sys.exit(res)