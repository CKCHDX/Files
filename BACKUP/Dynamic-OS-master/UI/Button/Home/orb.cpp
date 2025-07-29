#include "Orb.h"
#include <QPainter>
#include <QGridLayout>
#include <QFont>
#include <QMouseEvent>
#include <QPushButton>

static const int ORB_SIZE = 104;
static const int OPEN_SIZE = 335;

Orb::Orb(QWidget* parent)
    : QWidget(parent), m_state(Closed), m_progress(0.0), m_anim(new QPropertyAnimation(this, "morphProgress"))
{
    setFixedSize(OPEN_SIZE, OPEN_SIZE);
    setAttribute(Qt::WA_TranslucentBackground, true);

    m_panel = new QWidget(this);
    m_panelOpacity = new QGraphicsOpacityEffect(m_panel);
    m_panel->setGraphicsEffect(m_panelOpacity);
    m_panel->hide();

    setupPanel();

    m_anim->setDuration(420);
    m_anim->setEasingCurve(QEasingCurve::InOutCubic);

    connect(m_anim, &QPropertyAnimation::valueChanged, this, [this]() {
        updatePanel();
        update();
    });

    connect(m_anim, &QPropertyAnimation::finished, this, [this]() {
        if (m_state == Opening) {
            m_state = Open;
        } else if (m_state == Closing) {
            m_state = Closed;
            m_panel->hide();
        }
    });

    updatePanel();
}



void Orb::focusOutEvent(QFocusEvent* event)
{
    QWidget::focusOutEvent(event);
    this->hide(); // or emit a signal if you need
}

void Orb::setupPanel()
{
    QGridLayout* layout = new QGridLayout();
    layout->setSpacing(16);
    layout->setContentsMargins(32, 32, 32, 32);

    struct { const char* icon; const char* name; } tiles[4] = {
        { "💻", "Terminal" },
        { "🌐", "Web Browser" },
        { "🎬", "Media" },
        { "🛠️", "Dev Suite" }
    };

    for (int i = 0; i < 4; ++i) {
        QPushButton* btn = new QPushButton(tiles[i].icon, m_panel);
        btn->setFixedSize(68, 68);
        btn->setFont(QFont("Segoe UI Emoji", 32));
        btn->setCursor(Qt::PointingHandCursor);
        btn->setToolTip(tiles[i].name);
        btn->setStyleSheet(R"(
            QPushButton {
                background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                  stop:0 rgba(44,189,220,0.16), stop:1 rgba(12,18,26,0.88));
                border-radius: 15px;
                color: white;
                font-size: 32px;
                border: 1.2px solid #33c5e0;
                box-shadow: 0 4px 14px rgba(0,240,255,0.10);
            }
            QPushButton:hover {
                background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                  stop:0 rgba(32,218,255,0.25), stop:1 rgba(7,31,39,0.98));
                border-color: #33e1ee;
            }
        )");
        connect(btn, &QPushButton::clicked, this, [this, name = QString::fromUtf8(tiles[i].name)]() {
            emit appRequested(name);
            closeHub();
        });
        layout->addWidget(btn, i/2, i%2, Qt::AlignCenter);
    }
    m_panel->setLayout(layout);
}

QRect Orb::closedRect() const
{
    return QRect((width() - ORB_SIZE)/2, (height() - ORB_SIZE)/2, ORB_SIZE, ORB_SIZE);
}

QRect Orb::openRect() const
{
    return QRect(0, 0, OPEN_SIZE, OPEN_SIZE);
}

bool Orb::isClickOnCenter(const QPoint& pos) const
{
    QPoint c = rect().center();
    return QLineF(pos, c).length() <= ORB_SIZE/2;
}

void Orb::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QRectF rect = closedRect();
    QRectF orRect = openRect();
    qreal prog = m_progress;
    rect.setLeft(rect.left() + (orRect.left() - rect.left()) * prog);
    rect.setTop(rect.top() + (orRect.top() - rect.top()) * prog);
    rect.setWidth(rect.width() + (orRect.width() - rect.width()) * prog);
    rect.setHeight(rect.height() + (orRect.height() - rect.height()) * prog);
    float radius = (1.0-prog) * (ORB_SIZE/2.0) + prog * 27;

    // Modern cyan/teal glow
    QRadialGradient grad(rect.center(), rect.width()/1.7, rect.center());
    grad.setColorAt(0, QColor(57,255,230,210));
    grad.setColorAt(0.48, QColor(0,207,255,144));
    grad.setColorAt(0.81, QColor(15,34,49,170));
    grad.setColorAt(1, QColor(3,14,26,230));

    // Shadow ring
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(0,221,255,28));
    p.drawEllipse(rect.center(), rect.width()*0.52, rect.height()*0.51);

    // Orb main
    p.setBrush(grad);
    p.setPen(QPen(QColor(64,224,208,210), prog > 0.5 ? 6 : 3.2));
    p.drawRoundedRect(rect, radius, radius);

    // ⬣ icon with light glow effect
    p.save();
    p.translate(rect.center());
    p.rotate(prog*90);
    // Outer glow
    if (prog > 0.33)
    {
        QFont glowFont = p.font();
        glowFont.setPointSizeF((1.0-prog)*43.5 + prog*35);
        p.setFont(glowFont);
        p.setPen(QPen(QColor(0,255,250,90), 5.5));
        p.drawText(QRectF(-rect.width()/2, -rect.height()/2, rect.width(), rect.height()),
                   Qt::AlignCenter, "⬣");
    }
    // Main icon
    QFont font = p.font();
    font.setPointSizeF((1.0-prog)*43.5 + prog*36.5);
    font.setStyleStrategy(QFont::ForceOutline);
    p.setFont(font);
    p.setPen(Qt::white);
    p.drawText(QRectF(-rect.width()/2, -rect.height()/2, rect.width(), rect.height()),
               Qt::AlignCenter, "⬣");
    p.restore();
}

void Orb::resizeEvent(QResizeEvent*)
{
    updatePanel();
}

void Orb::mousePressEvent(QMouseEvent* e)
{
    if (m_state == Closed && closedRect().contains(e->pos())) {
        openHub();
    } else if (m_state == Open && isClickOnCenter(e->pos())) {
        closeHub();
    }
    QWidget::mousePressEvent(e);
}

void Orb::openHub()
{
    if (m_state != Closed) return;
    m_state = Opening;
    m_panel->show();
    m_panel->raise();
    m_panel->setGeometry(closedRect());
    m_anim->stop();
    m_anim->setStartValue(0.0);
    m_anim->setEndValue(1.0);
    m_anim->start();
}

void Orb::closeHub()
{
    if (m_state != Open) return;
    m_state = Closing;
    m_anim->stop();
    m_anim->setStartValue(1.0);
    m_anim->setEndValue(0.0);
    m_anim->start();
}

void Orb::setMorphProgress(qreal v)
{
    if (qFuzzyCompare(m_progress, v)) return;
    m_progress = v;
    updatePanel();
    update();
}

void Orb::updatePanel()
{
    QRect newGeom(
        closedRect().left() + (openRect().left() - closedRect().left()) * m_progress,
        closedRect().top() + (openRect().top() - closedRect().top()) * m_progress,
        closedRect().width() + (openRect().width() - closedRect().width()) * m_progress,
        closedRect().height() + (openRect().height() - closedRect().height()) * m_progress
    );
    m_panel->setGeometry(newGeom);
    m_panelOpacity->setOpacity(m_progress);
}

void Orb::reset()
{
    m_anim->stop();
    m_progress = 0.0;
    m_state = Closed;
    if (m_panel) m_panel->hide();
    updatePanel();
    update();
}
