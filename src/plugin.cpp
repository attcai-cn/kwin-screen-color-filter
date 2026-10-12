#include "ScreenColorFilterEffect.h"

KWIN_EFFECT_FACTORY_SUPPORTED_ENABLED(ScreenColorFilterEffect, "metadata.json",
    return ScreenColorFilterEffect::supported();,
    return ScreenColorFilterEffect::enabledByDefault();)

#include "plugin.moc"
