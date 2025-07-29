#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>
#include <QPushButton>
#include <QStackedWidget>

class KeyboardSettings;
class MouseSettings;
class UpdateSettings;
class SystemSettings;

class Settings : public QWidget
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = nullptr);

signals:
    void performanceModeChanged(int mode);  // Emits SystemSettings::PerformanceMode as int

private slots:
    void showKeyboardSettings();
    void showMouseSettings();
    void showUpdateSettings();
    void showSystemSettings();
    void showAppearanceSettings();
    void showLanguageSettings();

private:
    void setupUI();

    QPushButton *btnKeyboard;
    QPushButton *btnMouse;
    QPushButton *btnUpdate;
    QPushButton *btnSystem;
    QPushButton *btnAppearance;
    QPushButton *btnLanguage;

    QStackedWidget *stackedWidget;

    KeyboardSettings *keyboardSettingsPage;
    MouseSettings *mouseSettingsPage;
    UpdateSettings *updateSettingsPage;
    SystemSettings *systemSettingsPage;
    QWidget *appearanceSettingsPage;
    QWidget *languageSettingsPage;
};

#endif // SETTINGS_H
