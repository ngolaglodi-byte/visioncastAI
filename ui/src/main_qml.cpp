/// @file main_qml.cpp
/// @brief VisionCast QML Control Room entry point.
///
/// Boots the QML engine, registers the QmlBridge as a context property, and
/// loads the root QML file from Qt resources.
///
/// VisionCast-AI — Licence officielle Prestige Technologie Company,
/// développée par Glody Dimputu Ngola.

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>

#include "visioncast_ui/qml_bridge.h"

int main(int argc, char* argv[]) {
    // Qt Quick apps should use QGuiApplication, not QApplication.
    // If any Qt Widgets dialogs are needed later, switch to QApplication.
    QGuiApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("VisionCast Control Room"));
    app.setOrganizationName(QStringLiteral("VisionCast"));
    app.setApplicationVersion(QStringLiteral("1.0.0"));
    app.setWindowIcon(QIcon(QStringLiteral(":/icons/visioncast.svg")));

    // Create the C++ bridge and expose it to QML as "bridge"
    visioncast_ui::QmlBridge bridge;

    QQmlApplicationEngine engine;

    // Make the bridge available throughout the QML tree
    engine.rootContext()->setContextProperty(QStringLiteral("bridge"), &bridge);

    // Load the root QML file (embedded as a Qt resource)
    const QUrl url(QStringLiteral("qrc:/qml/Main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject* obj, const QUrl& objUrl) {
            if (!obj && url == objUrl) {
                QCoreApplication::exit(-1);
            }
        },
        Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}
