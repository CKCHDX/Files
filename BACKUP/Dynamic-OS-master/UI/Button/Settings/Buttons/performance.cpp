#include "performance.h"
#include <QVBoxLayout>
#include <QGroupBox>
#include <QFont>
#include <QHBoxLayout>
#include <QFrame>
#include <QButtonGroup>

PerformanceWidget::PerformanceWidget(QWidget *parent)
: QWidget(parent),
mode(HighPerformance)
{
    auto *mainLayout = new QVBoxLayout(this);

    auto *heading = new QLabel("Choose your system's performance mode:", this);
    QFont hf = heading->font();
    hf.setPointSize(12);
    hf.setBold(true);
    heading->setFont(hf);
    mainLayout->addWidget(heading);

    auto *group = new QGroupBox(this);
    auto *layout = new QVBoxLayout(group);
    auto *buttonGroup = new QButtonGroup(this);
    buttonGroup->setExclusive(true);

    struct Mode { QString icon, label, tooltip; PerformanceMode m; };
    const Mode modes[] = {
        {u8"🐢", "Low Performance",   "Disable most effects for max speed.", LowPerformance},
        {u8"⚖️", "Normal Performance","Balanced visuals & speed.",          NormalPerformance},
        {u8"🚀", "High Performance",  "Enable all effects & animations.",    HighPerformance}
    };

    for (const auto &modeInfo : modes) {
        auto *frame = new QFrame(this);
        frame->setObjectName("modeFrame");
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setStyleSheet(R"(
            #modeFrame {
                border-radius: 8px;
                border: 1px solid #ccc;
                padding: 8px;
            }
            #modeFrame:hover {
                border-color: #0078d7;
                background: #e6f0ff;
            }
        )");
        frame->installEventFilter(this);

        auto *hLayout = new QHBoxLayout(frame);
        auto *icon = new QLabel(modeInfo.icon, frame);
        icon->setFont(QFont("Segoe UI Emoji", 18));
        hLayout->addWidget(icon);

        auto *radio = new QRadioButton(modeInfo.label, frame);
        radio->setToolTip(modeInfo.tooltip);
        hLayout->addWidget(radio);
        buttonGroup->addButton(radio);
        hLayout->addStretch();

        layout->addWidget(frame);

        switch (modeInfo.m) {
            case LowPerformance:    lowPerformanceRadio = radio;    break;
            case NormalPerformance: normalPerformanceRadio = radio; break;
            case HighPerformance:   highPerformanceRadio = radio;   break;
        }
        connect(radio, &QRadioButton::toggled, this, &PerformanceWidget::onModeSelected);
    }

    group->setLayout(layout);
    mainLayout->addWidget(group);

    descriptionLabel = new QLabel(this);
    descriptionLabel->setWordWrap(true);
    QFont df = descriptionLabel->font();
    df.setItalic(true);
    descriptionLabel->setFont(df);
    mainLayout->addWidget(descriptionLabel);

    highPerformanceRadio->setChecked(true);
    descriptionLabel->setText("🚀 High Performance: All effects and animations enabled.");
}

void PerformanceWidget::onModeSelected()
{
    PerformanceMode newMode = mode;
    QString desc;

    if (lowPerformanceRadio->isChecked()) {
        newMode = LowPerformance;
        desc = "🐢 Low Performance: Effects disabled for maximum speed.";
    } else if (normalPerformanceRadio->isChecked()) {
        newMode = NormalPerformance;
        desc = "⚖️ Normal Performance: Balanced visuals & speed.";
    } else if (highPerformanceRadio->isChecked()) {
        newMode = HighPerformance;
        desc = "🚀 High Performance: All effects and animations enabled.";
    }

    descriptionLabel->setText(desc);
    if (newMode != mode) {
        mode = newMode;
        emit performanceModeChanged(mode);
    }
}

PerformanceWidget::PerformanceMode PerformanceWidget::currentMode() const
{
    return mode;
}

void PerformanceWidget::setMode(PerformanceMode newMode)
{
    if (mode != newMode) {
        mode = newMode;
        switch (mode) {
            case LowPerformance:    lowPerformanceRadio->setChecked(true);    break;
            case NormalPerformance: normalPerformanceRadio->setChecked(true); break;
            case HighPerformance:   highPerformanceRadio->setChecked(true);   break;
        }
        emit performanceModeChanged(mode);
    }
}

bool PerformanceWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonRelease) {
        auto *frame = qobject_cast<QFrame*>(obj);
        if (frame) {
            auto radios = frame->findChildren<QRadioButton*>();
            if (!radios.isEmpty()) {
                radios.first()->setChecked(true);
                return true;
            }
        }
    }
    return QWidget::eventFilter(obj, event);
}
