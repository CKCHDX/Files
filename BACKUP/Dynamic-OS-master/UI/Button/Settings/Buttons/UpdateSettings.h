#ifndef UPDATESETTINGS_H
#define UPDATESETTINGS_H

#include <QWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>

class UpdateSettings : public QWidget
{
    Q_OBJECT
public:
    explicit UpdateSettings(QWidget *parent = nullptr);

private slots:
    void onUpdateClicked();
    void onDownloadFinished();
    void onDownloadReadyRead();
    void onDownloadError(QNetworkReply::NetworkError code);

private:
    void appendLog(const QString &text);
    bool replaceExecutable(const QString &tempFilePath);
    void restartApp();
    void checkForUpdate();

    QPushButton *updateButton;
    QTextEdit *logOutput;

    QNetworkAccessManager *networkManager;
    QNetworkReply *currentDownload = nullptr;
    QFile *file = nullptr;
};

#endif // UPDATESETTINGS_H
