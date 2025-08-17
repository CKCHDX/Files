#ifndef USER_PAGE_H
#define USER_PAGE_H

#include <QWidget>

class QLineEdit;
class QPushButton;
class QLabel;

class UserPage : public QWidget {
    Q_OBJECT
public:
    explicit UserPage(QWidget *parent = nullptr);

    QString username() const;
    QString password() const;
    QString hostname() const;

signals:
    void continueClicked(const QString &username, const QString &password, const QString &hostname);

private slots:
    void validateInput();

private:
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QLineEdit *passwordConfirmEdit;
    QLineEdit *hostnameEdit;
    QLabel *errorLabel;
    QPushButton *continueButton;
};

#endif // USER_PAGE_H
