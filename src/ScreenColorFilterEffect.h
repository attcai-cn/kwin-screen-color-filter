/*
 * Screen Color Filter - a system-wide KWin effect that tints the whole screen
 * with an adjustable color and intensity, similar to f.lux / Windows color filters.
 *
 * Adapted from the kwin-colorcorrection-effect (GPL) by juanma1980 / KDE-Rounded-Corners.
 * Architecture follows KWin's built-in colorblindnesscorrection effect.
 */
#pragma once

#include "ScreenColorFilterShader.h"

#if QT_VERSION_MAJOR >= 6
    #include <effect/effecthandler.h>
    #include <effect/offscreeneffect.h>
#else
    #include <kwineffects.h>
    #include <kwinoffscreeneffect.h>
#endif

#include <set>

class ScreenColorFilterEffect : public KWin::OffscreenEffect
{
    Q_OBJECT
public:
    ScreenColorFilterEffect();
    ~ScreenColorFilterEffect() override;

    static bool supported();
    static bool enabledByDefault() { return false; }

    void reconfigure(ReconfigureFlags flags) override;

    [[nodiscard]] int requestedEffectChainPosition() const override { return 99; }
    [[nodiscard]] bool isActive() const override { return !m_windows.empty(); }

public Q_SLOTS:
    void toggleEnabled();
    void increaseIntensity();
    void decreaseIntensity();

private Q_SLOTS:
    void windowAdded(KWin::EffectWindow *window);
    void windowDeleted(KWin::EffectWindow *window);

private:
    void setupShortcuts();
    void loadData();
    void clearWindows();
    void redirectAll();
    void unredirectAll();
    void updateUniforms();

private:
    ScreenColorFilterShader m_shaderManager;
    std::set<KWin::EffectWindow *> m_windows;
    bool m_filterEnabled = true;
};
