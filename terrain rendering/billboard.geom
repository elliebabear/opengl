#version 420 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in vec4 pos[];
in float isBillboard[];

out float ISbillboard;
out vec2 texCoords;

uniform mat4 MVP;

void main() {
	vec4 point = pos[0];

	vec2 size = vec2(2.0f, 4.0f);
	vec2 offset = size * 0.07f;

	gl_Position = MVP * vec4( point.x + offset.x, point.y+offset.y, point.z, point.w );
	texCoords= vec2(1,0);
	ISbillboard = isBillboard[0];
	EmitVertex();

	gl_Position = MVP * vec4( point.x-offset.x, point.y+offset.y, point.z, point.w );
	texCoords = vec2(0,0);
	ISbillboard = isBillboard[0];
	EmitVertex();

	gl_Position = MVP * vec4( point.x+offset.x, point.y-offset.y, point.z, point.w );
	texCoords = vec2(1,1);
	ISbillboard = isBillboard[0];
	EmitVertex();

	gl_Position = MVP * vec4( point.x-offset.x, point.y-offset.y, point.z, point.w );
	texCoords = vec2(0,1);
	ISbillboard = isBillboard[0];
	EmitVertex();

	EndPrimitive();
}  