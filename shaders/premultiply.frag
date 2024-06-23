uniform bool textured;
uniform sampler2D src_texture;

void main() {
    vec4 src_color4;
    if (textured) {
        src_color4 = texture2D(src_texture, gl_TexCoord[0].xy);
    } else {
        src_color4 = gl_Color;
    }
    gl_FragColor = vec4(src_color4.rgb * src_color4.a, src_color4.a);
}
