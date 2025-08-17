#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class BackgroundWidget;
class WelcomePage;
class KeyboardPage;
class DiskPage;
class UserPage;
class InstallPage;
class FinishPage;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onStartInstallation();
    void onLicenseAccepted();
    void onKeyboardContinue(const QString &layout);
    void onDiskContinue(const QString &disk);
    void onUserContinue(const QString &username, const QString &password, const QString &hostname);

private:
    BackgroundWidget *bgWidget;
    WelcomePage *welcomePage;
    KeyboardPage *keyboardPage;
    DiskPage *diskPage;
    UserPage *userPage;
    InstallPage *installPage;
    FinishPage *finishPage;
    QString selectedKeyboardLayout;
    QString selectedDisk;
    QString selectedUsername;
    QString selectedPassword;
    QString selectedHostname;
};

#endif // MAINWINDOW_H
