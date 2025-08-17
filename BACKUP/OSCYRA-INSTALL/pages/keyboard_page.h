#ifndef KEYBOARD_PAGE_H
#define KEYBOARD_PAGE_H

#include <QWidget>

class QComboBox;
class QPushButton;

class KeyboardPage : public QWidget {
    Q_OBJECT
public:
    explicit KeyboardPage(QWidget *parent = nullptr);

    QString selectedLayout() const;

signals:
    void continueClicked(const QString &layout);

private:
    QComboBox *layoutComboBox;
    QPushButton *continueButton;
};

#endif // KEYBOARD_PAGE_H
