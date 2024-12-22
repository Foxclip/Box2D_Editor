// #version 330 core

uniform float alpha_multiplier;

%INCLUDE%

void main() {
    vec4 color = vec4(0.0, 0.0, 0.0, 1.0);

    %APPLY%

    float alpha = color.a * alpha_multiplier;
    color = vec4(color.rgb, alpha);
    gl_FragColor = color;
}
