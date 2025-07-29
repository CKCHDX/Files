#ifndef MANAGE_FUNCTION_APP_H
#define MANAGE_FUNCTION_APP_H

#include <QString>
#include <QList>

class ManageFunctionApp
{
public:
    ManageFunctionApp();
    QList<QString> getInstalledApps();
    void uninstallApp(const QString& appName);
    void refreshAppList();
};

#endif // MANAGE_FUNCTION_APP_H
