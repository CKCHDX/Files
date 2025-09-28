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

    QProcess *proc = new QProcess(this);

    connect(proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this, proc](int exitCode, QProcess::ExitStatus exitStatus){
        if(exitStatus == QProcess::NormalExit && exitCode == 0) {
            QString output = proc->readAllStandardOutput();

            QVariantList networks;
            QJsonDocument doc = QJsonDocument::fromJson(output.toUtf8());
            if(doc.isArray()) {
                QJsonArray arr = doc.array();
                for(const QJsonValue &val : arr) {
                    QVariantMap map = val.toObject().toVariantMap();
                    networks.append(map);
                }
            }
            emit scanResults(networks);
            emit logMessage("Scan completed.");
        } else {
            emit logMessage("Scan process failed or aborted.");
            emit scanResults(QVariantList());
        }
        proc->deleteLater();
    });

    // Optional: handle error signals if needed

    proc->start("python3", QStringList() << "backend/network_monitor.py");
}

void BackendBridge::clear()
{
    emit logMessage("Clearing network list...");
    emit scanResults(QVariantList());
}
