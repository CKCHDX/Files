#include "Settings.h"

#include "Buttons/KeyboardSettings.h"
#include "Buttons/MouseSettings.h"
#include "Buttons/UpdateSettings.h"
#include "Buttons/SystemSettings.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>

Settings::Settings(QWidget *parent)
: QWidget(parent)
{
    setupUI();
}

void Settings::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Buttons layout
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    btnKeyboard = new QPushButton("Keyboard", this);
    btnMouse = new QPushButton("Mouse", this);
    btnUpdate = new QPushButton("Update", this);
    btnSystem = new QPushButton("System", this);
    btnAppearance = new QPushButton("Appearance", this);
    btnLanguage = new QPushButton("Language", this);

    buttonLayout->addWidget(btnKeyboard);
    buttonLayout->addWidget(btnMouse);
    buttonLayout->addWidget(btnUpdate);
    buttonLayout->addWidget(btnSystem);
    buttonLayout->addWidget(btnAppearance);
    buttonLayout->addWidget(btnLanguage);

    mainLayout->addLayout(buttonLayout);

    // Stacked widget to hold different settings pages
    stackedWidget = new QStackedWidget(this);

    keyboardSettingsPage = new KeyboardSettings(this);
    mouseSettingsPage = new MouseSettings(this);
    updateSettingsPage = new UpdateSettings(this);
    systemSettingsPage = new SystemSettings(this);
    appearanceSettingsPage = new QWidget(this);  // Placeholder, implement later
    languageSettingsPage = new QWidget(this);    // Placeholder, implement later

    stackedWidget->addWidget(keyboardSettingsPage);  // index 0
    stackedWidget->addWidget(mouseSettingsPage);     // index 1
    stackedWidget->addWidget(updateSettingsPage);    // index 2
    stackedWidget->addWidget(systemSettingsPage);    // index 3
    stackedWidget->addWidget(appearanceSettingsPage); // index 4
    stackedWidget->addWidget(languageSettingsPage);   // index 5

    mainLayout->addWidget(stackedWidget);

    // Connect buttons to slots to switch pages
    connect(btnKeyboard, &QPushButton::clicked, this, &Settings::showKeyboardSettings);
    connect(btnMouse, &QPushButton::clicked, this, &Settings::showMouseSettings);
    connect(btnUpdate, &QPushButton::clicked, this, &Settings::showUpdateSettings);
    connect(btnSystem, &QPushButton::clicked, this, &Settings::showSystemSettings);
    connect(btnAppearance, &QPushButton::clicked, this, &Settings::showAppearanceSettings);
    connect(btnLanguage, &QPushButton::clicked, this, &Settings::showLanguageSettings);

    // Connect system settings mode change signal to relay it outside Settings
    connect(systemSettingsPage, &SystemSettings::performanceModeChanged,
            this, [this](SystemSettings::PerformanceMode mode){
                emit performanceModeChanged(static_cast<int>(mode));
                qDebug() << "Settings: Performance mode changed to"
                << (mode == SystemSettings::LowPerformance ? "Low" : "High");
            });

    // Start with keyboard settings shown by default
    stackedWidget->setCurrentIndex(0);
}

void Settings::showKeyboardSettings()
{
    stackedWidget->setCurrentIndex(0);
}

void Settings::showMouseSettings()
{
    stackedWidget->setCurrentIndex(1);
}

void Settings::showUpdateSettings()
{
    stackedWidget->setCurrentIndex(2);
}

void Settings::showSystemSettings()
{
    stackedWidget->setCurrentIndex(3);
}

void Settings::showAppearanceSettings()
{
    stackedWidget->setCurrentIndex(4);
}

void Settings::showLanguageSettings()
{
    stackedWidget->setCurrentIndex(5);
}
