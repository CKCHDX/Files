#ifndef STORAGE_H
#define STORAGE_H

#include <QWidget>
#include <QTimer>
#include <QVector>
#include <QString>
#include <QColor>
#include <QFutureWatcher>

struct DiskSlice {
    QString name;
    QString path; // full path to folder or mount point
    quint64 size; // bytes
    QColor color;
    QVector<DiskSlice> children;
};

class DiskUsageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DiskUsageWidget(QWidget *parent = nullptr);

public slots:
    void refreshDiskInfo();
    void goBack();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QVector<DiskSlice> currentSlices;           // current visible slices (partitions or folders)
    QVector<QVector<DiskSlice>> history;        // history stack for navigation

    float animationProgress;
    QTimer *animationTimer;

    int hoveredSliceIndex;

    QRectF pieRect() const;
    QPointF center() const;
    qreal radiusOuter() const;
    qreal radiusInner() const;

    void updateAnimation();
    void drawSlices(QPainter &p, const QRectF &rect, const QVector<DiskSlice> &slices);
    int sliceAtPosition(const QPoint &pos, const QRectF &rect, const QVector<DiskSlice> &slices) const;

    void loadPartitions();

    QFutureWatcher<QVector<DiskSlice>> folderSizeWatcher;
    QVector<DiskSlice> calculateFolderSizes(const QString &path);
    void onFolderSizeCalculationFinished();

    QColor generateColor(int index) const;
};

#endif // STORAGE_H
