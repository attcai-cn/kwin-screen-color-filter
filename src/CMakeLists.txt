add_subdirectory(kcm)

set(effect_SRCS
    ScreenColorFilterEffect.h
    ScreenColorFilterEffect.cpp
    ScreenColorFilterShader.h
    ScreenColorFilterShader.cpp
    plugin.cpp
)

kconfig_add_kcfg_files(effect_SRCS ScreenColorFilterConfig.kcfgc)

kcoreaddons_add_plugin(kwin4_effect_screencolorfilter SOURCES ${effect_SRCS} INSTALL_NAMESPACE "kwin/effects/plugins/")

target_link_libraries(kwin4_effect_screencolorfilter
    Qt${QT_MAJOR_VERSION}::DBus
    KF${QT_MAJOR_VERSION}::ConfigGui
    KF${QT_MAJOR_VERSION}::GlobalAccel
    KF${QT_MAJOR_VERSION}::I18n
)
if(${QT_MAJOR_VERSION} EQUAL 6)
    target_link_libraries(kwin4_effect_screencolorfilter
            KWin::kwin
    )
else()
    target_link_libraries(kwin4_effect_screencolorfilter
            KWinEffects::kwineffects
            KWinEffects::kwinglutils
    )
endif()

# KWin resolves custom fragment shaders by appending "_core" to the basename
# on core-profile GL contexts, so install both names with identical content.
install(FILES shaders/Tint.frag DESTINATION ${KDE_INSTALL_DATADIR}/kwin/shaders)
install(FILES shaders/Tint.frag DESTINATION ${KDE_INSTALL_DATADIR}/kwin/shaders RENAME Tint_core.frag)
