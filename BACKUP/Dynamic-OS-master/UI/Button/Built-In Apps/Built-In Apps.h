#ifndef BUILT_IN_APPS_H
#define BUILT_IN_APPS_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

class BuiltInApps : public QWidget
{
    Q_OBJECT
public:
    explicit BuiltInApps(QWidget* parent = nullptr);

private slots:
    void onWebBrowserClicked();
    void onCalculatorClicked();
    void onNotesClicked();
    void onTerminalClicked();

private:
    QPushButton* webBrowserButton;
    QPushButton* calculatorButton;
    QPushButton* notesButton;
    QPushButton* terminalButton;
};

#endif // BUILT_IN_APPS_H
