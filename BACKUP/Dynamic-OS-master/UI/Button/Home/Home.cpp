#include "Home.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QHBoxLayout>
#include <QSpacerItem>

Home::Home(QWidget* parent)
    : QWidget(parent)
{
    // Theme color for labels
    const QString accent = "#25f6fa";

    QVBoxLayout* layout = new QVBoxLayout(this);

    // Top: Welcome Title
    QLabel* title = new QLabel("Welcome to Dynamic OS", this);
    QFont large = title->font(); large.setPointSize(27); large.setWeight(QFont::Bold);
    title->setFont(large);
    title->setAlignment(Qt::AlignHCenter);
    title->setStyleSheet("color: " + accent + "; letter-spacing:0.05em;");

    // Center: Orb (slightly below center)
    orb = new Orb(this);

    // Below orb: User snapshot info (stylized)
    QLabel* info = new QLabel(
        "🖥️ <b>Session:</b> User1<br>"
        "🔋 <b>Battery:</b> 87%<br>"
        "💾 <b>RAM Used:</b> 5.2 GB<br>"
        "📡 <b>WiFi:</b> Connected<br>", this);
    info->setTextFormat(Qt::RichText);
    info->setAlignment(Qt::AlignCenter);
    info->setStyleSheet("font-size: 16px; color: #c3fafe;");

    // Spacer for breathing space above and below orb
    layout->addSpacing(20);
    layout->addWidget(title, 0, Qt::AlignTop | Qt::AlignHCenter);
    layout->addSpacing(16);
    layout->addWidget(orb, 0, Qt::AlignHCenter);
    layout->addSpacing(10);
    layout->addWidget(info, 0, Qt::AlignHCenter);
    layout->addStretch(1);

    // (Optional) Notices Panel for user
    QLabel* notice = new QLabel("Tip: Click the orb ⬣ to access quick launch for Terminal, Web, Media, Dev Suite.", this);
    notice->setStyleSheet("font-size: 14px; color:#b2f7ff; background:rgba(7,30,44,0.36); border-radius:7px; padding:6px 12px;");
    notice->setAlignment(Qt::AlignCenter);
    layout->addWidget(notice, 0, Qt::AlignBottom|Qt::AlignHCenter);

    setLayout(layout);

    connect(orb, &Orb::appRequested, this, &Home::appRequested);
}
