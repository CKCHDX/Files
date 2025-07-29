#include "OrbSidebarButton.h"
#include <QPainter>
#include <QMouseEvent>

OrbSidebarButton::OrbSidebarButton(QWidget* parent)
    : QWidget(parent)
{
    setFixedSize(46, 46);
    setCursor(Qt::PointingHandCursor);
    // Consider setToolTip("Open Dynamic Hub");
}

void OrbSidebarButton::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    // Orb with neon glow
    QRadialGradient grad(rect().center(), width()/2);
    grad.setColorAt(0, QColor(0,215,245,190));
    grad.setColorAt(0.7, QColor(30,70,85,170));
    grad.setColorAt(1, QColor(30,40,55,200));
    p.setBrush(grad);
    p.setPen(QPen(QColor(64,224,208,190), 2));
    p.drawEllipse(rect().adjusted(3,3,-3,-3));

    // The ⬣ icon
    p.setPen(Qt::white);
    QFont font = p.font(); font.setPointSize(21);
    p.setFont(font);
    p.drawText(rect(), Qt::AlignCenter, "⬣");
}

void OrbSidebarButton::mousePressEvent(QMouseEvent* e)
{
    if (e->button()==Qt::LeftButton)
        emit clicked();
}
