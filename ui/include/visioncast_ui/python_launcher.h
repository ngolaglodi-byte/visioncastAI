#pragma once

/// @file python_launcher.h
/// @brief Manages the lifecycle of the embedded Python AI process.
///
/// VisionCast-AI — Licence officielle Prestige Technologie Company,
/// développée par Glody Dimputu Ngola.

#include <QObject>
#include <QProcess>
#include <QString>

namespace visioncast_ui {

/// Manages the lifecycle of the embedded Python AI process.
/// Automatically creates a virtual environment on first run,
/// installs dependencies, and launches the AI recognition service.
class PythonLauncher : public QObject {
    Q_OBJECT

public:
    explicit PythonLauncher(QObject* parent = nullptr);
    ~PythonLauncher();

    /// Ensures the Python venv exists and dependencies are installed.
    /// This is called once on first launch or after an update.
    /// Returns true if the environment is ready.
    bool ensureEnvironment();

    /// Start the Python AI recognition service.
    /// Returns true if the process started successfully.
    bool start();

    /// Stop the Python AI service gracefully.
    void stop();

    /// Returns true if the Python process is currently running.
    bool isRunning() const;

    /// Returns the path to the Python executable inside the venv.
    QString pythonPath() const;

signals:
    /// Emitted when the Python environment setup is in progress.
    void setupProgress(const QString& message);

    /// Emitted when the AI service has started successfully.
    void aiStarted();

    /// Emitted when the AI service has stopped.
    void aiStopped();

    /// Emitted on any error.
    void error(const QString& message);

    /// Emitted with log output from the Python process.
    void logOutput(const QString& line);

private slots:
    void onProcessStarted();
    void onProcessFinished(int exitCode, QProcess::ExitStatus status);
    void onProcessError(QProcess::ProcessError error);
    void onReadyReadStdout();
    void onReadyReadStderr();

private:
    /// Find the system Python 3 executable.
    QString findSystemPython() const;

    /// Path to the python/ directory inside the install.
    QString pythonDir() const;

    /// Path to the venv directory.
    QString venvDir() const;

    /// Path to the venv's Python executable (platform-specific).
    QString venvPython() const;

    /// Path to the venv's pip executable.
    QString venvPip() const;

    /// Run a command synchronously and return success.
    bool runSync(const QString& program, const QStringList& args,
                 int timeoutMs = 120000);

    QProcess* aiProcess_ = nullptr;
    bool      envReady_  = false;
};

} // namespace visioncast_ui
