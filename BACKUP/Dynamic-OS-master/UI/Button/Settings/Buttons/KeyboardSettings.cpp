#include "KeyboardSettings.h"
#include <QProcess>
#include <QDebug>

KeyboardSettings::KeyboardSettings(QWidget *parent)
: QWidget(parent)
{
    QFormLayout *layout = new QFormLayout(this);

    keyboardRepeatSlider = new QSlider(Qt::Horizontal, this);
    keyboardRepeatSlider->setRange(1, 50);
    keyboardRepeatSlider->setValue(25);

    keyboardDelaySlider = new QSlider(Qt::Horizontal, this);
    keyboardDelaySlider->setRange(100, 1000);
    keyboardDelaySlider->setSingleStep(50);
    keyboardDelaySlider->setValue(500);

    layout->addRow("Repeat Rate:", keyboardRepeatSlider);
    layout->addRow("Repeat Delay (ms):", keyboardDelaySlider);

    connect(keyboardRepeatSlider, &QSlider::valueChanged,
            this, &KeyboardSettings::onRepeatRateChanged);
    connect(keyboardDelaySlider, &QSlider::valueChanged,
            this, &KeyboardSettings::onDelayChanged);
}

void KeyboardSettings::onRepeatRateChanged(int /*value*/)
{
    applyKeyboardSettings();
}

void KeyboardSettings::onDelayChanged(int /*value*/)
{
    applyKeyboardSettings();
}

void KeyboardSettings::applyKeyboardSettings()
{
    int rate = keyboardRepeatSlider->value();
    int delay = keyboardDelaySlider->value();

    QString cmd = QString("xset r rate %1 %2").arg(delay).arg(rate);
    QProcess::execute(cmd);

    qDebug() << "Applied keyboard repeat rate and delay:" << delay << rate;
}
