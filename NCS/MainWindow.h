#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QWebEngineView;
class QWebChannel;
class BackendBridge;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    QWebEngineView *webView;
    QWebChannel *channel;
    BackendBridge *backendBridge;
};

#endif // MAINWINDOW_H
