#include "user_page.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFont>

UserPage::UserPage(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);

    auto *title = new QLabel("Create User Account", this);
    QFont titleFont = title->font();
    titleFont.setPointSize(20);
    titleFont.setBold(true);
    title->setFont(titleFont);
    title->setAlignment(Qt::AlignCenter);

    auto *desc = new QLabel(
        "Set up your username, password, and computer name for the installed system.<br>"
        "Please choose a strong password and a unique hostname.",
        this
    );
    desc->setAlignment(Qt::AlignCenter);
    desc->setWordWrap(true);
    desc->setTextFormat(Qt::RichText);

    // Username
    auto *userLabel = new QLabel("Username:", this);
    usernameEdit = new QLineEdit(this);
    usernameEdit->setPlaceholderText("e.g. oscyra");
    usernameEdit->setMaxLength(32);

    // Password
    auto *passLabel = new QLabel("Password:", this);
    passwordEdit = new QLineEdit(this);
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setPlaceholderText("Password");

    // Password Confirm
    auto *passConfLabel = new QLabel("Confirm Password:", this);
    passwordConfirmEdit = new QLineEdit(this);
    passwordConfirmEdit->setEchoMode(QLineEdit::Password);
    passwordConfirmEdit->setPlaceholderText("Retype password");

    // Hostname
    auto *hostLabel = new QLabel("Hostname:", this);
    hostnameEdit = new QLineEdit(this);
    hostnameEdit->setPlaceholderText("e.g. oscyra-pc");
    hostnameEdit->setMaxLength(32);

    errorLabel = new QLabel(this);
    errorLabel->setStyleSheet("color: #ff5555;");
    errorLabel->setAlignment(Qt::AlignCenter);

    continueButton = new QPushButton("Continue", this);
    continueButton->setFixedWidth(180);
    continueButton->setMinimumHeight(36);
    continueButton->setEnabled(false);

    title->setStyleSheet("color: #e0e6f0;");
    desc->setStyleSheet("color: #c0c6d0;");
    continueButton->setStyleSheet(
        "background-color: #29304a; color: #fff; border-radius: 8px; font-size: 16px; padding: 8px 18px;"
        "border: 1px solid #3a3f5a;"
    );

    // Layout fields
    layout->addStretch();
    layout->addWidget(title);
    layout->addSpacing(10);
    layout->addWidget(desc);
    layout->addSpacing(10);

    layout->addWidget(userLabel);
    layout->addWidget(usernameEdit);

    layout->addWidget(passLabel);
    layout->addWidget(passwordEdit);

    layout->addWidget(passConfLabel);
    layout->addWidget(passwordConfirmEdit);

    layout->addWidget(hostLabel);
    layout->addWidget(hostnameEdit);

    layout->addSpacing(8);
    layout->addWidget(errorLabel);
    layout->addSpacing(18);
    layout->addWidget(continueButton, 0, Qt::AlignCenter);
    layout->addStretch();

    // Validate on input
    connect(usernameEdit, &QLineEdit::textChanged, this, &UserPage::validateInput);
    connect(passwordEdit, &QLineEdit::textChanged, this, &UserPage::validateInput);
    connect(passwordConfirmEdit, &QLineEdit::textChanged, this, &UserPage::validateInput);
    connect(hostnameEdit, &QLineEdit::textChanged, this, &UserPage::validateInput);

    connect(continueButton, &QPushButton::clicked, [this]() {
        emit continueClicked(username(), password(), hostname());
    });
}

QString UserPage::username() const {
    return usernameEdit->text().trimmed();
}
QString UserPage::password() const {
    return passwordEdit->text();
}
QString UserPage::hostname() const {
    return hostnameEdit->text().trimmed();
}

void UserPage::validateInput() {
    QString user = username();
    QString pass = password();
    QString passConf = passwordConfirmEdit->text();
    QString host = hostname();

    if (user.isEmpty() || pass.isEmpty() || passConf.isEmpty() || host.isEmpty()) {
        errorLabel->setText("All fields are required.");
        continueButton->setEnabled(false);
        return;
    }
    if (user.contains(' ')) {
        errorLabel->setText("Username cannot contain spaces.");
        continueButton->setEnabled(false);
        return;
    }
    if (pass != passConf) {
        errorLabel->setText("Passwords do not match.");
        continueButton->setEnabled(false);
        return;
    }
    if (user.length() < 2) {
        errorLabel->setText("Username is too short.");
        continueButton->setEnabled(false);
        return;
    }
    if (host.contains(' ')) {
        errorLabel->setText("Hostname cannot contain spaces.");
        continueButton->setEnabled(false);
        return;
    }
    errorLabel->setText("");
    continueButton->setEnabled(true);
}
