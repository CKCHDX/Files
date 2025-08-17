#include "install_page.h"
#include "installerlogic.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QFont>
#include <QMessageBox>

InstallPage::InstallPage(QWidget *parent)
    : QWidget(parent), logic(new InstallerLogic(this))
{
    auto *layout = new QVBoxLayout(this);

    auto *title = new QLabel("Installing Oscyra OS", this);
    QFont titleFont = title->font();
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    title->setFont(titleFont);
    title->setAlignment(Qt::AlignCenter);

    statusLabel = new QLabel("Preparing installation...", this);
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setStyleSheet("color: #c0c6d0; font-size: 15px;");

    progressBar = new QProgressBar(this);
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    progressBar->setFixedWidth(350);

    layout->addStretch();
    layout->addWidget(title);
    layout->addSpacing(20);
    layout->addWidget(statusLabel);
    layout->addSpacing(18);
    layout->addWidget(progressBar, 0, Qt::AlignCenter);
    layout->addStretch();

    connect(logic, &InstallerLogic::progressChanged, this, [this](const QString &status, int percent){
        setStatus(status);
        setProgress(percent);
    });
    connect(logic, &InstallerLogic::installFinished, this, [this](bool success, const QString &error){
        if (success) {
            setStatus("Installation complete!");
            setProgress(100);
        } else {
            setStatus("Error: " + error);
            QMessageBox::critical(this, "Installation Failed", error);
        }
        emit installFinished(success, error);
    });
}

void InstallPage::setStatus(const QString &status) {
    statusLabel->setText(status);
}

void InstallPage::setProgress(int percent) {
    progressBar->setValue(percent);
}

void InstallPage::startInstall(
    const QString &keyboardLayout,
    const QString &disk,
    const QString &username,
    const QString &password,
    const QString &hostname
) {
    setStatus("Starting installation...");
    setProgress(0);
    logic->installSystem(keyboardLayout, disk, username, password, hostname);
}
