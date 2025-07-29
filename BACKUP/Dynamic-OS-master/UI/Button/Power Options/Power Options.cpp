#include "Power Options.h"
#include <QProcess>
#include <QDebug>

PowerOptionsUI::PowerOptionsUI(QWidget* parent)
: QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    QLabel* title = new QLabel("Power Options", this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 22px; color: white;");

    powerOffButton = new QPushButton("Power Off", this);
    rebootButton   = new QPushButton("Reboot", this);
    logOffButton   = new QPushButton("Log Off", this);

    QString btnStyle =
    "QPushButton {"
    "  background-color: #a83232;"
    "  color: white;"
    "  border-radius: 12px;"
    "  padding: 10px 0;"
    "  font-size: 16px;"
    "  margin: 8px 0;"
    "}"
    "QPushButton:hover {"
    "  background-color: #d14a4a;"
    "}";

    powerOffButton->setStyleSheet(btnStyle);
    rebootButton->setStyleSheet(btnStyle.replace("#a83232", "#3250a8").replace("#d14a4a", "#4a6ad1"));
    logOffButton->setStyleSheet(btnStyle.replace("#a83232", "#32a852").replace("#d14a4a", "#4ad17c"));

    layout->addWidget(title);
    layout->addSpacing(16);
    layout->addWidget(powerOffButton);
    layout->addWidget(rebootButton);
    layout->addWidget(logOffButton);
    layout->addStretch();

    connect(powerOffButton, &QPushButton::clicked, this, &PowerOptionsUI::shutdownSystem);
    connect(rebootButton, &QPushButton::clicked, this, &PowerOptionsUI::rebootSystem);
    connect(logOffButton, &QPushButton::clicked, this, &PowerOptionsUI::logoffSystem);

    setStyleSheet("background-color: #1e1e1e;");
}

int PowerOptionsUI::showDarkMessageBox(const QString& title, const QString& message)
{
    QMessageBox box(this);
    box.setWindowTitle(title);
    box.setText(message);
    box.setIcon(QMessageBox::Question);
    box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    box.setStyleSheet(R"(
        QMessageBox {
            background-color: #2b2b2b;
            color: #ffffff;
            font-size: 14px;
        }
        QPushButton {
            background-color: #444;
            color: white;
            padding: 6px 12px;
            border: 1px solid #555;
            border-radius: 6px;
        }
        QPushButton:hover {
            background-color: #555;
        }
    )");
    return box.exec();
}

void PowerOptionsUI::shutdownSystem()
{
    if (showDarkMessageBox("Shutdown", "Are you sure you want to power off?") == QMessageBox::Yes) {
        QProcess::startDetached("systemctl", QStringList() << "poweroff");
    }
}

void PowerOptionsUI::rebootSystem()
{
    if (showDarkMessageBox("Reboot", "Are you sure you want to reboot?") == QMessageBox::Yes) {
        QProcess::startDetached("systemctl", QStringList() << "reboot");
    }
}

void PowerOptionsUI::logoffSystem()
{
    if (showDarkMessageBox("Log Off", "Are you sure you want to log off?") == QMessageBox::Yes) {
        // Prepare the process
        QProcess proc;
        QStringList args;
        args << "show-session" << qgetenv("XDG_SESSION_ID") << "-p" << "Id";
        proc.start("loginctl", args);
        proc.waitForFinished();
        QString output = proc.readAllStandardOutput();

        // Extract the session ID from output (format: Id=SESSION_ID)
        QString sessionId;
        for (const QString &line : output.split('\n')) {
            if (line.startsWith("Id=")) {
                sessionId = line.section('=', 1, 1).trimmed();
                break;
            }
        }

        if (!sessionId.isEmpty()) {
            QProcess::startDetached("loginctl", QStringList() << "terminate-session" << sessionId);
        } else {
            QProcess::startDetached("loginctl", QStringList() << "terminate-user" << qgetenv("USER"));
        }
    }
}

