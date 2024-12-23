uniform sampler2D selection_mask;
uniform vec3 outline_color;
uniform int offset;

ivec2 toPixel(vec2 coords) {
    ivec2 size = textureSize(selection_mask, 0);
    ivec2 result = ivec2(coords.x * size.x, coords.y * size.y);
    return result;
}

vec2 toCoords(ivec2 pixel) {
    ivec2 size = textureSize(selection_mask, 0);
    vec2 result = vec2((pixel.x + 0.5) / size.x, (pixel.y + 0.5) / size.y);
    return result;
}

float sqrDist(vec2 v1, vec2 v2) {
    vec2 C = v1 - v2;
    return dot(C, C);
}

vec4 selection_apply(vec4 color) {
    vec2 coord = gl_TexCoord[0].xy;
    ivec2 pixel_coord = toPixel(coord);
    vec4 mask_value = texture2D(selection_mask, coord);
    vec4 result = vec4(0.0, 0.0, 0.0, 0.0);
    if (mask_value != vec4(1.0, 1.0, 1.0, 1.0)) {
        for (int y = pixel_coord.y - offset; y <= pixel_coord.y + offset; y++) {
            bool exit = false;
            for (int x = pixel_coord.x - offset; x <= pixel_coord.x + offset; x++) {
                float sqrd = sqrDist(vec2(pixel_coord.x, pixel_coord.y), vec2(x, y));
                if (sqrd > offset * offset) {
                    continue;
                }
                vec2 coords_here = toCoords(ivec2(x, y));
                vec4 mask_here = texture2D(selection_mask, coords_here);
                if (mask_here == vec4(1.0, 1.0, 1.0, 1.0)) {
                    result = vec4(outline_color, 1.0);
                    exit = true;
                    break;
                }
            }
            if (exit) {
                break;
            }
        }
    }
    return result;
}
