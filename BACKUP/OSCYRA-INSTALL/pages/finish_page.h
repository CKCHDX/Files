#ifndef FINISH_PAGE_H
#define FINISH_PAGE_H

#include <QWidget>

class QLabel;
class QPushButton;

class FinishPage : public QWidget {
    Q_OBJECT
public:
    explicit FinishPage(bool success, QWidget *parent = nullptr, const QString &errorMsg = QString());

signals:
    void rebootRequested();
    void exitRequested();

private:
    QLabel *messageLabel;
    QPushButton *rebootButton;
    QPushButton *exitButton;
};

#endif // FINISH_PAGE_H
