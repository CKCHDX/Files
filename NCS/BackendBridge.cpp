#include "BackendBridge.h"
#include <QProcess>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

BackendBridge::BackendBridge(QObject *parent) : QObject(parent)
{
}

void BackendBridge::startScan()
{
    emit logMessage("Starting network scan...");

    QProcess proc;
    proc.start("python3", QStringList() << "backend/network_monitor.py");
    if(!proc.waitForFinished(5000)) {
        emit logMessage("Scan timed out.");
        emit scanResults(QVariantList());
        return;
    }

    QString output = proc.readAllStandardOutput();

    QVariantList nets;
    QJsonDocument doc = QJsonDocument::fromJson(output.toUtf8());
    if(doc.isArray()) {
        QJsonArray arr = doc.array();
        for(const QJsonValue &val : arr) {
            QVariantMap map = val.toObject().toVariantMap();
            nets.append(map);
        }
    }
    emit scanResults(nets);
    emit logMessage("Scan completed.");
}

void BackendBridge::clear()
{
    emit logMessage("Clearing network list...");
    emit scanResults(QVariantList());
}
