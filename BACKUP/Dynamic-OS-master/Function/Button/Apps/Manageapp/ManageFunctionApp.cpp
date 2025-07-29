#include "ManageFunctionApp.h"

ManageFunctionApp::ManageFunctionApp() {}

QList<QString> ManageFunctionApp::getInstalledApps()
{
    return { "App1", "App2", "App3" }; // Mock list of installed apps
}

void ManageFunctionApp::uninstallApp(const QString& appName)
{
    // Simulate app uninstallation
}

void ManageFunctionApp::refreshAppList()
{
    // Simulate refreshing app list
}
