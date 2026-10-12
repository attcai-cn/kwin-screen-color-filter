#include "ScreenColorFilterKCM.h"
#include "ui_ScreenColorFilterKCM.h"
#include "kwineffects_interface.h"
#include "ScreenColorFilterConfig.h"

#include <QColor>

static const QColor kDefaultTintColor(QStringLiteral("#ffad66"));

#if (QT_VERSION_MAJOR >= 6)
ScreenColorFilterKCM::ScreenColorFilterKCM(QObject* parent, const KPluginMetaData& args)
    : KCModule(parent, args)
    , ui(new Ui::Form)
{
    ui->setupUi(widget());
    addConfig(ScreenColorFilterConfig::self(), widget());
}
#else
ScreenColorFilterKCM::ScreenColorFilterKCM(QWidget* parent, const QVariantList& args)
    : KCModule(parent, args)
    , ui(new Ui::Form)
{
    ui->setupUi(this);
    addConfig(ScreenColorFilterConfig::self(), this);
}
#endif

void ScreenColorFilterKCM::save()
{
    // KCModule::save() lets the KConfigDialogManager sync every kcfg_ widget
    // into the skeleton and write kwinrc. The color button is bound to its
    // QColor property via kcfg_property, so tintColor is stored correctly.
    KCModule::save();

    // Safety net: never let an invalid color reach kwinrc - the effect would
    // discard it and keep rendering the default tint.
    QColor tint = ui->kcfg_tintColor->color();
    if (!tint.isValid()) {
        tint = kDefaultTintColor;
    }
    if (ScreenColorFilterConfig::tintColor() != tint) {
        ScreenColorFilterConfig::setTintColor(tint);
        ScreenColorFilterConfig::self()->save();
    }

    // Reload the effect so it re-parses kwinrc and applies the new look.
    OrgKdeKwinEffectsInterface interface(QStringLiteral("org.kde.KWin"),
                                         QStringLiteral("/Effects"),
                                         QDBusConnection::sessionBus());
    interface.unloadEffect(QStringLiteral("kwin4_effect_screencolorfilter"));
    interface.loadEffect(QStringLiteral("kwin4_effect_screencolorfilter"));
}

void ScreenColorFilterKCM::load()
{
    KCModule::load();
    ScreenColorFilterConfig::self()->load();

    QColor tint = ScreenColorFilterConfig::tintColor();
    if (!tint.isValid()) {
        tint = kDefaultTintColor;
    }
    ui->kcfg_tintColor->setColor(tint);
}

void ScreenColorFilterKCM::defaults()
{
    KCModule::defaults();
    ScreenColorFilterConfig::self()->setDefaults();
    ui->kcfg_tintColor->setColor(kDefaultTintColor);
}
