#include "Apps.h"
#include <QLabel>
#include <QVBoxLayout>

AppsFunction::AppsFunction(QWidget* parent)
    : QWidget(parent)
{
    // Minimal or "dummy" content
    QVBoxLayout* layout = new QVBoxLayout(this);
    QLabel* label = new QLabel("Apps Function Page (Placeholder)", this);
    label->setStyleSheet("color: white; font-size: 14px;");
    layout->addWidget(label);
    setLayout(layout);
}
