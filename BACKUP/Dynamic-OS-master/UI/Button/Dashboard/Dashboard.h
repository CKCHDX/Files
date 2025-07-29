#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QProgressBar>
#include <QTimer>

class Dashboard : public QWidget
{
    Q_OBJECT
public:
    explicit Dashboard(QWidget* parent = nullptr);

private slots:
    void updateSystemStats();

private:
    QLabel* cpuLabel;
    QLabel* ramLabel;
    QLabel* batteryLabel;
    QLabel* uptimeLabel;
    QLabel* diskLabel;
    QLabel* netLabel;
    QProgressBar* cpuBar;
    QProgressBar* ramBar;
    QProgressBar* batteryBar;
    QProgressBar* diskBar;
    QTimer* updateTimer;

    // For CPU calculation
    quint64 lastTotalUser = 0, lastTotalUserLow = 0, lastTotalSys = 0, lastTotalIdle = 0;
    double getCpuUsage();
    bool getRamUsageMB(int& usedMB, int& totalMB); // returns false if error
    int getBatteryPercent();
    QString getUptime();
    double getDiskUsagePercent(QString* details = nullptr);
    void getNetworkStats(quint64& rx, quint64& tx);
};

#endif // DASHBOARD_H
