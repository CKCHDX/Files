#include "Apps.h"
#include "Button/Apps/Manageapp/Content/Manageapp/ManageAppPage.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

AppsUI::AppsUI(QWidget* parent)
    : QWidget(parent), manageAppWindow(new ManageApp(this)) // Ensure ManageApp is instantiated correctly
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    QLabel* label = new QLabel("Apps Page", this);
    label->setStyleSheet("color: white; font-size: 18px;");

    // Create "Manage Apps" button
    manageAppsButton = new QPushButton("Manage Apps", this);
    manageAppsButton->setStyleSheet(
        "background-color: #444; color: white; border-radius: 10px; padding: 10px;"
        "border: 1px solid #666; font-size: 14px;");
    manageAppsButton->setFixedSize(120, 40);

    // Button layout (top-right corner)
    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->addStretch();
    topLayout->addWidget(manageAppsButton);

    // Add elements to layout
    layout->addLayout(topLayout);
    layout->addWidget(label);
    setLayout(layout);

    connect(manageAppsButton, &QPushButton::clicked, this, &AppsUI::openManageAppsWindow);
}

void AppsUI::openManageAppsWindow()
{
    manageAppWindow->show();
}
