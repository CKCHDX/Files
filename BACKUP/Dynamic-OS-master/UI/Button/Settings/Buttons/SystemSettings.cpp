#include "SystemSettings.h"
#include <QVBoxLayout>
#include <QGroupBox>

SystemSettings::SystemSettings(QWidget *parent)
: QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);

    // Performance settings
    auto *performanceGroup = new QGroupBox("Performance Mode", this);
    auto *performanceLayout = new QVBoxLayout(performanceGroup);
    performanceWidget = new PerformanceWidget(this);
    performanceLayout->addWidget(performanceWidget);
    performanceGroup->setLayout(performanceLayout);
    layout->addWidget(performanceGroup);

    // Signal forwarding
    connect(performanceWidget, &PerformanceWidget::performanceModeChanged,
            this, &SystemSettings::performanceModeChanged);

    // Storage settings
    auto *storageGroup = new QGroupBox("Disk Usage Overview", this);
    auto *storageLayout = new QVBoxLayout(storageGroup);
    diskUsageWidget = new DiskUsageWidget(this);
    storageLayout->addWidget(diskUsageWidget);
    storageGroup->setLayout(storageLayout);
    layout->addWidget(storageGroup);
}

SystemSettings::PerformanceMode SystemSettings::currentMode() const
{
    return performanceWidget->currentMode();
}

void SystemSettings::setMode(SystemSettings::PerformanceMode mode)
{
    performanceWidget->setMode(mode);
}
