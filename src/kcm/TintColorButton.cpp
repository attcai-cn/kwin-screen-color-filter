#include "TintColorButton.h"

#include <QByteArray>
#include <QColorDialog>
#include <QStyle>

TintColorButton::TintColorButton(QWidget *parent)
    : QPushButton(parent)
{
    setObjectName(QStringLiteral("TintColorButton"));
    // KConfigDialogManager binds every kcfg_* widget through its USER property.
    // For a QPushButton subclass that is the inherited QAbstractButton::checked
    // (bool), so the KCM read a bool from the button, qvariant_cast it to QColor
    // (invalid) and wrote tintColor=invalid into kwinrc - the effect then fell
    // back to the default tint and the picked color was never applied.
    // Point the manager at our QColor property instead.
    setProperty("kcfg_property", QByteArrayLiteral("color"));
    setFixedWidth(96);
    connect(this, &QPushButton::clicked, this, &TintColorButton::pickColor);
    setColor(QColor(QStringLiteral("#ffad66")));
}

void TintColorButton::setColor(const QColor &color)
{
    if (!color.isValid() || m_color == color)
        return;
    m_color = color;
    updateAppearance();
    Q_EMIT colorChanged(m_color);
}

void TintColorButton::updateAppearance()
{
    const QString style = QStringLiteral(
        "QPushButton { background-color: %1; border: 1px solid %2; border-radius: 3px; }"
        "QPushButton:hover { border: 1px solid palette(highlight); }");
    setStyleSheet(style.arg(m_color.name(),
                             palette().color(QPalette::Window).lightness() < 128
                                 ? QStringLiteral("#666666")
                                 : QStringLiteral("#999999")));
    setToolTip(tr("Click to pick the tint color (current: %1)").arg(m_color.name()));
}

void TintColorButton::pickColor()
{
    const QColor c = QColorDialog::getColor(m_color, this, tr("Select filter color"),
                                            QColorDialog::ShowAlphaChannel);
    if (c.isValid())
        setColor(c);
}
