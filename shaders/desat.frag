uniform float saturation;
uniform float vcenter;
uniform float vpercent;

// All components are in the range [0…1], including hue.
vec3 rgb2hsv(vec3 c) {
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

// All components are in the range [0…1], including hue.
vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

float scale_to(float f, float target, float percent) {
    return ((f - target) * percent) + target;
}

vec4 desat_apply(vec4 color) {
    vec4 tex_color = texture2D(texture, gl_TexCoord[0].xy);
    vec3 hsv = rgb2hsv(vec3(tex_color));
    hsv.y = hsv.y * saturation;
    hsv.z = ((hsv.z - vcenter) * vpercent) + vcenter;
    vec3 rgb = hsv2rgb(hsv);
    vec4 result = vec4(rgb * tex_color.a, tex_color.a); // premultiplied alpha
    return result;
}
