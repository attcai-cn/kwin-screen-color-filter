/*
 * Screen Color Filter fragment shader (KWin "MapTexture" trait, GLSL 140).
 *
 * Tints the whole screen with an adjustable color and intensity.
 *   blendMode == 0 : overlay  - mix original color toward tintColor by intensity
 *   blendMode == 1 : multiply - scale channels by mix(1, tintColor, intensity)
 *
 * Tint amount is scaled by alpha so shadow / transparent padding around a
 * window (rendered into the offscreen texture by OffscreenEffect) is not
 * tinted into a colored halo larger than the window.
 *
 * The KWin MapTexture vertex shader provides the "texcoord0" varying and the
 * "sampler" uniform used below.
 */
#version 140

uniform sampler2D sampler;
in vec2 texcoord0;

uniform vec3 tintColor;   // tint color, 0..1 RGB
uniform float intensity;  // 0..1
uniform int blendMode;    // 0 = overlay, 1 = multiply

out vec4 fragColor;

void main()
{
    vec4 c = texture(sampler, texcoord0);

    // Shadow / fully transparent padding: leave untouched (no colored halo).
    if (c.a <= 0.001) {
        fragColor = c;
        return;
    }

    // Scale tint by alpha so soft shadows keep their original dark tint
    // instead of being mixed toward the filter color.
    float amount = intensity * c.a;
    vec3 col = c.rgb;

    if (blendMode == 1) {
        // Multiply: at intensity 0 -> no change, at 1 -> full multiply by tintColor.
        vec3 factor = mix(vec3(1.0), tintColor, amount);
        col = col * factor;
    } else {
        // Overlay: blend the original color toward the tint color (sunglasses style).
        col = mix(col, tintColor, amount);
    }

    c.rgb = col;
    fragColor = c;
}
