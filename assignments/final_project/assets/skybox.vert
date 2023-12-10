#version 450

layout (location = 0) in vec3 vPos;

out vec3 TexCoords;

uniform mat4 _Projection;
uniform mat4 _View;

void main()
{
    mat4 view = mat4(mat3(_View));
    TexCoords = vPos;
    vec4 pos = _Projection * view * vec4(vPos, 1.0);
    gl_Position = pos.xyww;
}