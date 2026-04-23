#version 120
uniform sampler2D spriteTex; // The sprite texture
uniform vec4 ignoreColor;   // Color to ignore
uniform float alpha;
uniform bool execute;
uniform float tolerance;

void main() {
    vec4 pixel = texture2D(spriteTex, gl_TexCoord[0].xy);

    if (execute){
        if (distance(pixel.rgb, ignoreColor.rgb) >tolerance && pixel.a>0.9) {
            pixel.rgb = vec3(pixel.r * (1.f-alpha) + alpha,pixel.g * (1.f-alpha) + alpha,pixel.b * (1.f-alpha) + alpha);
        }
    }

    gl_FragColor = pixel * gl_Color;
}