#ifndef DISK_PAGE_H
#define DISK_PAGE_H

#include <QWidget>
#include <QVector>

struct DiskInfo {
    QString devicePath;    // /dev/sda, /dev/nvme0n1, etc.
    QString size;          // e.g. "465G"
    bool    isRemovable;
};

struct PartitionInfo {
    QString partPath;      // /dev/sda1
    QString size;          // "100G"
    QString fsType;        // "ext4", "ntfs", etc.
};

class QPushButton;
class QLabel;
class QButtonGroup;

class DiskPage : public QWidget {
    Q_OBJECT
public:
    explicit DiskPage(QWidget *parent = nullptr);

signals:
    /** Emitted when the user has chosen exactly one target device/partition to install onto. */
    void continueClicked(const QString &targetDevice);

private slots:
    void onDiskSelected(int id);
    void onAutoPartition();
    void onManualPartition();

private:
    void loadDisks();
    void showPartitionModeDialog();
    QString promptSizeGiB(const QString &title, double minGiB);

    QVector<DiskInfo> disks;
    int selectedDiskIdx = -1;

    // UI
    QButtonGroup *diskGroup;
    QLabel       *instructionLabel;
    QPushButton  *continueButton;
};

#endif // DISK_PAGE_H
