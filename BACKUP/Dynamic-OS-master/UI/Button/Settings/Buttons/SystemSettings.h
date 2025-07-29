#ifndef SYSTEMSETTINGS_H
#define SYSTEMSETTINGS_H

#include <QWidget>
#include "performance.h"
#include "storage.h"

class SystemSettings : public QWidget
{
    Q_OBJECT

public:
    // Re-export enum for backward compatibility
    using PerformanceMode = PerformanceWidget::PerformanceMode;
    static constexpr PerformanceMode LowPerformance    = PerformanceWidget::LowPerformance;
    static constexpr PerformanceMode NormalPerformance = PerformanceWidget::NormalPerformance;
    static constexpr PerformanceMode HighPerformance   = PerformanceWidget::HighPerformance;

    explicit SystemSettings(QWidget *parent = nullptr);

    PerformanceMode currentMode() const;
    void setMode(PerformanceMode mode);

signals:
    void performanceModeChanged(PerformanceMode mode);

private:
    PerformanceWidget *performanceWidget;
    DiskUsageWidget *diskUsageWidget;
};

#endif // SYSTEMSETTINGS_H
