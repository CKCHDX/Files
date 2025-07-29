// CustomAppWindow.h
#ifndef CUSTOMAPPWINDOW_H
#define CUSTOMAPPWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QSizeGrip>

class CustomAppWindow : public QWidget
{
    Q_OBJECT

public:
    explicit CustomAppWindow(QWidget* centralWidget, const QString& title, QWidget* parent = nullptr);

protected:
    void resizeEvent(QResizeEvent* ev) override;
    void keyPressEvent(QKeyEvent* ev) override;

private slots:
    void onCloseClicked();
    void onMaximizeClicked();

private:
    void setupTitleBar(const QString& title);
    void updateSizeGrips();

    QWidget*     m_titleBar    = nullptr;
    QLabel*      m_titleLabel  = nullptr;
    QPushButton* m_closeBtn    = nullptr;
    QPushButton* m_maxBtn      = nullptr;
    QSizeGrip*   m_gripBR      = nullptr;
    QSizeGrip*   m_gripBL      = nullptr;

    const int    m_titleBarH   = 36;
};

#endif // CUSTOMAPPWINDOW_H
