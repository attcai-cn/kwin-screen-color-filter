/*
 * Shader wrapper implementation for the Screen Color Filter effect.
 */
#include "ScreenColorFilterShader.h"
#include "ScreenColorFilterConfig.h"

#include <QStandardPaths>
#include <QVector3D>

#if QT_VERSION_MAJOR >= 6
    #include <opengl/glutils.h>
#else
    #include <kwinglutils.h>
#endif

ScreenColorFilterShader::~ScreenColorFilterShader() = default;

ScreenColorFilterShader::ScreenColorFilterShader()
{
    const QString frag = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                                QStringLiteral("kwin/shaders/Tint.frag"));
    if (frag.isEmpty()) {
        qCritical() << "ScreenColorFilter: Tint.frag shader not found!";
        return;
    }

    m_shader = KWin::ShaderManager::instance()->generateShaderFromFile(
        KWin::ShaderTrait::MapTexture, QString(), frag);
    if (!m_shader || !m_shader->isValid()) {
        qCritical() << "ScreenColorFilter: invalid shader";
        m_shader.reset();
        return;
    }
}

bool ScreenColorFilterShader::IsValid() const
{
    return m_shader && m_shader->isValid();
}

void ScreenColorFilterShader::ApplyConfig() const
{
    if (!IsValid()) {
        return;
    }

    QColor c = ScreenColorFilterConfig::tintColor();
    // kwinrc may contain "invalid" if an older build wrote a broken color;
    // fall back to the warm amber default instead of rendering black.
    if (!c.isValid()) {
        c = QColor(QStringLiteral("#ffad66"));
    }
    const float intensity = static_cast<float>(ScreenColorFilterConfig::intensity());
    const int mode = ScreenColorFilterConfig::blendMode();

    KWin::ShaderBinder binder(m_shader.get());
    m_shader->setUniform("tintColor", QVector3D(static_cast<float>(c.redF()),
                                                 static_cast<float>(c.greenF()),
                                                 static_cast<float>(c.blueF())));
    m_shader->setUniform("intensity", intensity);
    m_shader->setUniform("blendMode", mode);
}
