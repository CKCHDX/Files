#include "Dashboard.h"
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <QDir>
#include <QStorageInfo>
#include <QDateTime>

Dashboard::Dashboard(QWidget* parent)
: QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    QLabel* title = new QLabel("System Dashboard", this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 24px; font-weight: bold; margin: 10px;");

    cpuLabel = new QLabel("CPU Usage: --%", this);
    ramLabel = new QLabel("RAM Usage: --", this);
    batteryLabel = new QLabel("Battery: --%", this);
    uptimeLabel = new QLabel("Uptime: --", this);
    diskLabel = new QLabel("Disk: --", this);
    netLabel = new QLabel("Network: --", this);

    cpuBar = new QProgressBar(this);
    ramBar = new QProgressBar(this);
    batteryBar = new QProgressBar(this);
    diskBar = new QProgressBar(this);

    cpuBar->setRange(0, 100);
    ramBar->setRange(0, 100);
    batteryBar->setRange(0, 100);
    diskBar->setRange(0, 100);

    layout->addWidget(title);
    layout->addWidget(cpuLabel);
    layout->addWidget(cpuBar);
    layout->addWidget(ramLabel);
    layout->addWidget(ramBar);
    layout->addWidget(batteryLabel);
    layout->addWidget(batteryBar);
    layout->addWidget(uptimeLabel);
    layout->addWidget(diskLabel);
    layout->addWidget(diskBar);
    layout->addWidget(netLabel);
    layout->addStretch();

    setLayout(layout);

    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &Dashboard::updateSystemStats);
    updateTimer->start(2000);

    updateSystemStats();
}

void Dashboard::updateSystemStats()
{
    // CPU
    double cpu = getCpuUsage();
    cpuLabel->setText(QString("CPU Usage: %1%").arg(cpu, 0, 'f', 1));
    cpuBar->setValue(static_cast<int>(cpu));

    // RAM
    int ramUsed = 0, ramTotal = 0;
    bool ramOk = getRamUsageMB(ramUsed, ramTotal);
    if (!ramOk || ramUsed <= 0 || ramTotal <= 0) {
        ramLabel->setText("RAM UPGGCU>Error");
        ramBar->setValue(0);
    } else {
        ramLabel->setText(QString("RAM Usage: %1 MB / %2 MB").arg(ramUsed).arg(ramTotal));
        ramBar->setValue(static_cast<int>(100.0 * ramUsed / ramTotal));
    }

    // Battery
    int battery = getBatteryPercent();
    if (battery >= 0) {
        batteryLabel->setText(QString("Battery: %1%").arg(battery));
        batteryBar->setValue(battery);
    } else {
        batteryLabel->setText("Battery: N/A");
        batteryBar->setValue(0);
    }

    // Uptime
    uptimeLabel->setText("Uptime: " + getUptime());

    // Disk
    QString diskDetails;
    double diskPercent = getDiskUsagePercent(&diskDetails);
    diskLabel->setText("Disk: " + diskDetails);
    diskBar->setValue(static_cast<int>(diskPercent));

    // Network
    quint64 rx = 0, tx = 0;
    getNetworkStats(rx, tx);
    static quint64 lastRxVal = rx, lastTxVal = tx;
    static QDateTime lastUpdate = QDateTime::currentDateTime();
    double rxRate = 0, txRate = 0;
    QDateTime now = QDateTime::currentDateTime();
    double elapsed = lastUpdate.msecsTo(now) / 1000.0;
    if (elapsed > 0) {
        rxRate = (rx - lastRxVal) / elapsed;
        txRate = (tx - lastTxVal) / elapsed;
    }
    lastRxVal = rx;
    lastTxVal = tx;
    lastUpdate = now;

    QString rxUnit = "B/s", txUnit = "B/s";
    if (rxRate > 1024) { rxRate /= 1024; rxUnit = "KB/s"; }
    if (rxRate > 1024) { rxRate /= 1024; rxUnit = "MB/s"; }
    if (txRate > 1024) { txRate /= 1024; txUnit = "KB/s"; }
    if (txRate > 1024) { txRate /= 1024; txUnit = "MB/s"; }

    netLabel->setText(QString("Network: ↓ %1 %2  ↑ %3 %4")
    .arg(rxRate, 0, 'f', 1).arg(rxUnit)
    .arg(txRate, 0, 'f', 1).arg(txUnit));
}

// --- Helpers ---

double Dashboard::getCpuUsage()
{
    QFile file("/proc/stat");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return 0.0;

    QTextStream in(&file);
    QString line = in.readLine();
    QStringList values = line.split(QRegExp("\\s+"), Qt::SkipEmptyParts);

    if (values.size() < 5)
        return 0.0;

    quint64 user = values[1].toULongLong();
    quint64 nice = values[2].toULongLong();
    quint64 system = values[3].toULongLong();
    quint64 idle = values[4].toULongLong();

    quint64 totalUser = user;
    quint64 totalUserLow = nice;
    quint64 totalSys = system;
    quint64 totalIdle = idle;

    quint64 total = (totalUser - lastTotalUser) + (totalUserLow - lastTotalUserLow) +
    (totalSys - lastTotalSys);
    quint64 totalAll = total + (totalIdle - lastTotalIdle);

    double percent = 0.0;
    if (totalAll > 0)
        percent = 100.0 * total / totalAll;

    lastTotalUser = totalUser;
    lastTotalUserLow = totalUserLow;
    lastTotalSys = totalSys;
    lastTotalIdle = totalIdle;

    return percent;
}

// Returns true if RAM values are valid, sets usedMB and totalMB
bool Dashboard::getRamUsageMB(int& usedMB, int& totalMB)
{
    QFile file("/proc/meminfo");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        usedMB = totalMB = 0;
        return false;
    }

    int memTotal = 0, memAvailable = -1, memFree = 0, buffers = 0, cached = 0;
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.startsWith("MemTotal:"))
            memTotal = line.split(QRegExp("\\s+"))[1].toInt();
        else if (line.startsWith("MemAvailable:"))
            memAvailable = line.split(QRegExp("\\s+"))[1].toInt();
        else if (line.startsWith("MemFree:"))
            memFree = line.split(QRegExp("\\s+"))[1].toInt();
        else if (line.startsWith("Buffers:"))
            buffers = line.split(QRegExp("\\s+"))[1].toInt();
        else if (line.startsWith("Cached:"))
            cached = line.split(QRegExp("\\s+"))[1].toInt();
    }
    totalMB = memTotal / 1024;
    int used = 0;
    if (memAvailable >= 0) {
        used = memTotal - memAvailable;
    } else {
        used = memTotal - memFree - buffers - cached;
    }
    usedMB = used / 1024;
    if (usedMB <= 0 || totalMB <= 0)
        return false;
    return true;
}

int Dashboard::getBatteryPercent()
{
    QProcess proc;
    proc.start("upower -i $(upower -e | grep BAT)", QIODevice::ReadOnly);
    proc.waitForFinished(500);
    QString output = proc.readAllStandardOutput();
    QRegExp rx("percentage:\\s*(\\d+)%");
    if (rx.indexIn(output) != -1) {
        return rx.cap(1).toInt();
    }

    QDir dir("/sys/class/power_supply");
    QStringList entries = dir.entryList(QStringList() << "BAT*", QDir::Dirs);
    foreach (const QString& entry, entries) {
        QFile capFile(dir.absoluteFilePath(entry + "/capacity"));
        if (capFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString val = capFile.readLine().trimmed();
            bool ok = false;
            int percent = val.toInt(&ok);
            if (ok) return percent;
        }
    }
    return -1;
}

QString Dashboard::getUptime()
{
    QFile file("/proc/uptime");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return "--";

    QString line = file.readLine();
    double seconds = line.split(" ").first().toDouble();
    int days = seconds / (60*60*24);
    int hours = ((int)seconds % (60*60*24)) / (60*60);
    int mins = ((int)seconds % (60*60)) / 60;
    int secs = ((int)seconds) % 60;

    QString up;
    if (days > 0) up += QString("%1d ").arg(days);
    if (hours > 0) up += QString("%1h ").arg(hours);
    if (mins > 0) up += QString("%1m ").arg(mins);
    up += QString("%1s").arg(secs);
    return up.trimmed();
}

double Dashboard::getDiskUsagePercent(QString* details)
{
    QStorageInfo storage("/");
    if (!storage.isValid() || !storage.isReady())
        return 0.0;
    qint64 total = storage.bytesTotal();
    qint64 free = storage.bytesAvailable();
    qint64 used = total - free;
    double percent = (total > 0) ? (100.0 * used / total) : 0.0;
    if (details) {
        QString usedStr = QString::number(used / (1024*1024)) + " MB";
        QString totalStr = QString::number(total / (1024*1024)) + " MB";
        *details = QString("%1 / %2 used (%3%)").arg(usedStr).arg(totalStr).arg(percent, 0, 'f', 1);
    }
    return percent;
}

void Dashboard::getNetworkStats(quint64& rx, quint64& tx)
{
    QFile file("/proc/net/dev");
    rx = 0;
    tx = 0;
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.contains(":")) {
            QStringList parts = line.split(":");
            if (parts.size() != 2) continue;
            QString iface = parts[0].trimmed();
            if (iface == "lo") continue; // skip loopback
            QStringList vals = parts[1].split(QRegExp("\\s+"), Qt::SkipEmptyParts);
            if (vals.size() < 9) continue;
            rx += vals[0].toULongLong();
            tx += vals[8].toULongLong();
        }
    }
}
