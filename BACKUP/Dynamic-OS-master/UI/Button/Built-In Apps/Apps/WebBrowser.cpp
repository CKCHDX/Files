#include "WebBrowser.h"
#include "FullScreenWindow.h"
#include <QApplication>
#include <QScreen>
#include <QWebEngineSettings>

WebBrowser::WebBrowser(QWidget* parent)
: QWidget(parent)
{
    urlEdit = new QLineEdit(this);
    goButton = new QPushButton("Go", this);
    backButton = new QPushButton("←", this);
    webView = new QWebEngineView(this);

    QHBoxLayout* topLayout = new QHBoxLayout;
    topLayout->addWidget(backButton);
    topLayout->addWidget(urlEdit);
    topLayout->addWidget(goButton);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(webView);

    setLayout(mainLayout);

    connect(goButton, &QPushButton::clicked, this, &WebBrowser::loadPage);
    connect(urlEdit, &QLineEdit::returnPressed, this, &WebBrowser::loadPage);
    connect(backButton, &QPushButton::clicked, this, &WebBrowser::goBack);

    // Enable fullscreen support!
    webView->settings()->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, true);

    // Connect to the page's fullScreenRequested signal
    connect(webView->page(), &QWebEnginePage::fullScreenRequested, this, &WebBrowser::handleFullScreenRequest);

    urlEdit->setText("https://www.oscyra.solutions");
    loadPage();
}

WebBrowser::~WebBrowser()
{
    if (fsWindow) {
        fsWindow->close();
        fsWindow = nullptr;
    }
}

void WebBrowser::loadPage()
{
    QString url = urlEdit->text().trimmed();
    if (!url.startsWith("http://") && !url.startsWith("https://"))
        url = "https://" + url;
    webView->setUrl(QUrl(url));
}

void WebBrowser::goBack()
{
    webView->back();
}

void WebBrowser::handleFullScreenRequest(QWebEngineFullScreenRequest request)
{
    if (request.toggleOn()) {
        if (!fsWindow) {
            request.accept();
            fsWindow = new FullScreenWindow(webView);
            connect(fsWindow, &QWidget::destroyed, [this]() { fsWindow = nullptr; });
        }
    } else {
        request.accept();
        if (fsWindow) {
            fsWindow->close();
            fsWindow = nullptr;
        }
    }
}
