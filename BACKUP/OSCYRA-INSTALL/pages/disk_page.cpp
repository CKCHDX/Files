#include "disk_page.h"
#include <QVBoxLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QLabel>
#include <QButtonGroup>
#include <QFont>
#include <QInputDialog>
#include <QMessageBox>
#include <QProcess>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

DiskPage::DiskPage(QWidget *parent)
    : QWidget(parent)
{
    auto *mainLayout = new QVBoxLayout(this);

    // Title
    auto *title = new QLabel("1) Select Disk to Install On", this);
    QFont tfont = title->font(); tfont.setPointSize(18); tfont.setBold(true);
    title->setFont(tfont);
    title->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(title);

    // Scroll area with disk buttons
    auto *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    auto *container = new QWidget(this);
    auto *vbox = new QVBoxLayout(container);
    scroll->setWidget(container);
    mainLayout->addWidget(scroll);

    diskGroup = new QButtonGroup(this);
    diskGroup->setExclusive(true);
    connect(diskGroup, QOverload<int>::of(&QButtonGroup::idClicked),
            this, &DiskPage::onDiskSelected);

    // Instruction
    instructionLabel = new QLabel("Please select a disk above.", this);
    instructionLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(instructionLabel);

    // Continue (initially disabled)
    continueButton = new QPushButton("Continue", this);
    continueButton->setEnabled(false);
    mainLayout->addWidget(continueButton, 0, Qt::AlignCenter);
    connect(continueButton, &QPushButton::clicked,
            this, &DiskPage::showPartitionModeDialog);

    // Load disks
    loadDisks();
    for (int i = 0; i < disks.size(); ++i) {
        const auto &d = disks[i];
        QString text = QString("%1  (%2)").arg(d.devicePath, d.size);
        if (d.isRemovable) text += "  [USB]";
        auto *btn = new QPushButton(text, this);
        btn->setCheckable(true);
        diskGroup->addButton(btn, i);
        vbox->addWidget(btn);
    }
    vbox->addStretch();
}

void DiskPage::loadDisks() {
    // Use lsblk JSON to enumerate block devices of type "disk"
    QProcess p;
    p.start("lsblk", QStringList{"-J", "-d", "-o", "NAME,SIZE,TYPE,TRAN"});
    p.waitForFinished(2000);
    auto doc = QJsonDocument::fromJson(p.readAllStandardOutput());
    if (!doc.isObject()) return;
    auto arr = doc.object().value("blockdevices").toArray();

    disks.clear();
    for (auto val : arr) {
        auto o = val.toObject();
        if (o.value("type") != "disk") continue;
        DiskInfo d;
        d.devicePath  = "/dev/" + o.value("name").toString();
        d.size        = o.value("size").toString();
        d.isRemovable = (o.value("tran").toString() == "usb");
        disks.append(d);
    }
}

void DiskPage::onDiskSelected(int id) {
    selectedDiskIdx = id;
    continueButton->setEnabled(true);
    instructionLabel->setText(
        QString("Disk %1 selected. Click Continue to choose partitioning mode.")
        .arg(disks[id].devicePath)
    );
}

void DiskPage::showPartitionModeDialog() {
    if (selectedDiskIdx < 0) return;

    // Dialog with two options
    QMessageBox box(this);
    box.setWindowTitle("Partitioning Mode");
    box.setText(
        QString("You selected: <b>%1 (%2)</b><br><br>"
                "Choose how to partition this disk:")
        .arg(disks[selectedDiskIdx].devicePath, disks[selectedDiskIdx].size)
    );
    auto *autoBtn   = box.addButton("Auto Partition", QMessageBox::AcceptRole);
    auto *manualBtn = box.addButton("Manual Partition", QMessageBox::DestructiveRole);
    box.addButton("Cancel", QMessageBox::RejectRole);
    box.exec();

    if (box.clickedButton() == autoBtn) {
        onAutoPartition();
    } else if (box.clickedButton() == manualBtn) {
        onManualPartition();
    }
}

void DiskPage::onAutoPartition() {
    // Ask: entire disk or alongside?
    QMessageBox ask(this);
    ask.setWindowTitle("Auto Partition");
    ask.setText(
        "1) Wipe entire disk and use whole disk for installation\n"
        "2) Install alongside existing OS (create new partition)"
    );
    auto *wipeBtn = ask.addButton("Use Entire Disk", QMessageBox::AcceptRole);
    auto *alongBtn= ask.addButton("Install Alongside", QMessageBox::AcceptRole);
    ask.addButton("Cancel", QMessageBox::RejectRole);
    ask.exec();

    if (ask.clickedButton() == wipeBtn) {
        // Run parted to create a single ext4 on whole disk
        const QString cmd = QString(
            "parted %1 --script mklabel gpt mkpart primary ext4 1MiB 100%% && "
            "mkfs.ext4 -F %1p1"
        ).arg(disks[selectedDiskIdx].devicePath);
        QProcess::execute("bash", QStringList{"-c", cmd});
        emit continueClicked(disks[selectedDiskIdx].devicePath + "p1");
    }
    else if (ask.clickedButton() == alongBtn) {
        // Prompt for size
        QString sizeStr = promptSizeGiB("Partition size (GiB)", 10.0);
        if (sizeStr.isEmpty()) return;
        // Create partition at end of disk
        const QString disk = disks[selectedDiskIdx].devicePath;
        QString cmd = QString(
            "parted %1 --script mkpart primary ext4 100%%-%2GiB 100%% && "
            "mkfs.ext4 -F %1p$(lsblk -dln -o NAME %1 | wc -l)"
        ).arg(disk, sizeStr);
        QProcess::execute("bash", QStringList{"-c", cmd});
        // Note: we assume new partition is the last one; adjust as needed
        emit continueClicked(disk + "p" + QString::number(QProcess::execute(
            "bash", QStringList{"-c",
                QString("lsblk -dln -o NAME %1 | wc -l").arg(disk)
            }
        )));
    }
}

void DiskPage::onManualPartition() {
    QMessageBox::information(this, "Manual Partition",
        "Launching manual partition editor...\n\n"
        "Please use your preferred tool (e.g., KDE Partition Manager),\n"
        "then return here and click Continue.");
    // Optionally, you could launch kpartx or kde-partitionmanager:
    QProcess::startDetached("kdesu", QStringList{"partitionmanager"});
    // After user closes the external tool, they click Continue again:
    // We’ll assume they then pick a partition from a simple input:
    QString part = QInputDialog::getText(
        this, "Select Partition",
        "Enter the partition device to install on (e.g. /dev/sda3):"
    );
    if (!part.isEmpty())
        emit continueClicked(part);
}

QString DiskPage::promptSizeGiB(const QString &title, double minGiB) {
    bool ok = false;
    double val = QInputDialog::getDouble(
        this, title, "Size (GiB):", minGiB, minGiB, 10000.0, 1, &ok
    );
    return ok ? QString::number(val) : QString();
}
