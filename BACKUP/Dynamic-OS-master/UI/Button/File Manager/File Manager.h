// File Manager.h
#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <QMenu>
#include <QLineEdit>
#include <QFile>
#include <QFileInfo>
#include <QList>
#include <QString>
#include <QKeyEvent>
#include <QContextMenuEvent>
#include <QPaintEvent>

enum class FileAction { None, Copy, Cut };

class FileManager : public QWidget
{
    Q_OBJECT

public:
    explicit FileManager(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private slots:
    void enterFolder(const QString& folderName);
    void openFile(const QString& fileName);
    void showButtonContextMenu(const QPoint &pos);
    void showFolderContextMenu(const QPoint &pos);
    void createNewFile();
    void createNewFolder();
    void renameSelected();
    void showProperties();
    void searchFiles();
    void goUp();
    void copySelected();
    void cutSelected();
    void pasteFiles();
    void mountUsbIfNeeded();
    void refresh();

private:
    void setCurrentDir(const QString& path);
    void clearSelection();
    void selectButton(QPushButton* btn, bool multi = false);
    QString permissionsToString(QFile::Permissions perms) const;

    QLabel*              pathLabel;
    QPushButton*         upButton;
    QLineEdit*           searchEdit;
    QScrollArea*         scrollArea;
    QWidget*             gridWidget;
    QMenu*               contextMenu;
    QString              currentDir;
    QString              homeDir;
    QList<QPushButton*>  fileButtons;
    QList<QString>       selectedFiles;
    QList<QString>       clipboardFiles;
    FileAction           clipboardAction = FileAction::None;
};

#endif // FILE_MANAGER_H
