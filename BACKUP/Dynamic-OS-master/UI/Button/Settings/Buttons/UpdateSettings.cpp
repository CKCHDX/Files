#include "UpdateSettings.h"
#include <QVBoxLayout>
#include <QFile>
#include <QProcess>
#include <QStandardPaths>
#include <QDir>
#include <QCoreApplication>
#include <QMessageBox>

UpdateSettings::UpdateSettings(QWidget *parent)
: QWidget(parent),
networkManager(new QNetworkAccessManager(this))
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    updateButton = new QPushButton("Check for Update and Install", this);
    logOutput = new QTextEdit(this);
    logOutput->setReadOnly(true);

    layout->addWidget(updateButton);
    layout->addWidget(logOutput);

    connect(updateButton, &QPushButton::clicked, this, &UpdateSettings::onUpdateClicked);
}

void UpdateSettings::appendLog(const QString &text)
{
    logOutput->append(text);
}

void UpdateSettings::onUpdateClicked()
{
    if (currentDownload) {
        appendLog("Update already in progress...");
        return;
    }

    appendLog("Starting download...");

    // URL to raw binary release (using raw.githubusercontent.com for direct file download)
    QUrl url("https://raw.githubusercontent.com/CKCHDX/Dynamic-OS/master/update/Oscyra");

    // Prepare temp file path
    QString tempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    QString tempFilePath = tempDir + QDir::separator() + "Oscyra_update";

    file = new QFile(tempFilePath);
    if (!file->open(QIODevice::WriteOnly)) {
        appendLog("Failed to open temp file for writing");
        delete file;
        file = nullptr;
        return;
    }

    QNetworkRequest request(url);
    currentDownload = networkManager->get(request);

    connect(currentDownload, &QNetworkReply::readyRead, this, &UpdateSettings::onDownloadReadyRead);
    connect(currentDownload, &QNetworkReply::finished, this, &UpdateSettings::onDownloadFinished);
    connect(currentDownload, qOverload<QNetworkReply::NetworkError>(&QNetworkReply::errorOccurred),
            this, &UpdateSettings::onDownloadError);

    appendLog("Downloading from " + url.toString());
}

void UpdateSettings::onDownloadReadyRead()
{
    if (file)
        file->write(currentDownload->readAll());
}

void UpdateSettings::onDownloadError(QNetworkReply::NetworkError code)
{
    Q_UNUSED(code);
    appendLog("Download error: " + currentDownload->errorString());

    file->close();
    file->remove();
    delete file;
    file = nullptr;

    currentDownload->deleteLater();
    currentDownload = nullptr;
}

void UpdateSettings::onDownloadFinished()
{
    appendLog("Download finished");

    if (!file) {
        appendLog("File handle invalid");
        currentDownload->deleteLater();
        currentDownload = nullptr;
        return;
    }

    file->flush();
    file->close();

    currentDownload->deleteLater();
    currentDownload = nullptr;

    QString tempFilePath = file->fileName();

    delete file;
    file = nullptr;

    // Make executable
    QFile::setPermissions(tempFilePath, QFile::ExeOwner | QFile::ExeGroup | QFile::ExeOther | QFile::ReadOwner | QFile::WriteOwner);

    appendLog("Making file executable");

    if (!replaceExecutable(tempFilePath)) {
        appendLog("Failed to replace executable");
        return;
    }

    appendLog("Update complete, restarting application...");
    restartApp();
}

bool UpdateSettings::replaceExecutable(const QString &tempFilePath)
{
    QString targetPath = "/usr/bin/Oscyra";

    appendLog("Replacing " + targetPath);

    // Use pkexec for privilege escalation to move file
    // Construct command:
    // pkexec mv /tmp/Oscyra_update /usr/bin/Oscyra
    // pkexec chmod +x /usr/bin/Oscyra

    QStringList commands;
    commands << "mv" << tempFilePath << targetPath;

    QProcess mvProcess;
    QStringList pkexecArgs = {"mv", tempFilePath, targetPath};
    int ret = QProcess::execute("pkexec", pkexecArgs);

    if (ret != 0) {
        appendLog("Failed to move file with pkexec");
        return false;
    }

    // Make sure permissions are correct (redundant, but safe)
    QStringList chmodArgs = {"+x", targetPath};
    ret = QProcess::execute("pkexec", {"chmod", "+x", targetPath});

    if (ret != 0) {
        appendLog("Failed to chmod file with pkexec");
        return false;
    }

    appendLog("File replaced successfully.");

    return true;
}

void UpdateSettings::restartApp()
{
    QString appPath = QCoreApplication::applicationFilePath();

    appendLog("Restarting: " + appPath);

    // Start the new instance
    QProcess::startDetached(appPath, QStringList());

    // Quit current
    QCoreApplication::quit();
}
