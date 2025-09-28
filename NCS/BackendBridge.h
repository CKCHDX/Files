#ifndef BACKENDBRIDGE_H
#define BACKENDBRIDGE_H

#include <QObject>
#include <QVariantList>

class BackendBridge : public QObject
{
    Q_OBJECT

public:
    explicit BackendBridge(QObject *parent = nullptr);

public slots:
    void startScan();
    void clear();

signals:
    void scanResults(QVariantList networks);
    void logMessage(const QString &message);
};

#endif // BACKENDBRIDGE_H
