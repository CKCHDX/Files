#include "UI/Button/Apps/Manageapp/Manageapp.h"
#include <QApplication>
#include <QStackedWidget>
#include <QScreen>

ManageApp::ManageApp(QWidget* parent)
    : QWidget(nullptr), isMaximizedState(false) // Ensure it's an independent window
{
    // Fix: Ensure ManageApp always stays on top while keeping the original design
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);

    setStyleSheet(
        "background-color: #222; "
        "border: 2px solid #555; "
        "border-radius: 5px;"
    );

    setMinimumSize(400, 300);
    resize(800, 500);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(2, 2, 2, 2);

    // Title bar (Restored original style)
    titleBar = new QWidget(this);
    titleBar->setStyleSheet(
        "background-color: #333; "
        "border-bottom: 2px solid #555;"
        "padding: 5px;"
    );
    titleBar->setFixedHeight(40);

    QHBoxLayout* titleLayout = new QHBoxLayout(titleBar);
    titleLayout->setContentsMargins(10, 0, 10, 0);

    titleLabel = new QLabel("Manage Apps", this);
    titleLabel->setStyleSheet("color: white; font-size: 16px; font-weight: bold;");

    minimizeButton = new QPushButton("-", this);
    minimizeButton->setStyleSheet(
        "background-color: transparent; "
        "color: white; "
        "font-size: 18px; "
        "border: none; "
        "padding: 2px;"
    );
    minimizeButton->setFixedSize(30, 30);
    connect(minimizeButton, &QPushButton::clicked, this, &ManageApp::minimizeWindow);

    closeButton = new QPushButton("✖", this);
    closeButton->setStyleSheet(
        "background-color: #900; "
        "color: white; "
        "font-size: 16px; "
        "border-radius: 5px;"
    );
    closeButton->setFixedSize(30, 30);
    connect(closeButton, &QPushButton::clicked, this, &ManageApp::closeWindow);

    titleLayout->addWidget(titleLabel);
    titleLayout->addStretch();
    titleLayout->addWidget(minimizeButton);
    titleLayout->addWidget(closeButton);

    titleBar->setLayout(titleLayout);

    // Navigation Buttons (Restored original style)
    QWidget* buttonPanel = new QWidget(this);
    buttonPanel->setStyleSheet("background-color: #292929; border-bottom: 2px solid #555;");
    QHBoxLayout* buttonLayout = new QHBoxLayout(buttonPanel);

    addAppButton = new QPushButton("Add App", this);
    manageAppButton = new QPushButton("Manage App", this);
    uninstallAppButton = new QPushButton("Uninstall App", this);

    addAppButton->setStyleSheet(
        "background-color: #444; "
        "color: white; "
        "padding: 8px; "
        "border-radius: 8px; "
        "border: 1px solid #666;"
    );
    manageAppButton->setStyleSheet(
        "background-color: #444; "
        "color: white; "
        "padding: 8px; "
        "border-radius: 8px; "
        "border: 1px solid #666;"
    );
    uninstallAppButton->setStyleSheet(
        "background-color: #444; "
        "color: white; "
        "padding: 8px; "
        "border-radius: 8px; "
        "border: 1px solid #666;"
    );

    buttonLayout->addWidget(addAppButton);
    buttonLayout->addWidget(manageAppButton);
    buttonLayout->addWidget(uninstallAppButton);
    buttonPanel->setLayout(buttonLayout);

    // Content Pages (Restored original styling)
    contentStack = new QStackedWidget(this);
    addAppPage = new AddApp(this);
    manageAppPage = new ManageAppPage(this);
    uninstallAppPage = new UninstallApp(this);

    contentStack->addWidget(addAppPage);
    contentStack->addWidget(manageAppPage);
    contentStack->addWidget(uninstallAppPage);

    connect(addAppButton, &QPushButton::clicked, this, &ManageApp::showAddAppPage);
    connect(manageAppButton, &QPushButton::clicked, this, &ManageApp::showManageAppPage);
    connect(uninstallAppButton, &QPushButton::clicked, this, &ManageApp::showUninstallAppPage);

    mainLayout->addWidget(titleBar);
    mainLayout->addWidget(buttonPanel);
    mainLayout->addWidget(contentStack);

    setLayout(mainLayout);

    // Ensure the window is brought to the front on creation
    raise();
    activateWindow();
}

// Prevent MainWindow from overriding ManageApp
void ManageApp::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);
    show();
}

// Fix: Define resizeEvent to prevent errors
void ManageApp::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
}

// Navigation slots
void ManageApp::showAddAppPage() { contentStack->setCurrentWidget(addAppPage); }
void ManageApp::showManageAppPage() { contentStack->setCurrentWidget(manageAppPage); }
void ManageApp::showUninstallAppPage() { contentStack->setCurrentWidget(uninstallAppPage); }

void ManageApp::closeWindow() { this->close(); }
void ManageApp::minimizeWindow() { this->showMinimized(); }


void ManageApp::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && titleBar->geometry().contains(event->pos())) {
        dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void ManageApp::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPos() - dragPosition);
        event->accept();
    }
}

void ManageApp::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (titleBar->geometry().contains(event->pos())) {
        toggleMaximizeRestore();
    }
}

void ManageApp::toggleMaximizeRestore()
{
    if (isMaximizedState) {
        // Restore to original size
        resize(800, 500);
        move(100, 100);
    }
    else {
        // Fix: Ensure it resizes safely by checking the screen geometry
        QRect screenGeometry = QApplication::primaryScreen()->geometry();
        setGeometry(screenGeometry);
    }

    isMaximizedState = !isMaximizedState;
}
