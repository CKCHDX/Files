#ifndef WELCOME_PAGE_H
#define WELCOME_PAGE_H

#include <QWidget>

class QPushButton;

class WelcomePage : public QWidget {
    Q_OBJECT
public:
    explicit WelcomePage(QWidget *parent = nullptr);

signals:
    void startClicked();

private:
    QPushButton *startButton;
};

#endif // WELCOME_PAGE_H
