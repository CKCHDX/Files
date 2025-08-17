#include <QApplication>
#include <QPalette>
#include <QStyleFactory>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Use Fusion style for better dark theme support
    app.setStyle(QStyleFactory::create("Fusion"));

    // Custom dark palette
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(24, 26, 30));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(18, 18, 20));
    darkPalette.setColor(QPalette::AlternateBase, QColor(28, 28, 30));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(36, 38, 44));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);

    app.setPalette(darkPalette);

    MainWindow w;
    w.show();

    return app.exec();
}
