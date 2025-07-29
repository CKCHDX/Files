// File Manager.cpp
#include "File Manager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QInputDialog>
#include <QMessageBox>
#include <QDir>
#include <QDesktopServices>
#include <QUrl>
#include <QProcess>
#include <QTime>
#include <QClipboard>
#include <QStyleOption>
#include <QPainter>
#include <QApplication>
#include <QProcess>
#include <QStandardPaths>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>


FileManager::FileManager(QWidget *parent)
: QWidget(parent)
{
    // ensure proper background & stacking under bare X11
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_NoSystemBackground,   false);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_AlwaysStackOnTop);

    homeDir    = QDir::homePath();
    currentDir = homeDir;

    pathLabel  = new QLabel(this);
    pathLabel->setStyleSheet("font-weight: bold; color: white;");

    upButton   = new QPushButton("Up", this);
    searchEdit = new QLineEdit(this);
    searchEdit->setPlaceholderText("Search...");

    auto* topLayout = new QHBoxLayout;
    topLayout->setContentsMargins(10,10,10,0);
    topLayout->addWidget(upButton);
    topLayout->addWidget(pathLabel, 1);
    topLayout->addStretch();
    topLayout->addWidget(searchEdit);

    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("background: transparent; border: none;");

    gridWidget = new QWidget(scrollArea);
    scrollArea->setWidget(gridWidget);

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(scrollArea);
    setLayout(mainLayout);

    contextMenu = new QMenu(this);

    connect(upButton,   &QPushButton::clicked,   this, &FileManager::goUp);
    connect(searchEdit, &QLineEdit::textChanged, this, &FileManager::searchFiles);

    gridWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(gridWidget, &QWidget::customContextMenuRequested,
            this, &FileManager::showFolderContextMenu);

    setCurrentDir(homeDir);
}

void FileManager::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QWidget::paintEvent(event);
}

void FileManager::setCurrentDir(const QString& path)
{
    currentDir = path;
    searchEdit->clear();
    refresh();
}

void FileManager::refresh()
{
    if (auto old = gridWidget->layout()) {
        QLayoutItem* c;
        while ((c = old->takeAt(0))) {
            if (c->widget()) c->widget()->deleteLater();
            delete c;
        }
        delete old;
    }
    fileButtons.clear();

    auto* grid = new QGridLayout(gridWidget);
    grid->setContentsMargins(18,18,18,18);
    grid->setSpacing(18);

    QDir dir(currentDir);
    auto entries = dir.entryInfoList(
        QDir::NoDotAndDotDot | QDir::AllEntries,
        QDir::DirsFirst | QDir::Name
    );

    // filter by search text
    QString filter = searchEdit->text().trimmed();
    if (!filter.isEmpty()) {
        QFileInfoList tmp;
        for (auto& fi : entries)
            if (fi.fileName().contains(filter, Qt::CaseInsensitive))
                tmp << fi;
        entries = tmp;
    }

    int row = 0, col = 0, cols = 4;
    for (auto& fi : entries) {
        auto* btn = new QPushButton(fi.fileName(), gridWidget);
        btn->setMinimumSize(120,80);
        btn->setCheckable(true);
        btn->setContextMenuPolicy(Qt::CustomContextMenu);

        // style directories vs. files
        QString sheet = fi.isDir()
        ? R"(
                QPushButton {
                  border-radius:24px;
                  background:qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 #2d2d30,stop:1 #5858ff);
                  color:white;font-weight:bold;font-size:16px;margin:8px;
                }
                QPushButton:hover{background:#7ecb20;color:black;}
                QPushButton:pressed{background:#2222aa;color:#fff;}
              )"
              : R"(
                QPushButton {
                  border-radius:24px;
                  background:qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 #3c3c3c,stop:1 #888888);
                  color:white;font-size:15px;margin:8px;
                }
                QPushButton:hover{background:#ffa500;color:black;}
                QPushButton:pressed{background:#ff8800;color:#fff;}
              )";
        btn->setStyleSheet(sheet);

        grid->addWidget(btn, row, col);
        btn->installEventFilter(this);

        // detect double-click
        connect(btn, &QPushButton::clicked, [=]() {
            static QTime last;
            static QPushButton* prev = nullptr;
            auto now = QTime::currentTime();
            if (prev==btn && last.msecsTo(now) < QApplication::doubleClickInterval()) {
                if (fi.isDir()) enterFolder(fi.fileName());
                else             openFile(fi.fileName());
                prev = nullptr;
            } else {
                prev = btn; last = now;
            }
        });
        connect(btn, &QPushButton::customContextMenuRequested,
                this, &FileManager::showButtonContextMenu);

        fileButtons.append(btn);
        if (++col >= cols) { col = 0; ++row; }
    }

    gridWidget->setLayout(grid);
    pathLabel->setText(currentDir);
    clearSelection();
}

void FileManager::enterFolder(const QString& folderName)
{
    QString p = QDir(currentDir).filePath(folderName);
    if (QFileInfo(p).isDir()) setCurrentDir(p);
}

void FileManager::openFile(const QString& fileName)
{
    QString path = QDir(currentDir).filePath(fileName);
    if (!QProcess::startDetached("xdg-open", { path })) {
        QMessageBox::warning(this, "Open File", "Unable to open file.");
    }
}

void FileManager::goUp()
{
    QDir d(currentDir);
    if (!d.cdUp())
        return; // can't go up any further (e.g., at root "/")

        QString parent = d.absolutePath();

    // Allow going up only if parent is "/home" or inside "/home"
    if (parent == "/home" || parent.startsWith("/home/")) {
        setCurrentDir(parent);
    }
    // else do nothing, prevent going above "/home"
}


void FileManager::showButtonContextMenu(const QPoint& pos)
{
    contextMenu->clear();
    auto* btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    clearSelection();
    selectButton(btn);

    QString name = btn->text();
    QString full = QDir(currentDir).filePath(name);
    QFileInfo fi(full);

    if (fi.isFile())
        contextMenu->addAction("Open", [=]{ openFile(name); });
    contextMenu->addAction("Rename",  this, &FileManager::renameSelected);
    contextMenu->addAction("Delete", [=]{
        if (fi.isDir()) QDir(full).removeRecursively();
        else            QFile::remove(full);
        refresh();
    });
    contextMenu->addAction("Properties", this, &FileManager::showProperties);
    contextMenu->addSeparator();
    contextMenu->addAction("Copy",  this, &FileManager::copySelected);
    contextMenu->addAction("Cut",   this, &FileManager::cutSelected);
    contextMenu->addAction("Paste", this, &FileManager::pasteFiles);
    contextMenu->addSeparator();
    contextMenu->addAction("New File",   this, &FileManager::createNewFile);
    contextMenu->addAction("New Folder", this, &FileManager::createNewFolder);

    contextMenu->popup(btn->mapToGlobal(pos));
}

void FileManager::showFolderContextMenu(const QPoint& pos)
{
    contextMenu->clear();
    clearSelection();
    contextMenu->addAction("Paste",     this, &FileManager::pasteFiles);
    contextMenu->addSeparator();
    contextMenu->addAction("New File",   this, &FileManager::createNewFile);
    contextMenu->addAction("New Folder", this, &FileManager::createNewFolder);
    contextMenu->popup(gridWidget->mapToGlobal(pos));
}

void FileManager::createNewFile()
{
    bool ok;
    auto fn = QInputDialog::getText(this, "New File",
                                    "File name:", QLineEdit::Normal,
                                    "newfile.txt", &ok);
    if (ok && !fn.isEmpty()) {
        QFile f(QDir(currentDir).filePath(fn));
        f.open(QIODevice::WriteOnly);
        f.close();
        refresh();
    }
}

void FileManager::createNewFolder()
{
    bool ok;
    auto fn = QInputDialog::getText(this, "New Folder",
                                    "Folder name:", QLineEdit::Normal,
                                    "New Folder", &ok);
    if (ok && !fn.isEmpty()) {
        QDir().mkdir(QDir(currentDir).filePath(fn));
        refresh();
    }
}

void FileManager::renameSelected()
{
    if (selectedFiles.isEmpty()) return;
    auto old = selectedFiles.first();
    bool ok;
    auto n   = QInputDialog::getText(this, "Rename",
                                     "New name:", QLineEdit::Normal,
                                     old, &ok);
    if (ok && !n.isEmpty() && n != old) {
        QFile::rename(QDir(currentDir).filePath(old),
                      QDir(currentDir).filePath(n));
        refresh();
    }
}

QString FileManager::permissionsToString(QFile::Permissions p) const
{
    QString s;
    s += (p & QFile::ReadOwner)  ? "r":"-";
    s += (p & QFile::WriteOwner) ? "w":"-";
    s += (p & QFile::ExeOwner)   ? "x":"-";
    s += (p & QFile::ReadGroup)  ? "r":"-";
    s += (p & QFile::WriteGroup) ? "w":"-";
    s += (p & QFile::ExeGroup)   ? "x":"-";
    s += (p & QFile::ReadOther)  ? "r":"-";
    s += (p & QFile::WriteOther) ? "w":"-";
    s += (p & QFile::ExeOther)   ? "x":"-";
    return s;
}

void FileManager::showProperties()
{
    if (selectedFiles.isEmpty()) return;
    auto name = selectedFiles.first();
    auto full = QDir(currentDir).filePath(name);
    QFileInfo fi(full);

    QString det;
    det += QString("Name: %1\n").arg(fi.fileName());
    det += QString("Path: %1\n").arg(fi.absoluteFilePath());
    det += QString("Type: %1\n").arg(fi.isDir() ? "Folder" : "File");
    det += QString("Size: %1 bytes\n").arg(fi.size());
    det += QString("Modified: %1\n").arg(fi.lastModified().toString());
    det += QString("Permissions: %1\n").arg(permissionsToString(fi.permissions()));

    QMessageBox::information(this, "Properties", det);
}

void FileManager::searchFiles() { refresh(); }

void FileManager::copySelected()
{
    clipboardFiles  = selectedFiles;
    clipboardAction = FileAction::Copy;
}

void FileManager::cutSelected()
{
    clipboardFiles  = selectedFiles;
    clipboardAction = FileAction::Cut;
}

void FileManager::pasteFiles()
{
    for (auto& name : clipboardFiles) {
        QString src = QDir(currentDir).filePath(name);
        QString dst = QDir(currentDir).filePath(name);
        if (clipboardAction == FileAction::Copy)
            QFile::copy(src, dst);
        else if (clipboardAction == FileAction::Cut)
            QFile::rename(src, dst);
    }
    clipboardFiles.clear();
    clipboardAction = FileAction::None;
    refresh();
}

void FileManager::clearSelection()
{
    selectedFiles.clear();
    for (auto* b : fileButtons) b->setChecked(false);
}

void FileManager::selectButton(QPushButton* btn, bool multi)
{
    if (!btn) return;
    if (!multi) selectedFiles.clear();
    selectedFiles.append(btn->text());
    btn->setChecked(true);
}

void FileManager::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Backspace) goUp();
    else QWidget::keyPressEvent(e);
}

bool FileManager::eventFilter(QObject* obj, QEvent* ev)
{
    if (ev->type() == QEvent::MouseButtonPress) {
        if (auto* b = qobject_cast<QPushButton*>(obj)) {
            clearSelection();
            selectButton(b);
        }
    }
    return QWidget::eventFilter(obj, ev);
}

void FileManager::contextMenuEvent(QContextMenuEvent* ev)
{
    showFolderContextMenu(ev->pos());
}

void FileManager::mountUsbIfNeeded()
{
    QStringList devices;

    // Use lsblk to find devices with "disk" type and partitions starting with sda
    QProcess lsblk;
    lsblk.start("lsblk", {"-o", "NAME,MOUNTPOINT,TYPE", "-J"});
    lsblk.waitForFinished();
    QByteArray output = lsblk.readAllStandardOutput();

    // Parse lsblk JSON output (requires Qt 5.14+ for QJsonDocument)
    QJsonDocument doc = QJsonDocument::fromJson(output);
    if (!doc.isObject()) return;
    QJsonObject rootObj = doc.object();
    QJsonArray blockdevices = rootObj["blockdevices"].toArray();

    for (auto deviceVal : blockdevices) {
        QJsonObject deviceObj = deviceVal.toObject();
        QString name = deviceObj["name"].toString();

        // Example check for sda1, sdb1, etc.
        if (name.startsWith("sd") && deviceObj["type"].toString() == "part") {
            QString mountpoint = deviceObj["mountpoint"].toString();
            if (mountpoint.isEmpty()) {
                // Not mounted - mount it
                QString devPath = "/dev/" + name;

                // Target mount path: /home/$USER/usb-$name
                QString homePath = QDir::homePath();
                QString targetDir = homePath + "/usb-" + name;

                QDir dir(targetDir);
                if (!dir.exists()) {
                    QDir().mkdir(targetDir);
                }

                // Mount using sudo or polkit is needed - example uses system call (adjust for your environment)
                QString command = QString("udisksctl mount --no-user-interaction -b %1").arg(devPath);
                // You can replace this with direct mount call, but this needs root privileges:
                // sudo mount %1 %2

                // Run command
                QProcess::execute(command);

                // Optionally, verify mounted and update UI accordingly
            }
        }
    }
}
