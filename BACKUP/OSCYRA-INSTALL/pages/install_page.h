#ifndef INSTALL_PAGE_H
#define INSTALL_PAGE_H

#include <QWidget>

class QProgressBar;
class QLabel;
class InstallerLogic;

class InstallPage : public QWidget {
    Q_OBJECT
public:
    explicit InstallPage(QWidget *parent = nullptr);

    void setStatus(const QString &status);
    void setProgress(int percent);

public slots:
    void startInstall(
        const QString &keyboardLayout,
        const QString &disk,
        const QString &username,
        const QString &password,
        const QString &hostname
    );

signals:
    void installFinished(bool success, const QString &errorMsg);

private:
    QLabel *statusLabel;
    QProgressBar *progressBar;
    InstallerLogic *logic;
};

#endif // INSTALL_PAGE_H
