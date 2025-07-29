#ifndef TASKBARWIDGET_H
#define TASKBARWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QToolButton>
#include <QTimer>
#include <QList>

struct UserAppInfo {
    QString windowTitle;
    QString processName;
    QString windowId;
};

class TaskbarWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TaskbarWidget(QWidget* parent = nullptr);

private slots:
    void updateTaskbarApps();
    void onAppButtonClicked();
    void onLeftArrow();
    void onRightArrow();

private:
    QHBoxLayout* layout;
    QToolButton* leftArrow;
    QToolButton* rightArrow;
    QList<UserAppInfo> userApps;
    int offset = 0;
    QTimer* updateTimer;

    void refreshTaskbar();
};

#endif // TASKBARWIDGET_H
