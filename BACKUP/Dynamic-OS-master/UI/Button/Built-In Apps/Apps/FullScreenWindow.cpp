#include "FullScreenWindow.h"
#include <QVBoxLayout>
#include <QKeyEvent>

FullScreenWindow::FullScreenWindow(QWebEngineView* view, QWidget* parent)
: QWidget(parent), m_view(view)
{
    m_oldParent = m_view->parentWidget();
    m_oldGeometry = m_view->geometry();

    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_DeleteOnClose);

    QVBoxLayout* lay = new QVBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->addWidget(m_view);
    setLayout(lay);

    showFullScreen();
}

FullScreenWindow::~FullScreenWindow()
{
    if (m_view && m_oldParent) {
        m_view->setParent(m_oldParent);
        m_oldParent->layout()->addWidget(m_view);
        m_view->setGeometry(m_oldGeometry);
    }
}

void FullScreenWindow::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        close();
    } else {
        QWidget::keyPressEvent(event);
    }
}
