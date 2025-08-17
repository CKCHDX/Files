#include "mainwindow.h"
#include "background_widget.h"
#include "pages/welcome_page.h"
#include "pages/keyboard_page.h"
#include "pages/disk_page.h"
#include "pages/user_page.h"
#include "pages/install_page.h"
#include "pages/finish_page.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include <QProcess>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      bgWidget(nullptr),
      welcomePage(nullptr),
      keyboardPage(nullptr),
      diskPage(nullptr),
      userPage(nullptr),
      installPage(nullptr),
      finishPage(nullptr)
{
    setWindowTitle("Oscyra Installer");
    resize(800, 500);

    bgWidget = new BackgroundWidget(this);
    bgWidget->setGeometry(0, 0, width(), height());
    bgWidget->lower();

    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout *layout = new QVBoxLayout(central);
    welcomePage = new WelcomePage(this);
    layout->addWidget(welcomePage);

    connect(welcomePage, &WelcomePage::startClicked, this, &MainWindow::onStartInstallation);
}

MainWindow::~MainWindow() {}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    if (bgWidget)
        bgWidget->setGeometry(0, 0, width(), height());
}

void MainWindow::onStartInstallation() {
    QString licenseText =
        "Dynamic-OS Installer – License Agreement\n\n"
        "By continuing, you agree to install Dynamic-OS, a modern Linux-based system built for speed, security, and user-friendly design. "
        "Dynamic-OS is provided as-is, without warranty. Your privacy is respected; no data is collected during installation. "
        "For more details, visit oscyra.solutions/\n\n"
        "Please review and accept to proceed.";

    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Oscyra OS License Agreement");
    msgBox.setText(licenseText);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);

    int ret = msgBox.exec();
    if (ret == QMessageBox::Ok) {
        onLicenseAccepted();
    }
}

void MainWindow::onLicenseAccepted() {
    if (welcomePage) {
        welcomePage->hide();
        welcomePage->deleteLater();
        welcomePage = nullptr;
    }

    keyboardPage = new KeyboardPage(this);
    centralWidget()->layout()->addWidget(keyboardPage);
    connect(keyboardPage, &KeyboardPage::continueClicked, this, &MainWindow::onKeyboardContinue);
}

void MainWindow::onKeyboardContinue(const QString &layout) {
    selectedKeyboardLayout = layout;
    if (keyboardPage) {
        keyboardPage->hide();
        keyboardPage->deleteLater();
        keyboardPage = nullptr;
    }
    diskPage = new DiskPage(this);
    centralWidget()->layout()->addWidget(diskPage);
    connect(diskPage, &DiskPage::continueClicked, this, &MainWindow::onDiskContinue);
}

void MainWindow::onDiskContinue(const QString &disk) {
    selectedDisk = disk;
    if (diskPage) {
        diskPage->hide();
        diskPage->deleteLater();
        diskPage = nullptr;
    }
    userPage = new UserPage(this);
    centralWidget()->layout()->addWidget(userPage);
    connect(userPage, &UserPage::continueClicked, this, &MainWindow::onUserContinue);
}

void MainWindow::onUserContinue(const QString &username, const QString &password, const QString &hostname) {
    selectedUsername = username;
    selectedPassword = password;
    selectedHostname = hostname;
    if (userPage) {
        userPage->hide();
        userPage->deleteLater();
        userPage = nullptr;
    }

    installPage = new InstallPage(this);
    centralWidget()->layout()->addWidget(installPage);

    installPage->startInstall(
        selectedKeyboardLayout,
        selectedDisk,
        selectedUsername,
        selectedPassword,
        selectedHostname
    );

    connect(installPage, &InstallPage::installFinished, this,
        [this](bool success, const QString &errorMsg){
            if (installPage) {
                installPage->hide();
                installPage->deleteLater();
                installPage = nullptr;
            }
            finishPage = new FinishPage(success, this, errorMsg);
            centralWidget()->layout()->addWidget(finishPage);

            connect(finishPage, &FinishPage::rebootRequested, this, [](){
                QProcess::startDetached("reboot", QStringList());
                QApplication::quit();
            });
            connect(finishPage, &FinishPage::exitRequested, this, [](){
                QApplication::quit();
            });
        }
    );
}
