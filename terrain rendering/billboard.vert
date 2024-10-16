#version 420 core

// Input vertex data
layout(location = 0) in vec3 vertexPosition_ocs;
layout(location = 1) in vec2 vertexUV;

// Output data
out vec4 pos;
out float isBillboard;

// Values that stay constant for the whole mesh.
uniform float scale;
layout (binding=12) uniform sampler2D heightTexture;

float random(float x)
{
    float y = fract(sin(x)*100000.0);
    return y;
}

void main(){
	//Output position of the vertex, in clip space : MVP * position
	float height = texture(heightTexture, vec2(vertexUV.x,vertexUV.y)).r*65536.0 + texture(heightTexture, vec2(vertexUV.x,vertexUV.y)).g*256.0 + texture(heightTexture, vec2(vertexUV.x,vertexUV.y)).b;
	height = 0.0007*height - 10.0 - scale;
	if(height <= -10){
		height = -10;
	}
	vec3 vextexPos = vec3(vertexPosition_ocs.x, vertexPosition_ocs.y + height, vertexPosition_ocs.z);
	gl_Position  =  vec4(vextexPos, 1.0);
	pos =  vec4(vextexPos, 1.0);

	float flag = 0.0f;
	if(height>-10.0f && height<-7.0f){
		flag = 1.0f;
	}
	float x = vextexPos.x;
	float y = vextexPos.y;
	float z = vextexPos.z;
	float seed = x * 12.345 + y * 67.891 + z * 23.456;
	float random = random(seed);
	isBillboard = random * flag;
}