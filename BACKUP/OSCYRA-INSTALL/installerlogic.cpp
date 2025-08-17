#include "installerlogic.h"
#include <QProcess>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDebug>
#include <QCoreApplication>

InstallerLogic::InstallerLogic(QObject *parent)
    : QObject(parent), targetRoot("/mnt/target")
{}

void InstallerLogic::installSystem(const QString &keyboardLayout,
                                   const QString &disk,
                                   const QString &username,
                                   const QString &password,
                                   const QString &hostname)
{
    QString error;

    emit progressChanged("Partitioning and formatting disk...", 5);
    if (!partitionAndFormatDisk(disk, error)) {
        emit installFinished(false, "Partition/format failed: " + error);
        return;
    }

    emit progressChanged("Mounting target partition...", 10);
    if (!mountTarget(disk, error)) {
        emit installFinished(false, "Mount failed: " + error);
        return;
    }

    emit progressChanged("Copying system files...", 30);
    if (!copySystemFiles(error)) {
        emit installFinished(false, "Copy failed: " + error);
        return;
    }

    emit progressChanged("Writing configuration files...", 60);
    if (!writeConfigs(keyboardLayout, username, hostname, error)) {
        emit installFinished(false, "Config write failed: " + error);
        return;
    }

    emit progressChanged("Creating user...", 70);
    if (!createUser(username, password, error)) {
        emit installFinished(false, "User creation failed: " + error);
        return;
    }

    emit progressChanged("Ensuring required packages...", 80);
    if (!ensurePackagesInstalled(error)) {
        emit installFinished(false, "Package install failed: " + error);
        return;
    }

    emit progressChanged("Installing bootloader...", 85);
    if (!installBootloader(disk, error)) {
        emit installFinished(false, "Bootloader install failed: " + error);
        return;
    }

    emit progressChanged("Cleaning up...", 95);
    if (!cleanupGuest(error)) {
        emit installFinished(false, "Cleanup failed: " + error);
        return;
    }

    QFile flagFile(targetRoot + "/etc/oscyra_installed");
    flagFile.open(QIODevice::WriteOnly);
    flagFile.close();

    emit progressChanged("Installation complete!", 100);
    emit installFinished(true, "");
}

bool InstallerLogic::partitionAndFormatDisk(const QString &disk, QString &error) {
    // Determine partition suffix: if disk name ends in a digit (nvme0n1) → p1, else → 1
    QString part = disk;
    if (disk.back().isDigit())
        part += "p1";
    else
        part += "1";

    // 1) Create GPT and a single ext4 partition
    const QString cmd = QString(
        "parted %1 --script mklabel gpt mkpart primary ext4 1MiB 100%% && "
        "mkfs.ext4 -F %2"
    ).arg(disk, part);

    QProcess proc;
    proc.start("bash", QStringList{ "-c", cmd });
    if (!proc.waitForFinished(60000) || proc.exitCode() != 0) {
        error = proc.readAllStandardError();
        return false;
    }
    return true;
}

bool InstallerLogic::mountTarget(const QString &disk, QString &error) {
    // Same suffix logic
    QString part = disk;
    if (disk.back().isDigit())
        part += "p1";
    else
        part += "1";

    QProcess proc;
    proc.start("mount", QStringList{ part, targetRoot });
    if (!proc.waitForFinished(10000) || proc.exitCode() != 0) {
        error = proc.readAllStandardError();
        return false;
    }
    return true;
}




bool InstallerLogic::copySystemFiles(QString &error) {
    QProcess proc;
    proc.start("rsync", QStringList()
        << "-aAXv"
        << "--exclude=/proc"
        << "--exclude=/sys"
        << "--exclude=/dev"
        << "--exclude=/tmp"
        << "--exclude=/run"
        << "--exclude=/mnt"
        << "--exclude=/media"
        << "--exclude=/lost+found"
        << "/"
        << targetRoot);
    if (!proc.waitForFinished(-1) || proc.exitCode() != 0) {
        error = proc.readAllStandardError();
        return false;
    }
    return true;
}

bool InstallerLogic::writeConfigs(const QString &keyboardLayout, const QString &username, const QString &hostname, QString &error) {
    // Keyboard
    QFile kb(targetRoot + "/etc/default/keyboard");
    if (kb.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&kb);
        out << "XKBLAYOUT=\"" << keyboardLayout << "\"\n";
        kb.close();
    }
    // Hostname
    QFile hn(targetRoot + "/etc/hostname");
    if (hn.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&hn);
        out << hostname << "\n";
        hn.close();
    }
    return true;
}

bool InstallerLogic::createUser(const QString &username, const QString &password, QString &error) {
    // Chroot and create user
    QString cmd = QString(
        "chroot %1 /bin/bash -c 'useradd -m -G wheel,sudo,audio,video,network -s /bin/bash %2 && "
        "echo \"%2:%3\" | chpasswd'").arg(targetRoot, username, password);
    QProcess proc;
    proc.start("bash", QStringList() << "-c" << cmd);
    if (!proc.waitForFinished(10000) || proc.exitCode() != 0) {
        error = proc.readAllStandardError();
        return false;
    }
    // Remove guest user
    proc.start("chroot", QStringList() << targetRoot << "userdel" << "-r" << "guest");
    proc.waitForFinished(5000);
    return true;
}

bool InstallerLogic::installBootloader(const QString &disk, QString &error) {
    QString grubCmd = QString("grub-install --target=i386-pc --boot-directory=%1/boot %2")
        .arg(targetRoot, disk);
    QProcess proc;
    proc.start("bash", QStringList() << "-c" << grubCmd);
    if (!proc.waitForFinished(20000) || proc.exitCode() != 0) {
        error = proc.readAllStandardError();
        return false;
    }
    QString grubCfgCmd = QString("chroot %1 grub-mkconfig -o /boot/grub/grub.cfg").arg(targetRoot);
    proc.start("bash", QStringList() << "-c" << grubCfgCmd);
    proc.waitForFinished(10000);
    return true;
}

bool InstallerLogic::cleanupGuest(QString &error) {
    QDir guestHome(targetRoot + "/home/guest");
    if (guestHome.exists()) {
        guestHome.removeRecursively();
    }
    return true;
}

// --------- PACKAGE MANAGEMENT -----------

QStringList InstallerLogic::getRequiredPackages() const {
    QStringList packages;
    QFile pkgFile("/usr/bin/oscyra.txt");
    if (pkgFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&pkgFile);
        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (!line.isEmpty() && !line.startsWith("#"))
                packages << line;
        }
        pkgFile.close();
    }
    return packages;
}

bool InstallerLogic::isPackageInstalled(const QString &pkg) {
    // Try dpkg-query (Debian/Ubuntu) inside chroot
    QProcess proc;
    QString cmd = QString("chroot %1 dpkg-query -W -f='${Status}' %2 2>/dev/null | grep -q 'install ok installed'").arg(targetRoot, pkg);
    proc.start("bash", QStringList() << "-c" << cmd);
    proc.waitForFinished(3000);
    return proc.exitCode() == 0;
}

bool InstallerLogic::isNetworkOnline() {
    QProcess proc;
    proc.start("ping", QStringList() << "-c1" << "-w2" << "8.8.8.8");
    proc.waitForFinished(3000);
    return proc.exitCode() == 0;
}

void InstallerLogic::promptForNetwork() {
    // Use kdialog if available, otherwise just print
    QProcess::execute("kdialog", QStringList() << "--msgbox" << "Network connection required to install all packages.\nPlease connect to WiFi and press OK.");
}

bool InstallerLogic::ensurePackagesInstalled(QString &error) {
    QStringList pkgs = getRequiredPackages();
    QStringList missing;
    for (const QString &pkg : pkgs) {
        if (!isPackageInstalled(pkg)) {
            missing << pkg;
        }
    }
    if (missing.isEmpty())
        return true;

    // Check network
    if (!isNetworkOnline()) {
        promptForNetwork();
        if (!isNetworkOnline()) {
            error = "No network connection. Some packages could not be installed.";
            return false;
        }
    }

    // Try to install missing packages (ignore failures)
    for (const QString &pkg : missing) {
        emit progressChanged(QString("Installing package: %1").arg(pkg), 82);
        QProcess proc;
        QString cmd = QString("chroot %1 apt-get -y install %2").arg(targetRoot, pkg);
        proc.start("bash", QStringList() << "-c" << cmd);
        proc.waitForFinished(-1);
        // Ignore failures, just continue
    }
    return true;
}
