#include "MainControl.h"
#include <QMainWindow>
#include <QPushButton>
#include <QWidget>
#include <QDebug>

MainControl::MainControl(QObject* parent)
    : QObject(parent)
{
}

void MainControl::initialize(QMainWindow* mainWin)
{
    if (!mainWin) return;
    m_mainWin = mainWin;

    // 1) Find the buttons by object name:
    homeButton = mainWin->findChild<QPushButton*>("HomeButton");
    appsButton = mainWin->findChild<QPushButton*>("AppsButton");
    // ... and so on for each named button

    // 2) Connect them to your own slots:
    if (homeButton) {
        connect(homeButton, &QPushButton::clicked,
            this, &MainControl::onHomeButtonClicked);
    }
    if (appsButton) {
        connect(appsButton, &QPushButton::clicked,
            this, &MainControl::onAppsButtonClicked);
    }
    // ... etc.

    // 3) (Optional) Re-style them if needed:
    if (homeButton) {
        homeButton->setStyleSheet(
            "QPushButton { background-color: #5858ff; }"
        );
        homeButton->setText("Home (Overridden)");
    }
}

// Slots for your new logic:
void MainControl::onHomeButtonClicked()
{
    qDebug() << "Home button was clicked (from MainControl)!";
    // Possibly do something else, open a new window, etc.
}

void MainControl::onAppsButtonClicked()
{
    qDebug() << "Apps button was clicked (from MainControl)!";
    // ...
}
