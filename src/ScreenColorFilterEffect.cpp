/*
 * Screen Color Filter effect implementation.
 *
 * Follows the architecture of KWin's built-in colorblindnesscorrection effect:
 * redirect windows + setShader, no drawWindow override, uniforms set once via
 * ShaderBinder when config changes.
 */
#include "ScreenColorFilterEffect.h"
#include "ScreenColorFilterConfig.h"

#include <QAction>
#include <KConfigGroup>
#include <KGlobalAccel>
#include <KLocalizedString>
#include <KSharedConfig>

#if QT_VERSION_MAJOR >= 6
    #include <opengl/glutils.h>
#else
    #include <kwinglutils.h>
#endif

#include <algorithm>

ScreenColorFilterEffect::ScreenColorFilterEffect()
    : KWin::OffscreenEffect()
{
    setupShortcuts();
    loadData();
    connect(KWin::effects, &KWin::EffectsHandler::windowAdded, this, &ScreenColorFilterEffect::windowAdded);
    connect(KWin::effects, &KWin::EffectsHandler::windowDeleted, this, &ScreenColorFilterEffect::windowDeleted);
}

ScreenColorFilterEffect::~ScreenColorFilterEffect()
{
    clearWindows();
}

bool ScreenColorFilterEffect::supported()
{
    return KWin::effects->isOpenGLCompositing();
}

void ScreenColorFilterEffect::loadData()
{
    // Force re-read from disk: KWin caches kwinrc, and the KCM saves from
    // another process, so the singleton may hold stale values otherwise.
    if (KConfig *cfg = ScreenColorFilterConfig::self()->config()) {
        cfg->reparseConfiguration();
    }
    ScreenColorFilterConfig::self()->read();

    // Guard against "invalid" written by older builds: keep a valid color in
    // the skeleton so any subsequent save() never writes "invalid" back.
    if (!ScreenColorFilterConfig::tintColor().isValid()) {
        ScreenColorFilterConfig::setTintColor(QColor(QStringLiteral("#ffad66")));
    }

    if (!m_shaderManager.IsValid()) {
        return;
    }

    updateUniforms();

    if (m_filterEnabled) {
        redirectAll();
    }

    KWin::effects->addRepaintFull();
}

void ScreenColorFilterEffect::updateUniforms()
{
    if (!m_shaderManager.IsValid()) {
        return;
    }
    // OpenGL program uniforms persist; set once here, OffscreenEffect::paint()
    // binds the same GLShader via ShaderBinder each frame.
    m_shaderManager.ApplyConfig();
}

void ScreenColorFilterEffect::clearWindows()
{
    for (KWin::EffectWindow *w : m_windows) {
        unredirect(w);
    }
    m_windows.clear();
}

void ScreenColorFilterEffect::redirectAll()
{
    if (!m_shaderManager.IsValid() || !m_filterEnabled) {
        return;
    }
    for (KWin::EffectWindow *w : KWin::effects->stackingOrder()) {
        if (!m_windows.contains(w)) {
            redirect(w);
            setShader(w, m_shaderManager.GetShader().get());
            m_windows.insert(w);
        }
    }
}

void ScreenColorFilterEffect::unredirectAll()
{
    for (KWin::EffectWindow *w : m_windows) {
        unredirect(w);
    }
    m_windows.clear();
}

void ScreenColorFilterEffect::windowAdded(KWin::EffectWindow *window)
{
    if (!m_filterEnabled || !m_shaderManager.IsValid() || !window) {
        return;
    }
    if (m_windows.contains(window)) {
        return;
    }
    redirect(window);
    setShader(window, m_shaderManager.GetShader().get());
    m_windows.insert(window);
}

void ScreenColorFilterEffect::windowDeleted(KWin::EffectWindow *window)
{
    m_windows.erase(window);
}

void ScreenColorFilterEffect::setupShortcuts()
{
    auto *toggle = new QAction(this);
    toggle->setObjectName(QStringLiteral("ScreenColorFilterToggle"));
    toggle->setText(i18n("Toggle Screen Color Filter"));
    KGlobalAccel::self()->setDefaultShortcut(toggle, {Qt::MetaModifier | Qt::Key_F});
    KGlobalAccel::self()->setShortcut(toggle, {Qt::MetaModifier | Qt::Key_F});
    connect(toggle, &QAction::triggered, this, &ScreenColorFilterEffect::toggleEnabled);

    auto *increase = new QAction(this);
    increase->setObjectName(QStringLiteral("ScreenColorFilterIncrease"));
    increase->setText(i18n("Increase Color Filter Intensity"));
    KGlobalAccel::self()->setDefaultShortcut(increase, {Qt::MetaModifier | Qt::Key_Plus});
    KGlobalAccel::self()->setShortcut(increase, {Qt::MetaModifier | Qt::Key_Plus});
    connect(increase, &QAction::triggered, this, &ScreenColorFilterEffect::increaseIntensity);

    auto *decrease = new QAction(this);
    decrease->setObjectName(QStringLiteral("ScreenColorFilterDecrease"));
    decrease->setText(i18n("Decrease Color Filter Intensity"));
    KGlobalAccel::self()->setDefaultShortcut(decrease, {Qt::MetaModifier | Qt::Key_Minus});
    KGlobalAccel::self()->setShortcut(decrease, {Qt::MetaModifier | Qt::Key_Minus});
    connect(decrease, &QAction::triggered, this, &ScreenColorFilterEffect::decreaseIntensity);
}

void ScreenColorFilterEffect::reconfigure(ReconfigureFlags flags)
{
    Q_UNUSED(flags)
    loadData();
}

void ScreenColorFilterEffect::toggleEnabled()
{
    m_filterEnabled = !m_filterEnabled;
    if (m_filterEnabled) {
        redirectAll();
    } else {
        unredirectAll();
    }
    KWin::effects->addRepaintFull();
}

void ScreenColorFilterEffect::increaseIntensity()
{
    qreal v = ScreenColorFilterConfig::intensity() + 0.05;
    v = std::min(v, 1.0);
    // Write only the intensity key so a stale/invalid color in the skeleton
    // is never flushed back to kwinrc.
    KConfigGroup group(KSharedConfig::openConfig(QStringLiteral("kwinrc")),
                       QStringLiteral("Effect-ScreenColorFilter"));
    group.writeEntry(QStringLiteral("intensity"), v);
    group.sync();
    loadData();
}

void ScreenColorFilterEffect::decreaseIntensity()
{
    qreal v = ScreenColorFilterConfig::intensity() - 0.05;
    v = std::max(v, 0.0);
    KConfigGroup group(KSharedConfig::openConfig(QStringLiteral("kwinrc")),
                       QStringLiteral("Effect-ScreenColorFilter"));
    group.writeEntry(QStringLiteral("intensity"), v);
    group.sync();
    loadData();
}
