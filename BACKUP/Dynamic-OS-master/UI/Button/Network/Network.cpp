#include "Network.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QInputDialog>
#include <QMessageBox>
#include <QProcess>
#include <QDebug>
#include <QStyleOption>
#include <QPainter>
#include <QDBusConnection>
#include <QTimer>

QString Network::signalBars(int bars) const
{
    switch (bars) {
        case 4: return "▂▄▆█";
        case 3: return "▂▄▆";
        case 2: return "▂▄";
        case 1: return "▂";
        default: return "";
    }
}


Network::Network(QWidget *parent)
: QWidget(parent),
statusTimer(new QTimer(this))
{
    // Ensure proper rendering in minimal environments
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_AlwaysStackOnTop);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground, false);

    // Initialize DBus connection
    if (!QDBusConnection::systemBus().isConnected()) {
        qWarning("Cannot connect to system bus!");
    }

    // Status bar
    statusBarLabel = new QLabel(this);
    statusBarLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    statusBarLabel->setStyleSheet(R"(
        font-weight: bold;
        font-size: 16px;
        background: transparent;
        color: white;
    )");

    refreshButton = new QPushButton("Refresh", this);
    disconnectButton = new QPushButton("Disconnect", this);

    // Style buttons consistently
    QString buttonStyle = R"(
        QPushButton {
            border-radius: 6px;
            padding: 8px;
            background: #444;
            color: white;
            font-weight: bold;
        }
        QPushButton:hover {
            background: #555;
        }
        QPushButton:pressed {
            background: #333;
        }
    )";
    refreshButton->setStyleSheet(buttonStyle);
    disconnectButton->setStyleSheet(buttonStyle);

    // Network grid area
    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("background: transparent; border: none;");

    networksWidget = new QWidget(scrollArea);
    networksWidget->setStyleSheet(R"(
        background-color: rgba(40, 40, 40, 220);
        border-radius: 8px;
    )");

    networksLayout = new QGridLayout(networksWidget);
    networksLayout->setSpacing(20);
    networksLayout->setContentsMargins(20, 20, 20, 20);
    networksWidget->setLayout(networksLayout);
    scrollArea->setWidget(networksWidget);

    // Layouts
    QHBoxLayout* topBar = new QHBoxLayout;
    topBar->setContentsMargins(0, 0, 0, 0);
    topBar->addWidget(refreshButton);
    topBar->addStretch();
    topBar->addWidget(disconnectButton);
    topBar->addWidget(statusBarLabel);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);
    mainLayout->addLayout(topBar);
    mainLayout->addWidget(scrollArea);

    setLayout(mainLayout);

    // Force initial paint
    QTimer::singleShot(100, this, [this]() {
        update();
        networksWidget->update();
    });

    // Connections
    connect(refreshButton, &QPushButton::clicked, this, &Network::refreshNetworks);
    connect(disconnectButton, &QPushButton::clicked, this, &Network::disconnectFromNetwork);
    connect(statusTimer, &QTimer::timeout, this, &Network::updateStatusBar);

    statusTimer->start(3000);

    refreshNetworks();
    updateStatusBar();
}

void Network::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QWidget::paintEvent(event);
}

void Network::refreshNetworks()
{
    // Clear existing networks
    qDeleteAll(networksWidget->findChildren<QPushButton*>());
    ssidToButton.clear();
    ssidToSignal.clear();

    // Get network list
    QProcess proc;
    proc.start("nmcli -t -f SSID,SIGNAL dev wifi list");
    proc.waitForFinished();
    QString output = proc.readAllStandardOutput();
    QStringList lines = output.split('\n', Qt::SkipEmptyParts);

    // Process networks
    for (const QString& line : lines) {
        QStringList parts = line.split(':');
        if (parts.size() >= 2) {
            QString ssid = parts[0].trimmed();
            int signal = parts[1].toInt();
            if (!ssid.isEmpty()) {
                if (!ssidToSignal.contains(ssid) || ssidToSignal[ssid] < signal) {
                    ssidToSignal[ssid] = signal;
                }
            }
        }
    }

    // Create network buttons
    int colCount = qMax(3, width() / 200); // Responsive columns
    int row = 0, col = 0;

    for (auto it = ssidToSignal.constBegin(); it != ssidToSignal.constEnd(); ++it) {
        QString ssid = it.key();
        int bars = signalStrengthToBars(it.value());

        QPushButton* btn = new QPushButton(
            QString("%1\n%2").arg(ssid).arg(signalBars(bars)),
                                           networksWidget
        );

        btn->setCheckable(true);
        btn->setMinimumSize(160, 90);
        btn->setStyleSheet(R"(
            QPushButton {
                border-radius: 24px;
                background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #3c3c3c, stop:1 #5858ff);
                color: white;
                font-size: 18px;
                font-weight: bold;
                padding: 12px;
                margin: 8px;
                border: 2px solid transparent;
            }
            QPushButton:hover {
                background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #5858ff, stop:1 #3c3c3c);
                border: 2px solid #7ecb20;
                box-shadow: 0 0 12px #7ecb20;
            }
            QPushButton:pressed {
                background: #2222aa;
                color: #fff;
                border: 2px solid #fff;
            }
            QPushButton:checked {
                background: #7ecb20;
                color: black;
                border: 2px solid #333;
            }
        )");

        connect(btn, &QPushButton::clicked, this, &Network::handleNetworkButton);
        ssidToButton[ssid] = btn;
        networksLayout->addWidget(btn, row, col);

        if (++col >= colCount) {
            col = 0;
            row++;
        }
    }

    autoSelectConnectedNetwork();
    updateStatusBar();
}

void Network::autoSelectConnectedNetwork()
{
    QProcess proc;
    proc.start("nmcli -t -f active,ssid dev wifi");
    proc.waitForFinished();
    QString output = proc.readAllStandardOutput();

    QString connectedSsid;
    for (const QString& line : output.split('\n', Qt::SkipEmptyParts)) {
        if (line.startsWith("yes:")) {
            connectedSsid = line.section(':', 1, 1);
            break;
        }
    }

    if (!connectedSsid.isEmpty() && ssidToButton.contains(connectedSsid)) {
        ssidToButton[connectedSsid]->setChecked(true);
        ssidToButton[connectedSsid]->setFocus();
    }
}

void Network::handleNetworkButton()
{
    QPushButton* senderBtn = qobject_cast<QPushButton*>(sender());
    if (!senderBtn) return;

    // Uncheck all buttons
    for (auto btn : ssidToButton) {
        btn->setChecked(false);
    }
    senderBtn->setChecked(true);

    // Find which SSID was clicked
    QString ssid;
    for (auto it = ssidToButton.constBegin(); it != ssidToButton.constEnd(); ++it) {
        if (it.value() == senderBtn) {
            ssid = it.key();
            break;
        }
    }

    if (ssid.isEmpty()) return;

    if (!isConnected(ssid)) {
        if (isPasswordRemembered(ssid)) {
            QProcess::execute(QString("nmcli con up id \"%1\"").arg(ssid));
        } else {
            showPasswordDialog(ssid);
        }
        refreshNetworks();
    }
}

void Network::disconnectFromNetwork()
{
    QProcess proc;
    proc.start("nmcli -t -f active,ssid dev wifi");
    proc.waitForFinished();
    QString output = proc.readAllStandardOutput();

    QString connectedSsid;
    for (const QString& line : output.split('\n', Qt::SkipEmptyParts)) {
        if (line.startsWith("yes:")) {
            connectedSsid = line.section(':', 1, 1);
            break;
        }
    }

    if (!connectedSsid.isEmpty()) {
        QProcess::execute(QString("nmcli con down id \"%1\"").arg(connectedSsid));
        refreshNetworks();
    } else {
        QMessageBox::information(this, "Not Connected", "You are not connected to any network.");
    }
}

void Network::updateStatusBar()
{
    QProcess proc;
    proc.start("nmcli -t -f active,ssid dev wifi");
    proc.waitForFinished();
    QString output = proc.readAllStandardOutput();

    QString ssid;
    for (const QString& line : output.split('\n', Qt::SkipEmptyParts)) {
        if (line.startsWith("yes:")) {
            ssid = line.section(':', 1, 1);
            break;
        }
    }

    if (!ssid.isEmpty()) {
        statusBarLabel->setText(
            QString("<span style='color:#7ecb20;'>● Connected</span> to <b>%1</b>").arg(ssid)
        );
    } else {
        statusBarLabel->setText("<span style='color:#ff5555;'>● Not Connected</span>");
    }
}

bool Network::isConnected(const QString &ssid)
{
    QProcess proc;
    proc.start("nmcli -t -f active,ssid dev wifi");
    proc.waitForFinished();
    QString output = proc.readAllStandardOutput();

    for (const QString& line : output.split('\n', Qt::SkipEmptyParts)) {
        if (line.startsWith("yes:")) {
            QString connectedSsid = line.section(':', 1, 1);
            return connectedSsid == ssid;
        }
    }
    return false;
}

bool Network::isPasswordRemembered(const QString &ssid)
{
    QProcess proc;
    proc.start(QString("nmcli -g 802-11-wireless-security.psk connection show \"%1\"").arg(ssid));
    proc.waitForFinished();
    return !proc.readAllStandardOutput().trimmed().isEmpty();
}

int Network::signalStrengthToBars(int strength)
{
    if (strength >= 80) return 4;
    if (strength >= 55) return 3;
    if (strength >= 30) return 2;
    if (strength >= 10) return 1;
    return 0;
}

void Network::showPasswordDialog(const QString &ssid)
{
    bool ok;
    QString password = QInputDialog::getText(
        this,
        "Enter Password",
        QString("Password for %1:").arg(ssid),
                                             QLineEdit::Password,
                                             "",
                                             &ok
    );

    if (ok && !password.isEmpty()) {
        int result = QProcess::execute(
            QString("nmcli dev wifi connect \"%1\" password \"%2\"").arg(ssid, password)
        );

        if (result != 0) {
            QMessageBox::critical(this, "Failed", "Connection failed. Check password.");
        }
    }
}
