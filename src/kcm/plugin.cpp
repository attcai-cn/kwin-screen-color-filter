#include "ScreenColorFilterKCM.h"
#include <KPluginFactory>

K_PLUGIN_FACTORY_WITH_JSON(ScreenColorFilterConfigFactory,
                           "metadata.json",
                           registerPlugin<ScreenColorFilterKCM>();)

#include "plugin.moc"
