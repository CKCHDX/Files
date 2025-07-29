#ifndef MOUSESETTINGS_H
#define MOUSESETTINGS_H

#include <QWidget>
#include <QSlider>
#include <QCheckBox>

class MouseSettings : public QWidget
{
    Q_OBJECT

public:
    explicit MouseSettings(QWidget *parent = nullptr);

private slots:
    void onSensitivityChanged(int value);
    void onAccelerationToggled(bool enabled);

private:
    void detectTouchpadDevice();
    void applyMouseSettings();

    QString touchpadDeviceId;

    QSlider *mouseSensitivitySlider;
    QCheckBox *mouseAccelerationCheckbox;
};

#endif // MOUSESETTINGS_H
