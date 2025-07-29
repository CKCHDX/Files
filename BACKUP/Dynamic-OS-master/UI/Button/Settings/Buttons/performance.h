#ifndef PERFORMANCE_H
#define PERFORMANCE_H

#include <QWidget>
#include <QRadioButton>
#include <QLabel>
#include <QEvent>

class PerformanceWidget : public QWidget
{
    Q_OBJECT

public:
    enum PerformanceMode {
        LowPerformance,
        NormalPerformance,
        HighPerformance
    };
    Q_ENUM(PerformanceMode)

    explicit PerformanceWidget(QWidget *parent = nullptr);
    PerformanceMode currentMode() const;
    void setMode(PerformanceMode mode);

signals:
    void performanceModeChanged(PerformanceMode mode);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void onModeSelected();

private:
    PerformanceMode mode;
    QLabel *descriptionLabel;
    QRadioButton *lowPerformanceRadio;
    QRadioButton *normalPerformanceRadio;
    QRadioButton *highPerformanceRadio;
};

#endif // PERFORMANCE_H
