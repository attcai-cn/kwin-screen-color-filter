/*
 * Minimal color picker button (QColor property compatible with KConfigXT),
 * used because KF6 KColorWidgets is not packaged on all distros (e.g. Debian).
 */
#pragma once

#include <QColor>
#include <QPushButton>

class TintColorButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
public:
    explicit TintColorButton(QWidget *parent = nullptr);

    [[nodiscard]] QColor color() const { return m_color; }
    void setColor(const QColor &color);

Q_SIGNALS:
    void colorChanged(const QColor &color);

private:
    void updateAppearance();
    void pickColor();

    QColor m_color;
};
