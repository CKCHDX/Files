#include "welcome_page.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFont>

WelcomePage::WelcomePage(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);

    auto *title = new QLabel("Oscyra Installer", this);
    QFont titleFont = title->font();
    titleFont.setPointSize(24);
    titleFont.setBold(true);
    title->setFont(titleFont);
    title->setAlignment(Qt::AlignCenter);

    auto *desc = new QLabel(
        "Welcome to Dynamic OS.\n"
        "This installer will guide you through the process of installing Oscyra on your system.\n",
        this
    );
    desc->setAlignment(Qt::AlignCenter);
    desc->setWordWrap(true);
    desc->setTextFormat(Qt::RichText);
    desc->setOpenExternalLinks(true);

    startButton = new QPushButton("Start Installation", this);
    startButton->setFixedWidth(220);
    startButton->setMinimumHeight(40);

    layout->addStretch();
    layout->addWidget(title);
    layout->addSpacing(18);
    layout->addWidget(desc);
    layout->addSpacing(24);
    layout->addWidget(startButton, 0, Qt::AlignCenter);
    layout->addStretch();

    title->setStyleSheet("color: #e0e6f0;");
    desc->setStyleSheet("color: #c0c6d0;");
    startButton->setStyleSheet(
        "background-color: #29304a; color: #fff; border-radius: 8px; font-size: 18px; padding: 10px 20px;"
        "border: 1px solid #3a3f5a;"
    );

    connect(startButton, &QPushButton::clicked, this, &WelcomePage::startClicked);
}
