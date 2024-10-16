#version 420 core

layout (quads, equal_spacing, ccw) in;

in vec2 UV_tesc[];
//in vec3 aNormal_tesc[];

out vec2 UV_tese;
out vec4 pos;
//out vec3 aNormal_tese[];
uniform mat4 MVP;

void main()
{
	float u = gl_TessCoord.x;
    float ou = 1 - u;
    float v = gl_TessCoord.y;
    float ov = 1 - v;
    pos = ou * ov * gl_in[0].gl_Position + u * ov * gl_in[1].gl_Position + u * v * gl_in[2].gl_Position + ou * v * gl_in[3].gl_Position;
    gl_Position = MVP * pos;

    vec2 uv0 = UV_tesc[0];
    vec2 uv1 = UV_tesc[1];
    vec2 uv2 = UV_tesc[2];
    vec2 uv3 = UV_tesc[3];
    vec2 lUV = uv0 + v * (uv3 - uv0);
    vec2 rUV = uv1 + v * (uv2 - uv1);
    vec2 texCoord = lUV + u * (rUV - lUV);
    UV_tese = texCoord;
}