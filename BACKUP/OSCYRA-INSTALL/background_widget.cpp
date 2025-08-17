#include "background_widget.h"
#include <QPainter>
#include <QLinearGradient>
#include <QRadialGradient>
#include <QRandomGenerator>
#include <QtMath>

BackgroundWidget::BackgroundWidget(QWidget *parent)
    : QWidget(parent), time(0.0f)
{
    // Make widget background transparent
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_NoSystemBackground);
    setAutoFillBackground(false);

    setupBlobs();

    connect(&timer, &QTimer::timeout, this, &BackgroundWidget::animate);
    timer.start(33); // ~30 FPS
}

static float randomFloat(float min, float max) {
    return min + (max - min) * QRandomGenerator::global()->generateDouble();
}

void BackgroundWidget::setupBlobs() {
    blobs.clear();
    int count = 6; // Number of blobs

    for (int i = 0; i < count; ++i) {
        LightBlob blob;
        blob.position = QPointF(randomFloat(0, width()), randomFloat(0, height()));
        blob.radius = randomFloat(width() * 0.12f, width() * 0.25f);
        blob.speedX = randomFloat(-0.3f, 0.3f);
        blob.speedY = randomFloat(-0.2f, 0.2f);
        blob.phase = randomFloat(0, 2 * M_PI);
        // Muted blue-gray colors with alpha for subtlety
        blob.color = QColor::fromRgbF(0.15f, 0.18f, 0.25f, 0.12f + 0.08f * randomFloat(0,1));
        blobs.append(blob);
    }
}

void BackgroundWidget::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 1. Base background: smooth dark gradient
    QLinearGradient bgGradient(rect().topLeft(), rect().bottomRight());
    bgGradient.setColorAt(0.0, QColor(18, 20, 24));
    bgGradient.setColorAt(1.0, QColor(32, 34, 40));
    painter.fillRect(rect(), bgGradient);

    // 2. Animate and draw soft light blobs
    for (LightBlob &blob : blobs) {
        // Update position with wrapping
        blob.position.rx() += blob.speedX;
        blob.position.ry() += blob.speedY;

        if (blob.position.x() < -blob.radius) blob.position.setX(width() + blob.radius);
        if (blob.position.x() > width() + blob.radius) blob.position.setX(-blob.radius);
        if (blob.position.y() < -blob.radius) blob.position.setY(height() + blob.radius);
        if (blob.position.y() > height() + blob.radius) blob.position.setY(-blob.radius);

        // Pulsate radius subtly
        float pulsate = 1.0f + 0.15f * std::sin(time + blob.phase);

        QRadialGradient gradient(blob.position, blob.radius * pulsate, blob.position);
        QColor c = blob.color;
        c.setAlphaF(c.alphaF() * (0.6f + 0.4f * std::sin(time * 1.3f + blob.phase)));
        gradient.setColorAt(0.0, c);
        gradient.setColorAt(1.0, Qt::transparent);

        painter.setBrush(gradient);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(blob.position, blob.radius * pulsate, blob.radius * pulsate);
    }

    // 3. Optional: very subtle, slow-moving light sweep
    QRadialGradient lightSweep(rect().center() + QPointF(width() * 0.2f * std::cos(time * 0.1f),
                                                        height() * 0.15f * std::sin(time * 0.12f)),
                              width() * 0.8, rect().center());
    lightSweep.setColorAt(0.0, QColor(255, 255, 255, 18));
    lightSweep.setColorAt(1.0, Qt::transparent);
    painter.setBrush(lightSweep);
    painter.drawEllipse(rect().center(), width() * 0.8, width() * 0.8);
}

void BackgroundWidget::animate() {
    time += 0.033f;
    update();
}
