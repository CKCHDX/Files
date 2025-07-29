#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QStackedWidget>
#include "UI/Button/Power Options/Power Options.h"
#include "UIEffects.h"
#include "UI/Button/Apps/Apps.h"  // Ensure this includes AppsUI class
#include "UI/Button/Network/Network.h"
#include "UI/Button/File Manager/File Manager.h"
#include "UI/Button/Built-In Apps/Built-In Apps.h"
#include "UI/Button/Dashboard/Dashboard.h"
#include "UI/TaskbarWidget.h"
#include "UI/Button/Settings/Settings.h"
#include "UI/Button/Task Manager/Task Manager.h"
#include "SystemSettings.h"
#include "UI/Button/Home/Home.h"
#include "UI/Button/OrbSidebarButton.h"
#include "UI/Button/Home/Orb.h"
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
private:
    QWidget* centralWidget;
    QVBoxLayout* sidebarLayout;
    QHBoxLayout* mainLayout;
    QStackedWidget* contentStack;
    UIEffects* uiEffects;
    PowerOptionsUI* powerOptionsPage;
    Network* networkPage;
    TaskManager* taskManagerPage;
    Orb* sidebarOrbHub = nullptr;
    FileManager* fileManagerPage;
    BuiltInApps* builtInAppsPage;
    Dashboard* dashboardPage;
    QPushButton* homeButton;
    QPushButton* appsButton;
    QPushButton* builtInAppsButton;
    QPushButton* dashboardButton;
    QPushButton* settingsButton;
    QPushButton* fileManagerButton;
    QPushButton* taskManagerButton;
    QPushButton* powerOptionsButton;
    QPushButton* networkButton;
    TaskbarWidget* taskbarWidget = nullptr;
    Settings* settingsPage;
    OrbSidebarButton* sidebarOrb = nullptr;
    Home* homePage;

    AppsUI* appsPage;  // FIX: Change Apps* to AppsUI*

    QMap<QPushButton*, QWidget*> pageMap;

    void setupUI();
    void applyStyles();
    void openHub();
    SystemSettings::PerformanceMode currentPerformanceMode = SystemSettings::HighPerformance;
    void applyPerformanceMode();

private slots:
    void onHomeClicked();
    void onAppsClicked();
    void onBuiltInAppsClicked();
    void onDashboardClicked();
    void onSettingsClicked();
    void onFileManagerClicked();
    void onTaskManagerClicked();
    void onPowerOptionsClicked();
    void onNetworkClicked();
    void onPerformanceModeChanged(int mode);
};


#endif // MAINWINDOW_H
