#include "MainWindow.h"
#include <QWebEngineView>
#include <QWebChannel>
#include <QDir>
#include "BackendBridge.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    webView = new QWebEngineView(this);

    // Load local index.html from gui folder
    QString url = QDir::current().absoluteFilePath("gui/index.html");
    webView->load(QUrl::fromLocalFile(url));

    setCentralWidget(webView);

    channel = new QWebChannel(this);
    backendBridge = new BackendBridge(this);

    channel->registerObject(QStringLiteral("backend"), backendBridge);
    webView->page()->setWebChannel(channel);
}
