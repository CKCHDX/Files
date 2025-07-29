#ifndef NETWORK_H
#define NETWORK_H

#include <QWidget>
#include <QTimer>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <QGridLayout>
#include <QMap>

class Network : public QWidget
{
    Q_OBJECT

public:
    explicit Network(QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *event) override;

private slots:
    void refreshNetworks();
    void disconnectFromNetwork();
    void updateStatusBar();
    void handleNetworkButton();

private:
    QString signalBars(int bars) const;  // Changed from static to member function
    bool isConnected(const QString &ssid);
    bool isPasswordRemembered(const QString &ssid);
    int signalStrengthToBars(int strength);
    void showPasswordDialog(const QString &ssid);
    void autoSelectConnectedNetwork();

    QTimer *statusTimer;
    QLabel *statusBarLabel;
    QPushButton *refreshButton;
    QPushButton *disconnectButton;
    QScrollArea *scrollArea;
    QWidget *networksWidget;
    QGridLayout *networksLayout;
    QMap<QString, QPushButton*> ssidToButton;
    QMap<QString, int> ssidToSignal;
};

#endif // NETWORK_H
