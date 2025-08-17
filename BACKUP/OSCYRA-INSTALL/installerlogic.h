#ifndef INSTALLERLOGIC_H
#define INSTALLERLOGIC_H

#include <QObject>
#include <QString>
#include <QStringList>

class InstallerLogic : public QObject {
    Q_OBJECT
public:
    explicit InstallerLogic(QObject *parent = nullptr);

    void installSystem(const QString &keyboardLayout,
                       const QString &disk,
                       const QString &username,
                       const QString &password,
                       const QString &hostname);

signals:
    void progressChanged(const QString &status, int percent);
    void installFinished(bool success, const QString &error);

private:
    bool partitionAndFormatDisk(const QString &disk, QString &error);
    bool mountTarget(const QString &disk, QString &error);
    bool copySystemFiles(QString &error);
    bool writeConfigs(const QString &keyboardLayout, const QString &username, const QString &hostname, QString &error);
    bool createUser(const QString &username, const QString &password, QString &error);
    bool installBootloader(const QString &disk, QString &error);
    bool cleanupGuest(QString &error);
    bool ensurePackagesInstalled(QString &error);

    QStringList getRequiredPackages() const;
    bool isPackageInstalled(const QString &pkg);
    bool isNetworkOnline();
    void promptForNetwork();

    QString targetRoot; // e.g. /mnt/target
};

#endif // INSTALLERLOGIC_H
