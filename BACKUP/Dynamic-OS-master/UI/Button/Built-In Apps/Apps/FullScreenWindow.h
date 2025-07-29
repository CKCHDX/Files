#ifndef FULLSCREENWINDOW_H
#define FULLSCREENWINDOW_H

#include <QWidget>
#include <QWebEngineView>

class FullScreenWindow : public QWidget
{
    Q_OBJECT
public:
    explicit FullScreenWindow(QWebEngineView* view, QWidget* parent = nullptr);
    ~FullScreenWindow();

protected:
    void keyPressEvent(QKeyEvent* event) override;

private:
    QWebEngineView* m_view;
    QWidget* m_oldParent;
    QRect m_oldGeometry;
};

#endif // FULLSCREENWINDOW_H
