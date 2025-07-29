#pragma once
#include <QWidget>

class OrbSidebarButton : public QWidget
{
    Q_OBJECT
public:
    explicit OrbSidebarButton(QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent* e) override;

signals:
    void clicked();
};
