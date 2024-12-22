uniform int type;
uniform sampler2D src_texture;

void main() {
    vec4 src_color4;
    switch (type) {
        case 0: src_color4 = gl_Color; break; // non-textured
        case 1: src_color4 = texture2D(src_texture, gl_TexCoord[0].xy); break; // textured
        case 2: src_color4 = gl_Color * texture2D(src_texture, gl_TexCoord[0].xy); break; // textured and using vertex color
    }
    float alpha = src_color4.a;
    gl_FragColor = vec4(src_color4.rgb * alpha, alpha);
}
