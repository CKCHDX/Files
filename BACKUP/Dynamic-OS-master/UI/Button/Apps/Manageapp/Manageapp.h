#ifndef MANAGE_APP_H
#define MANAGE_APP_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QMouseEvent>
#include <QSizeGrip>
#include <QPropertyAnimation>
#include <QResizeEvent>
#include <QShowEvent>  // Ensure this is included

// Include content pages
#include "Manageapp/Content/Addapp/AddApp.h"
#include "Manageapp/Content/Manageapp/ManageAppPage.h"
#include "Manageapp/Content/uninstallapp/UninstallApp.h"

class ManageApp : public QWidget
{
    Q_OBJECT
public:
    explicit ManageApp(QWidget* parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void showEvent(QShowEvent* event) override;  // Ensure this function is declared

private slots:
    void closeWindow();
    void minimizeWindow();
    void toggleMaximizeRestore();
    void showAddAppPage();
    void showManageAppPage();
    void showUninstallAppPage();

private:
    QPushButton* closeButton;
    QPushButton* minimizeButton;
    QLabel* titleLabel;
    QPoint dragPosition;
    QWidget* titleBar;
    QSizeGrip* sizeGrip;
    bool isMaximizedState;

    QStackedWidget* contentStack;
    QPushButton* addAppButton;
    QPushButton* manageAppButton;
    QPushButton* uninstallAppButton;

    AddApp* addAppPage;
    ManageAppPage* manageAppPage;
    UninstallApp* uninstallAppPage;
};

#endif // MANAGE_APP_H
