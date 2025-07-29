#include "Task Manager.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDebug>
#include <QCoreApplication>
#include <unistd.h>  // for getuid()

// ----- ProcessWidget implementation -----

ProcessWidget::ProcessWidget(const QString& name, const QColor& color, QWidget* parent)
: QWidget(parent), processName(name), backgroundColor(color)
{
    setFixedSize(140, 70);
    setToolTip(name);
}

void ProcessWidget::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw rounded rectangle background
    painter.setBrush(backgroundColor);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect(), 15, 15);

    // Draw process name text centered
    painter.setPen(Qt::white);
    QFont font = painter.font();
    font.setBold(true);
    font.setPointSize(10);
    painter.setFont(font);

    QRect textRect = rect();
    painter.drawText(textRect, Qt::AlignCenter, processName);
}

// ----- TaskManager implementation -----

TaskManager::TaskManager(QWidget* parent)
: QWidget(parent),
lastTotalUser(0), lastTotalUserLow(0), lastTotalSys(0), lastTotalIdle(0)
{
    cpuLabel = new QLabel("CPU Usage:");
    ramLabel = new QLabel("RAM Usage:");

    cpuProgress = new QProgressBar();
    ramProgress = new QProgressBar();
    cpuProgress->setRange(0, 100);
    ramProgress->setRange(0, 100);

    // Scroll area and horizontal layout for process widgets
    processContainer = new QWidget();
    processLayout = new QHBoxLayout(processContainer);
    processLayout->setSpacing(10);
    processLayout->setContentsMargins(10, 10, 10, 10);
    processLayout->addStretch();

    scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(processContainer);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setFixedHeight(100);

    // Legend box for colors
    legendWidget = new QWidget();
    legendLayout = new QHBoxLayout(legendWidget);
    legendLayout->setSpacing(20);
    legendLayout->setContentsMargins(10, 5, 10, 5);

    legendLayout->addWidget(createLegendBox(QColor(50, 120, 230), "System Process"));
    legendLayout->addWidget(createLegendBox(QColor(220, 50, 50), "User Process"));
    legendLayout->addWidget(createLegendBox(QColor(120, 120, 120), "Permission Denied"));

    mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(cpuLabel);
    mainLayout->addWidget(cpuProgress);
    mainLayout->addWidget(ramLabel);
    mainLayout->addWidget(ramProgress);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(new QLabel("Running Processes:"));
    mainLayout->addWidget(scrollArea);
    mainLayout->addWidget(legendWidget);

    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &TaskManager::updateSystemStats);
    updateTimer->start(2000);

    updateSystemStats();
}

TaskManager::~TaskManager()
{
}

QWidget* TaskManager::createLegendBox(const QColor& color, const QString& label)
{
    QWidget* widget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(widget);
    layout->setContentsMargins(5, 0, 5, 0);
    layout->setSpacing(5);

    // Colored square
    QLabel* colorLabel = new QLabel();
    colorLabel->setFixedSize(20, 20);
    colorLabel->setStyleSheet(QString("background-color: %1; border-radius: 4px;").arg(color.name()));

    QLabel* textLabel = new QLabel(label);

    layout->addWidget(colorLabel);
    layout->addWidget(textLabel);

    return widget;
}

void TaskManager::updateSystemStats()
{
    float cpu = getCpuUsage();
    float ram = getRamUsage();

    cpuLabel->setText(QString("CPU Usage: %1%").arg(cpu, 0, 'f', 1));
    ramLabel->setText(QString("RAM Usage: %1%").arg(ram, 0, 'f', 1));

    cpuProgress->setValue(static_cast<int>(cpu));
    ramProgress->setValue(static_cast<int>(ram));

    updateProcessList();
}

float TaskManager::getCpuUsage()
{
    QFile file("/proc/stat");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return 0.0f;

    QTextStream in(&file);
    QString line = in.readLine();
    QStringList values = line.split(QRegExp("\\s+"), Qt::SkipEmptyParts);

    if (values.isEmpty() || values[0] != "cpu")
        return 0.0f;

    float user = values[1].toFloat();
    float nice = values[2].toFloat();
    float system = values[3].toFloat();
    float idle = values[4].toFloat();

    float totalUser = user;
    float totalUserLow = nice;
    float totalSys = system;
    float totalIdle = idle;

    float total = (totalUser - lastTotalUser) +
    (totalUserLow - lastTotalUserLow) +
    (totalSys - lastTotalSys);

    float totalAll = total + (totalIdle - lastTotalIdle);

    float cpuUsage = (totalAll != 0) ? (total / totalAll) * 100.0f : 0.0f;

    lastTotalUser = totalUser;
    lastTotalUserLow = totalUserLow;
    lastTotalSys = totalSys;
    lastTotalIdle = totalIdle;

    return cpuUsage;
}

float TaskManager::getRamUsage()
{
    QFile file("/proc/meminfo");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return 0.0f;

    QTextStream in(&file);
    QString memTotalLine = in.readLine();  // MemTotal
    QString memFreeLine = in.readLine();   // MemFree
    QString buffersLine = in.readLine();   // Buffers
    QString cachedLine = in.readLine();    // Cached

    int memTotal = memTotalLine.split(QRegExp("\\s+"), Qt::SkipEmptyParts)[1].toInt();
    int memFree = memFreeLine.split(QRegExp("\\s+"), Qt::SkipEmptyParts)[1].toInt();
    int buffers = buffersLine.split(QRegExp("\\s+"), Qt::SkipEmptyParts)[1].toInt();
    int cached = cachedLine.split(QRegExp("\\s+"), Qt::SkipEmptyParts)[1].toInt();

    int used = memTotal - (memFree + buffers + cached);
    return static_cast<float>(used) / memTotal * 100.0f;
}

TaskManager::ProcessCategory TaskManager::classifyProcess(const QString& pid)
{
    QFile statusFile("/proc/" + pid + "/status");
    if (!statusFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return PermissionDenied;

    QTextStream in(&statusFile);
    QString line;
    uid_t uid = (uid_t)-1;

    while (!in.atEnd()) {
        line = in.readLine();
        if (line.startsWith("Uid:")) {
            QStringList parts = line.split(QRegExp("\\s+"), Qt::SkipEmptyParts);
            if (parts.size() >= 2) {
                uid = parts[1].toUInt();
                break;
            }
        }
    }

    if (uid == (uid_t)-1)
        return PermissionDenied;

    uid_t currentUid = getuid();

    if (uid == 0)
        return System;
    else if (uid == currentUid)
        return User;

    return System;
}

void TaskManager::updateProcessList()
{
    QDir procDir("/proc");
    QStringList entries = procDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    // Clear old widgets
    QLayoutItem* child;
    while ((child = processLayout->takeAt(0)) != nullptr) {
        if (child->widget()) {
            child->widget()->setParent(nullptr);
            delete child->widget();
        }
        delete child;
    }
    processLayout->addStretch();

    for (const QString& entry : entries)
    {
        bool ok;
        int pid = entry.toInt(&ok);
        if (!ok) continue;

        ProcessCategory category = classifyProcess(entry);

        QString procName = "Unknown";

        if (category == PermissionDenied)
        {
            // Can't read /proc/<pid>/status, show "Unknown" but gray color
            procName = "Unknown";
        }
        else
        {
            QFile statusFile("/proc/" + entry + "/status");
            if (statusFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&statusFile);
                while (!in.atEnd()) {
                    QString line = in.readLine();
                    if (line.startsWith("Name:")) {
                        procName = line.section(":", 1).trimmed();
                        break;
                    }
                }
            }
        }

        QColor color;
        switch (category) {
            case System: color = QColor(50, 120, 230); break;          // Blue
            case User: color = QColor(220, 50, 50); break;             // Red
            case PermissionDenied: color = QColor(120, 120, 120); break; // Gray
        }

        ProcessWidget* procWidget = new ProcessWidget(procName, color);
        processLayout->insertWidget(processLayout->count() - 1, procWidget);
    }
}
