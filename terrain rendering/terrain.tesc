#version 420 core

layout (vertices = 4) out;

in vec2 UV[];
//in vec3 aNormal[];

out vec2 UV_tesc[];
//out vec3 aNormal_tesc[];

uniform vec3 cameraPos;

float GetTL(float d0, float d1)
{
	float AvgDistance = (d0 + d1) / 2.0;
	if(AvgDistance <= 2.0){return 8.0;}
	else if(AvgDistance <= 10.0){return 4.0;}
	else{return 1.0;}
}

void main()
{
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

//	 Calculate the distance from the camera to the three control points
	float ed0 = distance(cameraPos, vec3(gl_in[0].gl_Position.xyz));
	float ed1 = distance(cameraPos, vec3(gl_in[1].gl_Position.xyz));
	float ed2 = distance(cameraPos, vec3(gl_in[2].gl_Position.xyz));
	float ed3 = distance(cameraPos, vec3(gl_in[3].gl_Position.xyz));

//	 Calculate the tessellation levels
	gl_TessLevelOuter[0] = GetTL(ed1, ed2);
	gl_TessLevelOuter[1] = GetTL(ed2, ed3);
	gl_TessLevelOuter[2] = GetTL(ed3, ed0);
	gl_TessLevelOuter[3] = GetTL(ed0, ed1);
	gl_TessLevelInner[0] = gl_TessLevelOuter[3];
	gl_TessLevelInner[1] = gl_TessLevelOuter[3];

	UV_tesc[gl_InvocationID] = UV[gl_InvocationID];
}