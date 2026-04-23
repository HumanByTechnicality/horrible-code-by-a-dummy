uniform sampler2D spriteTex; // The sprite texture
uniform vec4 targetColors[3];   // Color to replace
uniform vec4 replaceColors1[3];  // player 1's color
uniform vec4 replaceColors2[3];  // player 2's color
uniform float tolerance;   // Tolerance for matching
uniform bool fighter;
uniform float hit;

void main() {
    vec4 pixel = texture2D(spriteTex, gl_TexCoord[0].xy);

    // Check against each color in the array
    if (fighter){
        for (int i = 0; i < 3; i++) {
            if (distance(pixel.rgb, targetColors[i].rgb) < tolerance) {
                if (hit>0){
                    pixel.rgb = vec3(1,1,1);//vec3((hit + pixel.r * (1.0-hit)),(hit + pixel.g * (1.0-hit)),(hit + pixel.b * (1.0-hit)));
                    break;
                }
                pixel.rgb = replaceColors2[i].rgb;
                break;
            }
            if (distance(pixel.rgb, vec3(0,0,0))<tolerance) {
                pixel.rgb = vec3(replaceColors2[0][0]/2, replaceColors2[0][1]/2, replaceColors2[0][2]/2);
                break;
            }

        }
    }else{
        for (int i = 0; i<3; i++){
            if (distance(pixel.rgb, targetColors[i].rgb) < tolerance){
                if (hit > 0){
                    pixel.rgb = vec3(1,1,1);//vec3((hit + pixel.r * (1.0-hit)),(hit + pixel.g * (1.0-hit)),(hit + pixel.b * (1.0-hit)));
                    break;
                }
                pixel.rgb = replaceColors1[i].rgb;
                break;
            }
            if (distance(pixel.rgb, vec3(0,0,0))<tolerance) {
                pixel.rgb = vec3(replaceColors1[0][0]/2, replaceColors1[0][1]/2, replaceColors1[0][2]/2);
                break;
            }
        }
    }

    gl_FragColor = pixel * gl_Color;
}
