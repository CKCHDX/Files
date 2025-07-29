#ifndef WEBBROWSER_H
#define WEBBROWSER_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QWebEngineView>
#include <QWebEngineFullScreenRequest>
#include <QHBoxLayout>
#include <QVBoxLayout>

class FullScreenWindow;

class WebBrowser : public QWidget
{
    Q_OBJECT
public:
    explicit WebBrowser(QWidget* parent = nullptr);
    ~WebBrowser();

private slots:
    void loadPage();
    void goBack();
    void handleFullScreenRequest(QWebEngineFullScreenRequest request);

private:
    QLineEdit* urlEdit;
    QPushButton* goButton;
    QPushButton* backButton;
    QWebEngineView* webView;
    FullScreenWindow* fsWindow = nullptr;
};

#endif // WEBBROWSER_H
