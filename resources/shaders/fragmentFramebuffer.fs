#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform int changeEffect;

const float offset = 1.0 / 250.0;

void main()
{

   // vec3 col = vec4texture(screenTexture, TexCoords).rgb;
   // FragColor = vec4(col, 1.0);


    if(changeEffect == 0) {
        vec2 offsets[9] = vec2[](
                vec2(-offset,  offset), // top-left
                vec2( 0.0f,    offset), // top-center
                vec2( offset,  offset), // top-right
                vec2(-offset,  0.0f),   // center-left
                vec2( 0.0f,    0.0f),   // center-center
                vec2( offset,  0.0f),   // center-right
                vec2(-offset, -offset), // bottom-left
                vec2( 0.0f,   -offset), // bottom-center
                vec2( offset, -offset)  // bottom-right
            );
        float kernel[9] = float[](
               1.0 / 16, 2.0 / 16, 1.0 / 16,
               2.0 / 16, 4.0 / 16, 2.0 / 16,
               1.0 / 16, 2.0 / 16, 1.0 / 16
            );

        vec3 sampleTex[9];
        for(int i = 0; i < 9; ++i) {
            sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
        }

        vec3 col = vec3(0.0);
        for(int i = 0; i < 9; ++i) {
            col += sampleTex[i] * kernel[i];
        }
        FragColor = vec4(col, 1.0);
    }
    else {
         FragColor = texture(screenTexture, TexCoords);
         float average = (0.2 * FragColor.r + 0.7 * FragColor.g + 0.07 * FragColor.b) / 3.0;
         FragColor = vec4(average, average, average, 1.0);
    }
}
