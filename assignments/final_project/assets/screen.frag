#version 450
out vec4 FragColor;
in vec2 UV;

uniform sampler2D _HDRTexture;
uniform sampler2D _Bloom;
uniform bool _BloomActive;
void main()
{
    vec3 hdrColor = texture(_HDRTexture, UV).rgb;      
    vec3 bloomColor = texture(_Bloom, UV).rgb;
    
    if (_BloomActive) 
        hdrColor += bloomColor; // additive blending
    
    FragColor = vec4(hdrColor, 1.0);
} 