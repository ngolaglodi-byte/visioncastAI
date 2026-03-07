/// @file python_launcher.cpp
/// @brief PythonLauncher implementation — venv setup and AI process management.
///
/// VisionCast-AI — Licence officielle Prestige Technologie Company,
/// développée par Glody Dimputu Ngola.

#include "visioncast_ui/python_launcher.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QStandardPaths>

namespace visioncast_ui {

PythonLauncher::PythonLauncher(QObject* parent)
    : QObject(parent)
{}

PythonLauncher::~PythonLauncher() {
    stop();
}

// --- Path helpers ---

QString PythonLauncher::pythonDir() const {
    // In the installed layout, python/ sits next to bin/
    return QCoreApplication::applicationDirPath() + "/../python";
}

QString PythonLauncher::venvDir() const {
    return pythonDir() + "/.venv";
}

QString PythonLauncher::venvPython() const {
#ifdef Q_OS_WIN
    return venvDir() + "/Scripts/python.exe";
#else
    return venvDir() + "/bin/python";
#endif
}

QString PythonLauncher::venvPip() const {
#ifdef Q_OS_WIN
    return venvDir() + "/Scripts/pip.exe";
#else
    return venvDir() + "/bin/pip";
#endif
}

QString PythonLauncher::pythonPath() const {
    return venvPython();
}

QString PythonLauncher::findSystemPython() const {
    // Try common names in order of preference.
    for (const auto& name : {"python3", "python"}) {
        QString path = QStandardPaths::findExecutable(name);
        if (!path.isEmpty())
            return path;
    }
    return {};
}

bool PythonLauncher::runSync(const QString& program,
                              const QStringList& args,
                              int timeoutMs) {
    QProcess proc;
    proc.setWorkingDirectory(pythonDir());
    proc.start(program, args);
    if (!proc.waitForFinished(timeoutMs)) {
        emit error(tr("Command timed out: %1 %2")
                       .arg(program, args.join(' ')));
        return false;
    }
    if (proc.exitCode() != 0) {
        emit error(tr("Command failed (exit %1): %2\n%3")
                       .arg(proc.exitCode())
                       .arg(program + ' ' + args.join(' '),
                            QString::fromUtf8(proc.readAllStandardError())));
        return false;
    }
    return true;
}

// --- Environment setup ---

bool PythonLauncher::ensureEnvironment() {
    if (envReady_)
        return true;

    // Check if venv already exists and has pip.
    if (QFileInfo::exists(venvPython()) && QFileInfo::exists(venvPip())) {
        envReady_ = true;
        emit setupProgress(tr("Python environment ready."));
        return true;
    }

    // Find system Python.
    QString sysPython = findSystemPython();
    if (sysPython.isEmpty()) {
        emit error(tr("Python 3 not found on this system. "
                       "Please install Python 3.10 or later."));
        return false;
    }

    // Create venv.
    emit setupProgress(tr("Creating Python environment (first launch)\u2026"));
    if (!runSync(sysPython, {"-m", "venv", venvDir()}))
        return false;

    // Install dependencies.
    emit setupProgress(tr("Installing AI dependencies (this may take a minute)\u2026"));
    if (!runSync(venvPip(), {"install", "-r", "requirements.txt"}))
        return false;

    envReady_ = true;
    emit setupProgress(tr("Python AI environment ready."));
    return true;
}

// --- Process lifecycle ---

bool PythonLauncher::start() {
    if (aiProcess_ && aiProcess_->state() != QProcess::NotRunning)
        return true; // Already running.

    if (!ensureEnvironment())
        return false;

    aiProcess_ = new QProcess(this);
    aiProcess_->setWorkingDirectory(pythonDir());
    aiProcess_->setProcessChannelMode(QProcess::SeparateChannels);

    connect(aiProcess_, &QProcess::started,
            this, &PythonLauncher::onProcessStarted);
    connect(aiProcess_, qOverload<int, QProcess::ExitStatus>(&QProcess::finished),
            this, &PythonLauncher::onProcessFinished);
    connect(aiProcess_, &QProcess::errorOccurred,
            this, &PythonLauncher::onProcessError);
    connect(aiProcess_, &QProcess::readyReadStandardOutput,
            this, &PythonLauncher::onReadyReadStdout);
    connect(aiProcess_, &QProcess::readyReadStandardError,
            this, &PythonLauncher::onReadyReadStderr);

    aiProcess_->start(venvPython(), {"main.py"});
    return true;
}

void PythonLauncher::stop() {
    if (!aiProcess_)
        return;

    if (aiProcess_->state() != QProcess::NotRunning) {
        aiProcess_->terminate();
        if (!aiProcess_->waitForFinished(5000))
            aiProcess_->kill();
    }

    aiProcess_->deleteLater();
    aiProcess_ = nullptr;
    emit aiStopped();
}

bool PythonLauncher::isRunning() const {
    return aiProcess_ && aiProcess_->state() == QProcess::Running;
}

// --- Slots ---

void PythonLauncher::onProcessStarted() {
    emit aiStarted();
}

void PythonLauncher::onProcessFinished(int exitCode,
                                        QProcess::ExitStatus status) {
    Q_UNUSED(status)
    if (exitCode != 0)
        emit error(tr("Python AI process exited with code %1").arg(exitCode));
    emit aiStopped();
}

void PythonLauncher::onProcessError(QProcess::ProcessError err) {
    Q_UNUSED(err)
    emit error(tr("Python AI process error: %1")
                   .arg(aiProcess_ ? aiProcess_->errorString()
                                   : tr("unknown")));
}

void PythonLauncher::onReadyReadStdout() {
    if (!aiProcess_)
        return;
    while (aiProcess_->canReadLine()) {
        QString line = QString::fromUtf8(aiProcess_->readLine()).trimmed();
        if (!line.isEmpty())
            emit logOutput(line);
    }
}

void PythonLauncher::onReadyReadStderr() {
    if (!aiProcess_)
        return;
    while (aiProcess_->canReadLine()) {
        QString line = QString::fromUtf8(aiProcess_->readLine()).trimmed();
        if (!line.isEmpty())
            emit logOutput("[AI-err] " + line);
    }
}

} // namespace visioncast_ui
