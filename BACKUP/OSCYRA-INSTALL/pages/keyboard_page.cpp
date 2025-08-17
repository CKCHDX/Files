#include "keyboard_page.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QFont>

KeyboardPage::KeyboardPage(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);

    auto *title = new QLabel("Keyboard Layout", this);
    QFont titleFont = title->font();
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    title->setFont(titleFont);
    title->setAlignment(Qt::AlignCenter);

    auto *desc = new QLabel("Choose your preferred keyboard layout for the installed system:", this);
    desc->setAlignment(Qt::AlignCenter);

    layoutComboBox = new QComboBox(this);
    layoutComboBox->addItem("English (US)", "us");
    layoutComboBox->addItem("German (DE)", "de");
    layoutComboBox->addItem("Swedish (SE)", "se");
    layoutComboBox->addItem("French (FR)", "fr");
    layoutComboBox->addItem("Spanish (ES)", "es");
    layoutComboBox->setCurrentIndex(0);
    layoutComboBox->setFixedWidth(220);

    continueButton = new QPushButton("Continue", this);
    continueButton->setFixedWidth(180);
    continueButton->setMinimumHeight(36);

    layout->addStretch();
    layout->addWidget(title);
    layout->addSpacing(10);
    layout->addWidget(desc);
    layout->addSpacing(10);
    layout->addWidget(layoutComboBox, 0, Qt::AlignCenter);
    layout->addSpacing(18);
    layout->addWidget(continueButton, 0, Qt::AlignCenter);
    layout->addStretch();

    title->setStyleSheet("color: #e0e6f0;");
    desc->setStyleSheet("color: #c0c6d0;");
    continueButton->setStyleSheet(
        "background-color: #29304a; color: #fff; border-radius: 8px; font-size: 16px; padding: 8px 18px;"
        "border: 1px solid #3a3f5a;"
    );

    connect(continueButton, &QPushButton::clicked, [this]() {
        emit continueClicked(selectedLayout());
    });
}

QString KeyboardPage::selectedLayout() const {
    return layoutComboBox->currentData().toString();
}
