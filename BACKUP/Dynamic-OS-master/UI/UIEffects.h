#ifndef UIEFFECTS_H
#define UIEFFECTS_H

#include <QObject>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QWidget>

class UIEffects : public QObject
{
    Q_OBJECT
public:
    explicit UIEffects(QObject* parent = nullptr);

    void fadeIn(QWidget* widget, int duration = 500);
    void fadeOut(QWidget* widget, int duration = 500);

    // Add these:
    void setAnimationsEnabled(bool enabled);
    void setAnimationDuration(int duration);

private:
    bool animationsEnabled = true;
    int animationDuration = 500;
};

#endif // UIEFFECTS_H
