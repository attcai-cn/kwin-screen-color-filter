/*
 * Shader wrapper for the Screen Color Filter effect.
 *
 * Holds the GLShader and applies config uniforms via ShaderBinder (same
 * pattern as KWin's colorblindnesscorrection effect).
 */
#pragma once

#include <qconfig.h>
#if QT_VERSION_MAJOR >= 6
    #include <effect/effect.h>
#else
    #include <kwineffects.h>
#endif

#include <memory>

namespace KWin {
    class GLShader;
}

class ScreenColorFilterShader {
public:
    explicit ScreenColorFilterShader();
    ~ScreenColorFilterShader();

    [[nodiscard]] bool IsValid() const;

    // Bind shader and set uniforms from config. Uniforms persist on the GL
    // program; OffscreenEffect::paint() re-binds the same shader each frame.
    void ApplyConfig() const;

    std::unique_ptr<KWin::GLShader>& GetShader() { return m_shader; }

private:
    std::unique_ptr<KWin::GLShader> m_shader;
};
