uniform float alpha_multiplier;
uniform sampler2D texture;

%INCLUDE%

void main() {
    vec4 color = texture2D(texture, gl_TexCoord[0].xy);

    %APPLY%

    float alpha = color.a * alpha_multiplier;
    color = vec4(color.rgb, alpha);
    gl_FragColor = color;
}
