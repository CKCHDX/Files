#ifndef BACKGROUND_WIDGET_H
#define BACKGROUND_WIDGET_H

#include <QWidget>
#include <QTimer>
#include <QVector>
#include <QPointF>
#include <QColor>

struct LightBlob {
    QPointF position;
    float radius;
    float speedX;
    float speedY;
    float phase;
    QColor color;
};

class BackgroundWidget : public QWidget {
    Q_OBJECT
public:
    explicit BackgroundWidget(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void animate();

private:
    QTimer timer;
    QVector<LightBlob> blobs;
    float time;

    void setupBlobs();
};

#endif // BACKGROUND_WIDGET_H
