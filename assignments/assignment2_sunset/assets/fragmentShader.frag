#version 450
	out vec4 FragColor;
	in vec2 UV;

	uniform vec3 _SkyColor[4];
    uniform vec3 _SunColor[2];
    uniform float _SunRadius;
    uniform float _SunSpeed;
    uniform vec3 _HillColor[2];

    uniform vec2 _Resolution;
    uniform float _Time;

    float circleSDF(vec2 p, float r){
        return length(p)-r;
    }
    void main(){
        vec2 uvCoords = UV;
    
        // base colors
        vec3 bgDayColor = mix(_SkyColor[0], _SkyColor[1], uvCoords.y);
        vec3 bgNightColor = mix(_SkyColor[2], _SkyColor[3],uvCoords.y+0.3);
        vec4 sunColor = vec4(mix(_SunColor[0], _SunColor[1],uvCoords.y),1.0);
    
        vec3 bgColor = mix(bgNightColor, bgDayColor, uvCoords.y+sin(_Time*_SunSpeed));
    
        vec3 hillColor = mix(_HillColor[0], _HillColor[1],(uvCoords.y)+sin(_Time*_SunSpeed));
    
        // output setup
        vec3 outputColor = bgColor;

        // fixing coords for -1 to 1 space instead of 0 to 1 space
        uvCoords = uvCoords * 2.0 - 1.0;
        uvCoords.x *= _Resolution.x / _Resolution.y;
    
        // making the sun
    
        vec2 sunPos = vec2(0.0, sin(_Time*_SunSpeed)*0.7);
        
        float sun = circleSDF(uvCoords - sunPos, _SunRadius); //second param is radius
        sun = 1.0 - smoothstep(-0.05, 0.05, sun);
    
        outputColor = mix(outputColor, sunColor.rgb, sun*sunColor.a);
     
     
         // render first set of hils
    
        float hill_1 = -0.35 + (0.5*pow(sin(uvCoords.x), 2.0) + 0.2*pow(cosh(5.0*uvCoords.x), -1.0));
        hill_1 = step(abs(hill_1)-0.25, uvCoords.y);
    
        outputColor = mix(hillColor, outputColor, hill_1*1.0);
    

        // Output to screen
        FragColor = vec4(outputColor,1.0);
   }