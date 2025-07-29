#ifndef HOME_H
#define HOME_H

#include <QWidget>
#include "Orb.h"

class Home : public QWidget
{
    Q_OBJECT
public:
    explicit Home(QWidget* parent = nullptr);
    void reset() { orb->reset(); }

signals:
    void appRequested(const QString& app);

private:
    Orb* orb;
};

#endif // HOME_H
