#ifndef ORB_H
#define ORB_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QPushButton>

class Orb : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal morphProgress READ morphProgress WRITE setMorphProgress)

public:
    explicit Orb(QWidget* parent = nullptr);
    qreal morphProgress() const { return m_progress; }
    void setMorphProgress(qreal);

    void reset();
    void openHub();
signals:
    void appRequested(const QString& app);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;

private:
    enum State { Closed, Opening, Open, Closing } m_state;
    qreal m_progress;
    QPropertyAnimation* m_anim;
    QWidget* m_panel;
    QGraphicsOpacityEffect* m_panelOpacity;

    QRect closedRect() const;
    QRect openRect() const;
    bool isClickOnCenter(const QPoint& pos) const;

    void closeHub();
    void setupPanel();
    void updatePanel();
};

#endif // ORB_H
