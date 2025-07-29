// CustomAppWindow.cpp
#include "CustomAppWindow.h"
#include <QApplication>
#include <QScreen>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QWindow>

// A small subclass that simply tells Qt “start the native move” whenever you click it.
class TitleBar : public QWidget {
public:
    explicit TitleBar(CustomAppWindow* parent)
    : QWidget(parent)
    {
        setMouseTracking(true);
        // make sure its stylesheet background actually paints
        setAttribute(Qt::WA_StyledBackground);
    }

protected:
    void mousePressEvent(QMouseEvent* ev) override {
        if (ev->button() == Qt::LeftButton) {
            if (auto *wh = window()->windowHandle())
                wh->startSystemMove();
            ev->accept();
        }
    }
};

CustomAppWindow::CustomAppWindow(QWidget* centralWidget, const QString& title, QWidget* parent)
: QWidget(parent)
{
    // Frameless, translucent background to allow rounded corners
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    auto* mainLay = new QVBoxLayout(this);
    mainLay->setContentsMargins(2,2,2,2);
    mainLay->setSpacing(0);

    setupTitleBar(title);

    // Content area
    QWidget* content = new QWidget(this);
    content->setStyleSheet(
        "background: #181828;"
        "border-bottom-left-radius: 18px;"
        "border-bottom-right-radius: 18px;"
    );
    auto* contentLay = new QVBoxLayout(content);
    contentLay->setContentsMargins(0,0,0,0);
    contentLay->addWidget(centralWidget);

    m_gripBR = new QSizeGrip(this);
    m_gripBL = new QSizeGrip(this);

    mainLay->addWidget(m_titleBar);
    mainLay->addWidget(content);
    setLayout(mainLay);

    setMinimumSize(400,300);
    resize(1000,700);
    updateSizeGrips();
}

void CustomAppWindow::resizeEvent(QResizeEvent* ev) {
    QWidget::resizeEvent(ev);
    updateSizeGrips();
}

void CustomAppWindow::keyPressEvent(QKeyEvent* ev) {
    if (isMaximized() && ev->key() == Qt::Key_Escape) {
        showNormal();
    } else {
        QWidget::keyPressEvent(ev);
    }
}

void CustomAppWindow::onCloseClicked() {
    close();
}

void CustomAppWindow::onMaximizeClicked() {
    if (isMaximized()) showNormal();
    else              showMaximized();
}

void CustomAppWindow::setupTitleBar(const QString& title) {
    m_titleBar = new TitleBar(this);
    m_titleBar->setFixedHeight(m_titleBarH);
    m_titleBar->setStyleSheet(
        "background: #222;"
        "border-top-left-radius: 18px;"
        "border-top-right-radius: 18px;"
    );

    auto* lay = new QHBoxLayout(m_titleBar);
    lay->setContentsMargins(8,0,8,0);
    lay->setSpacing(4);

    // Text — pass mouse through so TitleBar still catches clicks
    m_titleLabel = new QLabel(title, m_titleBar);
    m_titleLabel->setStyleSheet("color: white; font: bold 16px;");
    m_titleLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    m_titleLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    lay->addWidget(m_titleLabel);

    lay->addStretch();

    // Maximize/Restore
    m_maxBtn = new QPushButton("⬜", m_titleBar);
    m_maxBtn->setFixedSize(28,28);
    m_maxBtn->setStyleSheet(
        "QPushButton { background: #333; color: white; border-radius:14px; font-size:16px; border:none; }"
        "QPushButton:hover { background: #7ecb20; color: #222; }"
    );
    connect(m_maxBtn, &QPushButton::clicked, this, &CustomAppWindow::onMaximizeClicked);
    lay->addWidget(m_maxBtn);

    // Close
    m_closeBtn = new QPushButton("✕", m_titleBar);
    m_closeBtn->setFixedSize(28,28);
    m_closeBtn->setStyleSheet(
        "QPushButton { background: #d32f2f; color: white; border-radius:14px; font-size:16px; border:none; }"
        "QPushButton:hover { background: #ff5252; }"
    );
    connect(m_closeBtn, &QPushButton::clicked, this, &CustomAppWindow::onCloseClicked);
    lay->addWidget(m_closeBtn);

    m_titleBar->setLayout(lay);
}

void CustomAppWindow::updateSizeGrips() {
    constexpr int G = 24;
    m_gripBR->setGeometry(width()-G, height()-G, G, G);
    m_gripBL->setGeometry(0, height()-G, G, G);
}
