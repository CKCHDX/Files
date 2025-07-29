#ifndef POWEROPTIONSUI_H
#define POWEROPTIONSUI_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>

class PowerOptionsUI : public QWidget
{
    Q_OBJECT
public:
    explicit PowerOptionsUI(QWidget* parent = nullptr);

private slots:
    void shutdownSystem();
    void rebootSystem();
    void logoffSystem();

private:
    int showDarkMessageBox(const QString& title, const QString& message);

    QPushButton* powerOffButton;
    QPushButton* rebootButton;
    QPushButton* logOffButton;
};

#endif // POWEROPTIONSUI_H
