#ifndef MAINCONTROL_H
#define MAINCONTROL_H

#include <QObject>
#include <QPointer>

class QMainWindow;
class QPushButton;

class MainControl : public QObject
{
    Q_OBJECT
public:
    explicit MainControl(QObject* parent = nullptr);

    // The method you'll call once you have the pointer to the MainWindow
    void initialize(QMainWindow* mainWin);

private:
    QPointer<QMainWindow> m_mainWin;

    // Example: references to your found buttons
    QPushButton* homeButton = nullptr;
    QPushButton* appsButton = nullptr;
    // ... and so on if you want direct pointers

private slots:
    // Example: your new slots for button actions
    void onHomeButtonClicked();
    void onAppsButtonClicked();
    // ...
};

#endif // MAINCONTROL_H
