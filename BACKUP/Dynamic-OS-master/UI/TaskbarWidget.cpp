#include "TaskbarWidget.h"
#include <QProcess>
#include <QFile>
#include <QIODevice>
#include <QDebug>

TaskbarWidget::TaskbarWidget(QWidget* parent)
: QWidget(parent)
{
    layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(5);

    leftArrow = new QToolButton(this);
    leftArrow->setText("◀");
    leftArrow->setVisible(false);
    connect(leftArrow, &QToolButton::clicked, this, &TaskbarWidget::onLeftArrow);

    rightArrow = new QToolButton(this);
    rightArrow->setText("▶");
    rightArrow->setVisible(false);
    connect(rightArrow, &QToolButton::clicked, this, &TaskbarWidget::onRightArrow);

    layout->addWidget(leftArrow);
    layout->addWidget(rightArrow);

    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &TaskbarWidget::updateTaskbarApps);
    updateTimer->start(2000);

    updateTaskbarApps();
}

void TaskbarWidget::updateTaskbarApps()
{
    QProcess proc;
    proc.start("wmctrl -lp");
    proc.waitForFinished();
    QString output = proc.readAllStandardOutput();

    QList<UserAppInfo> foundApps;
    QStringList lines = output.split('\n');
    for (const QString& line : lines) {
        QStringList parts = line.split(QRegExp("\\s+"), Qt::SkipEmptyParts);
        if (parts.size() < 5) continue;
        QString winId = parts[0];
        QString pid = parts[2];
        QString title = parts.mid(4).join(" ");
        if (title.trimmed().isEmpty()) continue;
        if (title.contains("Desktop", Qt::CaseInsensitive)) continue;

        QString procName;
        QFile commFile(QString("/proc/%1/comm").arg(pid));
        if (commFile.open(QIODevice::ReadOnly | QIODevice::Text))
            procName = commFile.readLine().trimmed();

        // Filter out desktop/shell/system windows
        if (procName.isEmpty()) continue;
        if (procName == "gnome-shell" || procName == "plasmashell" || procName == "ksmserver") continue;

        UserAppInfo info;
        info.windowTitle = title;
        info.processName = procName;
        info.windowId = winId;
        foundApps.append(info);
    }

    userApps = foundApps;
    refreshTaskbar();
}

void TaskbarWidget::refreshTaskbar()
{
    // Remove old app buttons (keep arrows)
    while (layout->count() > 2) {
        QLayoutItem* item = layout->takeAt(1);
        if (item->widget()) delete item->widget();
        delete item;
    }

    int maxVisible = 4;
    int total = userApps.size();
    if (offset > total - maxVisible)
        offset = qMax(0, total - maxVisible);

    leftArrow->setVisible(offset > 0);
    rightArrow->setVisible(total > maxVisible && (offset + maxVisible < total));

    for (int i = offset; i < qMin(offset + maxVisible, total); ++i) {
        const UserAppInfo& app = userApps[i];
        QToolButton* btn = new QToolButton(this);
        btn->setText(app.windowTitle.left(24));
        btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        btn->setFixedHeight(32);
        btn->setAutoRaise(true);
        btn->setCheckable(false);
        btn->setProperty("windowId", app.windowId);
        connect(btn, &QToolButton::clicked, this, &TaskbarWidget::onAppButtonClicked);
        layout->insertWidget(layout->count() - 1, btn);
    }
}

void TaskbarWidget::onAppButtonClicked()
{
    QToolButton* btn = qobject_cast<QToolButton*>(sender());
    if (!btn) return;
    QString winId = btn->property("windowId").toString();
    QProcess::startDetached(QString("wmctrl -ia %1").arg(winId));
}

void TaskbarWidget::onLeftArrow()
{
    if (offset > 0) {
        offset--;
        refreshTaskbar();
    }
}

void TaskbarWidget::onRightArrow()
{
    if (offset + 4 < userApps.size()) {
        offset++;
        refreshTaskbar();
    }
}
