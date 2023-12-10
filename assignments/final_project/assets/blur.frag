#version 450
out vec4 FragColor;

in vec2 UV;

uniform sampler2D _Screen;

uniform bool _Horizontal;
uniform float weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

void main()
{             
     // gets the size of a texel for offsetting
     vec2 tex_offset = 1.0 / textureSize(_Screen, 0);
     vec3 result = texture(_Screen, UV).rgb * weight[0];
     if(_Horizontal) {
         for(int i = 1; i < 5; ++i) {
            result += texture(_Screen, UV + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(_Screen, UV - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
         }
     }
     else {
         for(int i = 1; i < 5; ++i) {
             result += texture(_Screen, UV + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
             result += texture(_Screen, UV - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
         }
     }
     FragColor = vec4(result, 1.0);
}