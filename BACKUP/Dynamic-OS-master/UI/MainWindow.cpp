#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include "MainWindow.h"
#include "UIEffects.h"
#include "UI/Button/Apps/Apps.h"
#include "UI/Button/Power Options/Power Options.h"
#include <QDebug>
#include <QApplication>
#include "UI/Button/Network/Network.h"
#include "UI/Button/File Manager/File Manager.h"
#include "UI/Button/Built-In Apps/Built-In Apps.h"
#include "UI/Button/Settings/Settings.h"
#include "UI/Button/Task Manager/Task Manager.h"
#include "UI/Button/Built-In Apps/Apps/CustomAppWindow.h"
#include "UI/Button/Built-In Apps/Apps/WebBrowser.h"
#include <QMessageBox>
#include <QProcess>
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      centralWidget(new QWidget(this)),
      uiEffects(new UIEffects(this)),
      contentStack(new QStackedWidget(this)),
      homePage(new Home(this)),
      appsPage(new AppsUI(this)),
      powerOptionsPage(new PowerOptionsUI(this)),
      networkPage(new Network(this)),
      fileManagerPage(new FileManager(this)),
      builtInAppsPage(new BuiltInApps(this)),
      dashboardPage(new Dashboard(this)),
      settingsPage(new Settings(this)),
      taskManagerPage(new TaskManager(this)),
      sidebarOrb(new OrbSidebarButton(this)),
      sidebarOrbHub(nullptr)
{
    setupUI();
    applyStyles();
    setCentralWidget(centralWidget);



    // Map buttons to their pages
    pageMap[appsButton] = appsPage;
    pageMap[homeButton] = homePage;
    pageMap[builtInAppsButton] = builtInAppsPage;
    pageMap[networkButton] = networkPage;
    pageMap[fileManagerButton] = fileManagerPage;
    pageMap[dashboardButton] = dashboardPage;
    pageMap[settingsButton] = settingsPage;
    pageMap[taskManagerButton] = taskManagerPage;



    // Add pages to the stacked widget
    contentStack->addWidget(homePage);
    contentStack->addWidget(appsPage);
    contentStack->addWidget(builtInAppsPage);
    contentStack->addWidget(dashboardPage);
    contentStack->addWidget(fileManagerPage);
    contentStack->addWidget(powerOptionsPage);
    contentStack->addWidget(networkPage);
    contentStack->addWidget(settingsPage);
    contentStack->addWidget(taskManagerPage);
    



    // Set the initial page
    contentStack->setCurrentWidget(homePage);
    showFullScreen();
}


MainWindow::~MainWindow()
{
    
}
bool MainWindow::eventFilter(QObject* obj, QEvent* event) {
    if (obj == sidebarOrbHub && event->type() == QEvent::FocusOut) {
        sidebarOrbHub->hide();
        return true;
    }
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::setupUI()
{
    // Create main layouts
    mainLayout = new QHBoxLayout;
    sidebarLayout = new QVBoxLayout;

    // Create QStackedWidget if not already created
    // (We already did it in the constructor initialization list.)

    // Initialize sidebar buttons
    homeButton = new QPushButton("Home");
    appsButton = new QPushButton("Apps");
    builtInAppsButton = new QPushButton("Built-In Apps");
    dashboardButton = new QPushButton("Dashboard");
    settingsButton = new QPushButton("Settings");
    fileManagerButton = new QPushButton("File Manager");
    taskManagerButton = new QPushButton("Task Manager");
    powerOptionsButton = new QPushButton("Power Options");
    networkButton = new QPushButton("Network");
    sidebarOrbHub = new Orb(nullptr);
    sidebarOrbHub->installEventFilter(this);
    sidebarOrbHub->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    sidebarOrbHub->setAttribute(Qt::WA_TranslucentBackground, true);
    sidebarOrbHub->hide();
    // Spacing & sidebar arrangement
    sidebarLayout->setSpacing(15);
    sidebarLayout->addWidget(homeButton);
    sidebarLayout->addWidget(appsButton);
    sidebarLayout->addWidget(builtInAppsButton);
    sidebarLayout->addWidget(dashboardButton);
    sidebarLayout->addWidget(settingsButton);
    sidebarLayout->addWidget(fileManagerButton);
    sidebarLayout->addWidget(taskManagerButton);
    sidebarLayout->addWidget(powerOptionsButton);
    sidebarLayout->addWidget(networkButton);
    sidebarLayout->addWidget(sidebarOrb, 0, Qt::AlignHCenter);
    sidebarLayout->addWidget(taskbarWidget);
    sidebarLayout->addStretch();

    // Combine sidebar & stack in main layout
    mainLayout->addLayout(sidebarLayout, 1);
    mainLayout->addWidget(contentStack, 4);
    sidebarOrb->setVisible(false);
    centralWidget->setLayout(mainLayout);

    //
    // Connect signals/slots
    //
    connect(homeButton, &QPushButton::clicked, this, &MainWindow::onHomeClicked);
    connect(homePage, &Home::appRequested, this, [this](const QString& app) {
    if (app == "Terminal") {
        sidebarOrbHub->hide();
        // Try common Linux terminals, just as in BuiltInApps
        QStringList terms = { "konsole", "gnome-terminal", "xterm", "lxterminal", "xfce4-terminal", "mate-terminal", "tilix", "alacritty", "urxvt" };
        bool launched = false;
        for (const QString& term : terms) {
            if (QProcess::startDetached(term)) {
                launched = true;
                break;
            }
        }
        if (!launched) {
            QMessageBox::warning(this, "Terminal Not Found", "No supported terminal emulator was found on your system.");
        }
    } else if (app == "Web Browser") {
        // Launch the built-in web browser in a custom window
        CustomAppWindow* window = new CustomAppWindow(new WebBrowser, "Web Browser");
        window->setAttribute(Qt::WA_DeleteOnClose);
        window->show();
        sidebarOrbHub->hide();
    } else if (app == "Media") {
        // Example: use WebBrowser for demo, or substitute your real media widget
        // Replace WebBrowser with MediaPlayerWidget or your media app
        CustomAppWindow* window = new CustomAppWindow(new WebBrowser, "Media Player");
        window->setAttribute(Qt::WA_DeleteOnClose);
        window->show();
        sidebarOrbHub->hide();
        // TODO: Replace with actual media widget when available
    } else if (app == "Dev Suite") {
        // Example: use Notes or Calculator as stub, or your dev/IDE QWidget
        CustomAppWindow* window = new CustomAppWindow(new WebBrowser, "Dev Suite");
        window->setAttribute(Qt::WA_DeleteOnClose);
        window->show();
        sidebarOrbHub->hide();
        // TODO: Replace with real dev suite widget in future
    }
    });
    connect(sidebarOrb, &OrbSidebarButton::clicked, this, [this]() {
    if (!sidebarOrbHub->isVisible()) {
        // Position the pop-up next to the sidebar orb button, centered
        QPoint globalOrb = sidebarOrb->mapToGlobal(QPoint(sidebarOrb->width()/2, sidebarOrb->height()/2));
        int popupW = sidebarOrbHub->width();
        int popupH = sidebarOrbHub->height();
        int x = globalOrb.x() - popupW/2 + 4;
        int y = globalOrb.y() - popupH/2 + 4;
        sidebarOrbHub->move(x, y);
        sidebarOrbHub->reset();
        sidebarOrbHub->show();
        sidebarOrbHub->raise();
        sidebarOrbHub->openHub();
    } else {
        sidebarOrbHub->hide();
    }
    });

    
    connect(appsButton, &QPushButton::clicked, this, &MainWindow::onAppsClicked);

    connect(builtInAppsButton, &QPushButton::clicked, this, &MainWindow::onBuiltInAppsClicked);
    connect(dashboardButton, &QPushButton::clicked, this, &MainWindow::onDashboardClicked);
    connect(settingsButton, &QPushButton::clicked, this, &MainWindow::onSettingsClicked);
    connect(settingsPage, &Settings::performanceModeChanged, this, &MainWindow::onPerformanceModeChanged);

    connect(fileManagerButton, &QPushButton::clicked, this, &MainWindow::onFileManagerClicked);
    connect(taskManagerButton, &QPushButton::clicked, this, &MainWindow::onTaskManagerClicked);
    connect(powerOptionsButton, &QPushButton::clicked, this, &MainWindow::onPowerOptionsClicked);
    connect(networkButton, &QPushButton::clicked, this, &MainWindow::onNetworkClicked);
    
    taskbarWidget = new TaskbarWidget(this);
    sidebarLayout->addWidget(taskbarWidget);
    sidebarOrbHub->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);
    sidebarOrbHub->setAttribute(Qt::WA_TranslucentBackground, true);
    sidebarOrbHub->hide();


}

void MainWindow::applyStyles()
{
    setStyleSheet(
        "QMainWindow { "
        "  background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, "
        "              stop:0 #1e1e1e, stop:1 #2d2d30); "
        "}"
        "QPushButton { "
        "  background-color: rgba(255, 255, 255, 0.1); "
        "  color: white; "
        "  border-radius: 15px; "
        "  border: 1px solid #3c3c3c; "
        "  padding: 12px; "
        "  font-size: 16px; "
        "  transition: all 0.2s ease-in-out; "
        "}"
        "QPushButton:hover { "
        "  background-color: rgba(255, 255, 255, 0.2); "
        "  box-shadow: 0px 0px 15px rgba(255, 255, 255, 0.5); "
        "  transform: scale(1.05); "
        "}"
        "QPushButton:pressed { "
        "  background-color: rgba(255, 255, 255, 0.3); "
        "  transform: scale(0.95); "
        "}"
        "QLabel { "
        "  color: white; "
        "  font-size: 20px; "
        "}"
    );
}

// Slot implementations
void MainWindow::onHomeClicked()
{
    sidebarOrb->setVisible(false);
    homePage->reset();
    uiEffects->fadeOut(contentStack->currentWidget(), 500);
    contentStack->setCurrentWidget(homePage);
    uiEffects->fadeIn(contentStack->currentWidget(), 500);
}

void MainWindow::onAppsClicked()
{
    sidebarOrb->setVisible(true);
    homePage->reset();
    uiEffects->fadeOut(contentStack->currentWidget(), 500);
    contentStack->setCurrentWidget(appsPage);
    uiEffects->fadeIn(contentStack->currentWidget(), 500);
}
void MainWindow::onBuiltInAppsClicked()
{
    uiEffects->fadeOut(contentStack->currentWidget(), 500);
    contentStack->setCurrentWidget(builtInAppsPage);
    uiEffects->fadeIn(contentStack->currentWidget(), 500);
}


void MainWindow::onDashboardClicked()
{
    homePage->reset();
    uiEffects->fadeOut(contentStack->currentWidget(), 500);
    contentStack->setCurrentWidget(dashboardPage);
    uiEffects->fadeIn(contentStack->currentWidget(), 500);
}

void MainWindow::onSettingsClicked()
{
    homePage->reset();
    uiEffects->fadeOut(contentStack->currentWidget(), 500);
    contentStack->setCurrentWidget(settingsPage);  // Was: homePage
    uiEffects->fadeIn(contentStack->currentWidget(), 500);
}


void MainWindow::onFileManagerClicked()
{
    homePage->reset();
    uiEffects->fadeOut(contentStack->currentWidget(), 500);
    contentStack->setCurrentWidget(fileManagerPage);
    uiEffects->fadeIn(contentStack->currentWidget(), 500);
}


void MainWindow::onTaskManagerClicked()
{
    homePage->reset();
    uiEffects->fadeOut(contentStack->currentWidget(), 500);
    contentStack->setCurrentWidget(taskManagerPage);  // Was: homePage
    uiEffects->fadeIn(contentStack->currentWidget(), 500);
}


void MainWindow::onPowerOptionsClicked()
{
    homePage->reset();
    uiEffects->fadeOut(contentStack->currentWidget(), 500);
    contentStack->setCurrentWidget(powerOptionsPage);
    uiEffects->fadeIn(contentStack->currentWidget(), 500);
}

void MainWindow::onNetworkClicked()
{
    homePage->reset();
    uiEffects->fadeOut(contentStack->currentWidget(), 500);
    contentStack->setCurrentWidget(networkPage);
    uiEffects->fadeIn(contentStack->currentWidget(), 500);
}
void MainWindow::onPerformanceModeChanged(int mode)
{
    currentPerformanceMode = static_cast<SystemSettings::PerformanceMode>(mode);
    applyPerformanceMode();
}

void MainWindow::applyPerformanceMode()
{
    switch (currentPerformanceMode) {
        case SystemSettings::LowPerformance:
            uiEffects->setAnimationsEnabled(false);
            break;
        case SystemSettings::NormalPerformance:
            uiEffects->setAnimationsEnabled(true);
            uiEffects->setAnimationDuration(300);
            break;
        case SystemSettings::HighPerformance:
            uiEffects->setAnimationsEnabled(true);
            uiEffects->setAnimationDuration(500);
            break;
    }
}


