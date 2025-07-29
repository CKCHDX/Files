#include <QApplication>
#include <QSplashScreen>
#include <QTimer>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Create a splash screen with a message
    QSplashScreen splash;
    splash.setPixmap(QPixmap()); // Optionally set an image
    splash.showMessage("Starting Oscyra...", Qt::AlignCenter | Qt::AlignBottom, Qt::white);
    splash.show();

    // Simulate some loading time if needed (optional)
    // QTimer::singleShot(2000, &splash, &QSplashScreen::close);

    MainWindow mainWindow;
    mainWindow.show(); // or showFullScreen()

    // Close the splash screen after the main window is shown
    splash.finish(&mainWindow);

    return app.exec();
}
