#include "AddApp.h"

AddApp::AddApp(QWidget* parent) : QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    QLabel* label = new QLabel("Add App Page", this);
    label->setStyleSheet("color: white; font-size: 18px;");
    layout->addWidget(label);
    setLayout(layout);
}
