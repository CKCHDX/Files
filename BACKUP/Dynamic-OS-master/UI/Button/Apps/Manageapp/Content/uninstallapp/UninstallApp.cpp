#include "UninstallApp.h"

UninstallApp::UninstallApp(QWidget* parent) : QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    QLabel* label = new QLabel("Uninstall App Page", this);
    label->setStyleSheet("color: white; font-size: 18px;");
    layout->addWidget(label);
    setLayout(layout);
}
