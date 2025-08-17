#include "finish_page.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFont>

FinishPage::FinishPage(bool success, QWidget *parent, const QString &errorMsg)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);

    auto *title = new QLabel(success ? "Installation Complete!" : "Installation Failed", this);
    QFont titleFont = title->font();
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    title->setFont(titleFont);
    title->setAlignment(Qt::AlignCenter);

    messageLabel = new QLabel(this);
    messageLabel->setAlignment(Qt::AlignCenter);
    messageLabel->setWordWrap(true);
    messageLabel->setStyleSheet("color: #c0c6d0; font-size: 15px;");

    if (success) {
        messageLabel->setText(
            "Oscyra OS has been successfully installed.<br>"
            "You can now reboot your computer and remove the installation media.<br><br>"
            "Thank you for choosing Oscyra!"
        );
    } else {
        messageLabel->setText(
            "An error occurred during installation:<br><b>" + errorMsg.toHtmlEscaped() + "</b><br>"
            "Please check your hardware and try again."
        );
    }

    rebootButton = new QPushButton("Reboot Now", this);
    rebootButton->setFixedWidth(180);
    rebootButton->setMinimumHeight(36);

    exitButton = new QPushButton(success ? "Exit Installer" : "Close", this);
    exitButton->setFixedWidth(180);
    exitButton->setMinimumHeight(36);

    layout->addStretch();
    layout->addWidget(title);
    layout->addSpacing(20);
    layout->addWidget(messageLabel);
    layout->addSpacing(18);
    layout->addWidget(rebootButton, 0, Qt::AlignCenter);
    layout->addWidget(exitButton, 0, Qt::AlignCenter);
    layout->addStretch();

    title->setStyleSheet("color: #e0e6f0;");
    rebootButton->setStyleSheet(
        "background-color: #3aaf5a; color: #fff; border-radius: 8px; font-size: 16px; padding: 8px 18px;"
        "border: 1px solid #3a3f5a;"
    );
    exitButton->setStyleSheet(
        "background-color: #29304a; color: #fff; border-radius: 8px; font-size: 16px; padding: 8px 18px;"
        "border: 1px solid #3a3f5a;"
    );

    connect(rebootButton, &QPushButton::clicked, this, &FinishPage::rebootRequested);
    connect(exitButton, &QPushButton::clicked, this, &FinishPage::exitRequested);
}
