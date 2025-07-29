#include "ManageAppPage.h"

ManageAppPage::ManageAppPage(QWidget* parent) : QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    QLabel* label = new QLabel("Manage Installed Apps", this);
    label->setStyleSheet("color: white; font-size: 18px;");
    layout->addWidget(label);
    setLayout(layout);
}
