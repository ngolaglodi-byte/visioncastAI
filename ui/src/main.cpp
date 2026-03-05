/// @file main.cpp
/// @brief VisionCast Qt Control Room entry point.

#include <QApplication>

#include "visioncast_ui/main_window.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("VisionCast Control Room");
    app.setOrganizationName("VisionCast");

    visioncast_ui::MainWindow window;
    window.setWindowTitle("VisionCast AI — Broadcast Control Room");
    window.resize(1600, 900);
    window.show();

    return app.exec();
}
