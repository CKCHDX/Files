#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QTimer>
#include <QScrollArea>
#include <QHBoxLayout>
#include <QPainter>

class ProcessWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ProcessWidget(const QString& name, const QColor& color, QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QString processName;
    QColor backgroundColor;
};

class TaskManager : public QWidget
{
    Q_OBJECT

public:
    explicit TaskManager(QWidget* parent = nullptr);
    ~TaskManager();

private slots:
    void updateSystemStats();

private:
    QLabel* cpuLabel;
    QLabel* ramLabel;
    QProgressBar* cpuProgress;
    QProgressBar* ramProgress;

    QScrollArea* scrollArea;
    QWidget* processContainer;
    QHBoxLayout* processLayout;

    QWidget* legendWidget;
    QHBoxLayout* legendLayout;

    QVBoxLayout* mainLayout;
    QTimer* updateTimer;

    float lastTotalUser, lastTotalUserLow, lastTotalSys, lastTotalIdle;

    enum ProcessCategory { System, User, PermissionDenied };
    ProcessCategory classifyProcess(const QString& pid);

    float getCpuUsage();
    float getRamUsage();
    void updateProcessList();

    QWidget* createLegendBox(const QColor& color, const QString& label);
};

#endif // TASKMANAGER_H
