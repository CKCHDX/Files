#include "Built-In Apps.h"
#include "Apps/CustomAppWindow.h"
#include "Apps/WebBrowser.h"
#include <QMessageBox>
#include <QProcess>
BuiltInApps::BuiltInApps(QWidget* parent)
: QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    QLabel* label = new QLabel("Built-In Apps", this);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet("font-size: 24px; font-weight: bold; margin: 10px;");

    // Helper lambda for button style
    auto buttonStyle = []
    {
        return QString(
            "QPushButton {"
            "  border-radius: 32px;"
            "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #1e1e1e, stop:1 #5858ff);"
            "  color: white;"
            "  font-size: 22px;"
            "  font-weight: bold;"
            "  margin: 16px;"
            "  padding: 24px;"
            "}"
            "QPushButton:hover {"
            "  background: #7ecb20;"
            "  color: #222;"
            "}"
        );
    };

    webBrowserButton = new QPushButton("Web Browser", this);
    webBrowserButton->setMinimumHeight(80);
    webBrowserButton->setStyleSheet(buttonStyle());

    calculatorButton = new QPushButton("Calculator", this);
    calculatorButton->setMinimumHeight(80);
    calculatorButton->setStyleSheet(buttonStyle());

    notesButton = new QPushButton("Notes", this);
    notesButton->setMinimumHeight(80);
    notesButton->setStyleSheet(buttonStyle());

    terminalButton = new QPushButton("Terminal", this);
    terminalButton->setMinimumHeight(80);
    terminalButton->setStyleSheet(buttonStyle());

    layout->addWidget(label);
    layout->addWidget(webBrowserButton);
    layout->addWidget(calculatorButton);
    layout->addWidget(notesButton);
    layout->addWidget(terminalButton);
    layout->addStretch();

    connect(webBrowserButton, &QPushButton::clicked, this, &BuiltInApps::onWebBrowserClicked);
    connect(calculatorButton, &QPushButton::clicked, this, &BuiltInApps::onCalculatorClicked);
    connect(notesButton, &QPushButton::clicked, this, &BuiltInApps::onNotesClicked);
    connect(terminalButton, &QPushButton::clicked, this, &BuiltInApps::onTerminalClicked);

    setLayout(layout);
}

void BuiltInApps::onWebBrowserClicked()
{
    CustomAppWindow* window = new CustomAppWindow(new WebBrowser, "Web Browser");
    window->setAttribute(Qt::WA_DeleteOnClose);
    window->show();
}

void BuiltInApps::onCalculatorClicked()
{
    QMessageBox::information(this, "Coming Soon", "Calculator is coming soon!");
}

void BuiltInApps::onNotesClicked()
{
    QMessageBox::information(this, "Coming Soon", "Notes is coming soon!");
}

void BuiltInApps::onTerminalClicked()
{
    // Try common Linux terminals in order
    QStringList terminals = { "konsole", "gnome-terminal", "xterm", "lxterminal", "xfce4-terminal", "mate-terminal", "tilix", "alacritty", "urxvt" };
    bool launched = false;

    for (const QString& term : terminals) {
        if (QProcess::startDetached(term)) {
            launched = true;
            break;
        }
    }

    if (!launched) {
        QMessageBox::warning(this, "Terminal Not Found", "No supported terminal emulator was found on your system.");
    }
}
