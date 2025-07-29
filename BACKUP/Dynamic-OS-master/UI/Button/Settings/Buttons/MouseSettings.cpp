#include "MouseSettings.h"
#include <QFormLayout>
#include <QProcess>
#include <QRegularExpression>
#include <QDebug>

MouseSettings::MouseSettings(QWidget *parent)
: QWidget(parent)
{
    QFormLayout *layout = new QFormLayout(this);

    mouseSensitivitySlider = new QSlider(Qt::Horizontal, this);
    mouseSensitivitySlider->setRange(1, 20);
    mouseSensitivitySlider->setValue(10);

    mouseAccelerationCheckbox = new QCheckBox("Enable Acceleration", this);
    mouseAccelerationCheckbox->setChecked(true);

    layout->addRow("Pointer Speed:", mouseSensitivitySlider);
    layout->addRow("", mouseAccelerationCheckbox);

    connect(mouseSensitivitySlider, &QSlider::valueChanged,
            this, &MouseSettings::onSensitivityChanged);
    connect(mouseAccelerationCheckbox, &QCheckBox::toggled,
            this, &MouseSettings::onAccelerationToggled);

    detectTouchpadDevice();
    applyMouseSettings();
}

void MouseSettings::detectTouchpadDevice()
{
    QProcess proc;
    proc.start("xinput", {"list"});
    proc.waitForFinished();
    QString output = proc.readAllStandardOutput();

    QRegularExpression re("Touchpad.*id=(\\d+)");
    QRegularExpressionMatch match = re.match(output);

    if (match.hasMatch()) {
        touchpadDeviceId = match.captured(1);
        qDebug() << "Detected Touchpad Device ID:" << touchpadDeviceId;
    } else {
        touchpadDeviceId.clear();
        qDebug() << "Touchpad Device not found!";
    }
}

void MouseSettings::applyMouseSettings()
{
    if (touchpadDeviceId.isEmpty()) {
        qWarning() << "No touchpad device detected - cannot apply settings";
        return;
    }

    double speed = (mouseSensitivitySlider->value() - 10) / 10.0;

    QString cmdSpeed = QString("xinput --set-prop %1 \"libinput Accel Speed\" %2")
    .arg(touchpadDeviceId).arg(speed);
    QProcess::execute(cmdSpeed);

    QString profileValue = mouseAccelerationCheckbox->isChecked() ? "1 0" : "0 1";
    QString cmdProfile = QString("xinput --set-prop %1 \"libinput Accel Profile Enabled\" %2")
    .arg(touchpadDeviceId).arg(profileValue);
    QProcess::execute(cmdProfile);

    qDebug() << "Applied mouse settings: speed =" << speed
    << ", acceleration enabled =" << mouseAccelerationCheckbox->isChecked();
}

void MouseSettings::onSensitivityChanged(int /*value*/)
{
    applyMouseSettings();
}

void MouseSettings::onAccelerationToggled(bool /*enabled*/)
{
    applyMouseSettings();
}
