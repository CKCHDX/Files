#ifndef APPS_UI_H
#define APPS_UI_H

#include <QWidget>
#include <QPushButton>
#include "UI/Button/Apps/Manageapp/Manageapp.h"

class AppsUI : public QWidget
{
    Q_OBJECT
public:
    explicit AppsUI(QWidget* parent = nullptr);

private slots:
    void openManageAppsWindow();

private:
    QPushButton* manageAppsButton;
    ManageApp* manageAppWindow;
};

#endif // APPS_UI_H
