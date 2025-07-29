#ifndef FUNCTIONCONTROL_H
#define FUNCTIONCONTROL_H

#include <QObject>

class FunctionControl : public QObject
{
    Q_OBJECT
public:
    explicit FunctionControl(QObject* parent = nullptr);
    ~FunctionControl();

    void handleHome();
    void handleApps();
    void handleSettings();
};

#endif // FUNCTIONCONTROL_H
