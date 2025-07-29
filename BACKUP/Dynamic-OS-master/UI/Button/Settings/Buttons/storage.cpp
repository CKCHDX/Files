#include "storage.h"

#include <QPainter>
#include <QMouseEvent>
#include <QToolTip>
#include <QPainterPath>
#include <QtMath>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QtConcurrent>
#include <sys/statvfs.h>
#include <QDebug>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

DiskUsageWidget::DiskUsageWidget(QWidget *parent)
: QWidget(parent),
animationProgress(1.0f),
hoveredSliceIndex(-1)
{
    setMinimumSize(400, 400);
    setMouseTracking(true);

    connect(&folderSizeWatcher, &QFutureWatcher<QVector<DiskSlice>>::finished,
            this, &DiskUsageWidget::onFolderSizeCalculationFinished);

    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, &DiskUsageWidget::updateAnimation);

    loadPartitions();

    QTimer *refreshTimer = new QTimer(this);
    connect(refreshTimer, &QTimer::timeout, this, &DiskUsageWidget::refreshDiskInfo);
    refreshTimer->start(30000);
}

void DiskUsageWidget::refreshDiskInfo()
{
    if (folderSizeWatcher.isRunning())
        return; // Don't refresh while folder scan running

        loadPartitions();
}


void DiskUsageWidget::loadPartitions()
{
    QVector<DiskSlice> partitions;

    QFile mountsFile("/proc/mounts");
    if (!mountsFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open /proc/mounts";
        return;
    }

    QSet<QString> seenMounts;

    while (!mountsFile.atEnd()) {
        QByteArray line = mountsFile.readLine();
        QList<QByteArray> parts = line.split(' ');
        if (parts.size() < 2)
            continue;

        QString device = parts[0];
        QString mountPoint = parts[1];

        if (!(device.startsWith("/dev/sd") || device.startsWith("/dev/nvme")))
            continue;

        if (seenMounts.contains(mountPoint))
            continue; // avoid duplicates

            seenMounts.insert(mountPoint);

        struct statvfs stat;
        if (statvfs(mountPoint.toUtf8().constData(), &stat) != 0)
            continue;

        quint64 total = stat.f_blocks * stat.f_frsize;
        quint64 free = stat.f_bfree * stat.f_frsize;
        quint64 used = total > free ? total - free : 0;

        DiskSlice slice;
        slice.name = mountPoint;
        slice.path = mountPoint;
        slice.size = used;
        slice.color = generateColor(partitions.size());
        slice.children.clear(); // Will drill down on click

        partitions.append(slice);
    }
    mountsFile.close();

    history.clear();
    currentSlices = partitions;
    animationProgress = 0.0f;
    animationTimer->start(15);
    update();
}

QRectF DiskUsageWidget::pieRect() const
{
    int side = qMin(width(), height());
    return QRectF((width() - side) / 2 + 20, (height() - side) / 2 + 20, side - 40, side - 40);
}

QPointF DiskUsageWidget::center() const
{
    return pieRect().center();
}

qreal DiskUsageWidget::radiusOuter() const
{
    return pieRect().width() / 2;
}

qreal DiskUsageWidget::radiusInner() const
{
    return radiusOuter() * 0.5;
}

QColor DiskUsageWidget::generateColor(int index) const
{
    static QVector<QColor> baseColors = {
        QColor(255, 99, 132), QColor(54, 162, 235), QColor(255, 206, 86),
        QColor(75, 192, 192), QColor(153, 102, 255), QColor(255, 159, 64),
        QColor(100, 255, 150), QColor(255, 150, 100)
    };
    return baseColors[index % baseColors.size()];
}

QVector<DiskSlice> DiskUsageWidget::calculateFolderSizes(const QString &path)
{
    QVector<DiskSlice> folders;

    QDir dir(path);
    QFileInfoList entries = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);

    int idx = 0;

    std::function<quint64(const QDir &)> folderSizeCalc = [&](const QDir &d) -> quint64 {
        quint64 size = 0;
        QFileInfoList list = d.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
        for (const QFileInfo &info : list) {
            if (info.isDir()) {
                size += folderSizeCalc(QDir(info.absoluteFilePath()));
            } else {
                size += info.size();
            }
        }
        return size;
    };

    for (const QFileInfo &entry : entries) {
        quint64 size = folderSizeCalc(QDir(entry.absoluteFilePath()));

        DiskSlice slice;
        slice.name = entry.fileName();
        slice.path = entry.absoluteFilePath();
        slice.size = size;
        slice.color = generateColor(idx++);
        slice.children.clear();

        folders.append(slice);
    }

    return folders;
}

void DiskUsageWidget::onFolderSizeCalculationFinished()
{
    QVector<DiskSlice> result = folderSizeWatcher.result();
    if (!result.isEmpty()) {
        currentSlices = result;

        animationProgress = 0.0f;
        animationTimer->start(15);

        update();
    }
}

void DiskUsageWidget::drawSlices(QPainter &p, const QRectF &rect, const QVector<DiskSlice> &slices)
{
    if (slices.isEmpty())
        return;

    quint64 totalSize = 0;
    for (const auto &slice : slices)
        totalSize += slice.size;

    if (totalSize == 0)
        return;

    QPointF c = rect.center();
    qreal radiusOut = radiusOuter();
    qreal radiusIn = radiusInner();

    qreal startAngle = 90.0; // start from top

    for (int i = 0; i < slices.size(); ++i) {
        const DiskSlice &slice = slices[i];
        qreal spanAngle = 360.0 * double(slice.size) / totalSize;
        spanAngle *= animationProgress;

        QPainterPath path;

        // Outer arc points
        path.moveTo(c);
        for (qreal angle = startAngle; angle <= startAngle + spanAngle; angle += 1.0) {
            qreal rad = qDegreesToRadians(angle);
            QPointF pt(c.x() + radiusOut * std::cos(rad),
                       c.y() - radiusOut * std::sin(rad));
            path.lineTo(pt);
        }

        // Inner arc points (backwards)
        for (qreal angle = startAngle + spanAngle; angle >= startAngle; angle -= 1.0) {
            qreal rad = qDegreesToRadians(angle);
            QPointF pt(c.x() + radiusIn * std::cos(rad),
                       c.y() - radiusIn * std::sin(rad));
            path.lineTo(pt);
        }
        path.closeSubpath();

        p.setBrush(slice.color);
        p.setPen(QPen(Qt::black, 2));
        p.drawPath(path);

        // Draw label
        qreal radMid = qDegreesToRadians(startAngle + spanAngle / 2);
        QPointF textPos(c.x() + (radiusIn + (radiusOut - radiusIn)/2) * std::cos(radMid),
                        c.y() - (radiusIn + (radiusOut - radiusIn)/2) * std::sin(radMid));

        QString label = QString("%1\n%2 MB").arg(slice.name).arg(slice.size / (1024 * 1024));

        QFont f = p.font();
        f.setPointSize(8);
        p.setFont(f);
        p.setPen(Qt::black);

        QRectF textRect(textPos.x() - 40, textPos.y() - 20, 80, 40);
        p.drawText(textRect, Qt::AlignCenter | Qt::TextWordWrap, label);

        startAngle += spanAngle;
    }
}

int DiskUsageWidget::sliceAtPosition(const QPoint &pos, const QRectF &rect, const QVector<DiskSlice> &slices) const
{
    if (slices.isEmpty())
        return -1;

    QPointF c = rect.center();
    qreal radiusOut = radiusOuter();
    qreal radiusIn = radiusInner();

    qreal dx = pos.x() - c.x();
    qreal dy = c.y() - pos.y(); // invert Y for angle calculation

    qreal dist = std::sqrt(dx*dx + dy*dy);
    if (dist < radiusIn || dist > radiusOut)
        return -1;

    qreal angle = std::atan2(dy, dx);
    if (angle < 0)
        angle += 2 * M_PI;

    quint64 totalSize = 0;
    for (const auto &s : slices)
        totalSize += s.size;
    if (totalSize == 0)
        return -1;

    qreal angleDeg = qRadiansToDegrees(angle);
    qreal startAngle = 90.0;
    for (int i = 0; i < slices.size(); ++i) {
        qreal span = 360.0 * double(slices[i].size) / totalSize;
        qreal endAngle = startAngle + span;
        qreal modAngle = std::fmod(angleDeg - startAngle + 360.0, 360.0);
        if (modAngle >= 0 && modAngle < span) {
            return i;
        }
        startAngle += span;
    }
    return -1;
}

void DiskUsageWidget::mouseMoveEvent(QMouseEvent *event)
{
    QRectF rect = pieRect();
    int index = sliceAtPosition(event->pos(), rect, currentSlices);

    if (index != hoveredSliceIndex) {
        hoveredSliceIndex = index;
        if (index >= 0 && index < currentSlices.size()) {
            const DiskSlice &slice = currentSlices[index];
            QString tip = QString("%1\nSize: %2 MB").arg(slice.name).arg(slice.size / (1024 * 1024));
            QToolTip::showText(event->globalPos(), tip, this);
        } else {
            QToolTip::hideText();
        }
        update();
    }
}

void DiskUsageWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
        return;

    QRectF rect = pieRect();
    int index = sliceAtPosition(event->pos(), rect, currentSlices);
    if (index < 0 || index >= currentSlices.size())
        return;

    DiskSlice &clickedSlice = currentSlices[index];

    if (!clickedSlice.children.isEmpty()) {
        history.append(currentSlices);
        currentSlices = clickedSlice.children;
        animationProgress = 0.0f;
        animationTimer->start(15);
        update();
        return;
    }

    // Drill down into folders asynchronously
    history.append(currentSlices);
    animationProgress = 0.0f;
    animationTimer->start(15);

    folderSizeWatcher.setFuture(QtConcurrent::run(this, &DiskUsageWidget::calculateFolderSizes, clickedSlice.path));
}

void DiskUsageWidget::goBack()
{
    if (!history.isEmpty()) {
        currentSlices = history.takeLast();
        animationProgress = 0.0f;
        animationTimer->start(15);
        update();
    }
}

void DiskUsageWidget::updateAnimation()
{
    animationProgress += 0.05f;
    if (animationProgress >= 1.0f) {
        animationProgress = 1.0f;
        animationTimer->stop();
    }
    update();
}

void DiskUsageWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    drawSlices(p, pieRect(), currentSlices);
}
