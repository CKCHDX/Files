#include "UIEffects.h"

UIEffects::UIEffects(QObject* parent) : QObject(parent) {}

void UIEffects::setAnimationsEnabled(bool enabled) {
    animationsEnabled = enabled;
}

void UIEffects::setAnimationDuration(int duration) {
    animationDuration = duration;
}

void UIEffects::fadeIn(QWidget* widget, int duration) {
    if (!animationsEnabled) {
        widget->show();
        return;
    }
    int actualDuration = (duration > 0) ? duration : animationDuration;
    QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(widget);
    widget->setGraphicsEffect(effect);
    QPropertyAnimation* animation = new QPropertyAnimation(effect, "opacity");
    animation->setDuration(actualDuration);
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void UIEffects::fadeOut(QWidget* widget, int duration) {
    if (!animationsEnabled) {
        widget->hide();
        return;
    }
    int actualDuration = (duration > 0) ? duration : animationDuration;
    QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(widget);
    widget->setGraphicsEffect(effect);
    QPropertyAnimation* animation = new QPropertyAnimation(effect, "opacity");
    animation->setDuration(actualDuration);
    animation->setStartValue(1.0);
    animation->setEndValue(0.0);
    connect(animation, &QPropertyAnimation::finished, widget, &QWidget::hide);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}
