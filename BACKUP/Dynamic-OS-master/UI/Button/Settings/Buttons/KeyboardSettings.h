#ifndef KEYBOARDSETTINGS_H
#define KEYBOARDSETTINGS_H

#include <QWidget>
#include <QSlider>
#include <QFormLayout>

class KeyboardSettings : public QWidget
{
    Q_OBJECT

public:
    explicit KeyboardSettings(QWidget *parent = nullptr);

private slots:
    void onRepeatRateChanged(int value);
    void onDelayChanged(int value);

private:
    QSlider *keyboardRepeatSlider;
    QSlider *keyboardDelaySlider;

    void applyKeyboardSettings();
};

#endif // KEYBOARDSETTINGS_H
