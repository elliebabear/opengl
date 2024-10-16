#version 420 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_ocs;
layout(location = 1) in vec2 vertexUV;

// Output data ; will be interpolated for each fragment.
out vec2 UV;
//out vec3 lightDir;
//out vec3 viewDir;
//out vec3 aNormal;
//out vec3 vextexPos;
out float sscale;

// Values that stay constant for the whole mesh.
//uniform mat4 MVP;
//uniform mat4 M;
//uniform mat4 V;
//uniform vec3 lightPos;
//uniform vec3 cameraPos;
uniform float scale;
//uniform float offset;
layout (binding=1) uniform sampler2D heightTexture;


void main(){
	//Output position of the vertex, in clip space : MVP * position
	float height = texture(heightTexture, vec2(vertexUV.x,vertexUV.y)).r*65536.0 + texture(heightTexture, vec2(vertexUV.x,vertexUV.y)).g*256.0 + texture(heightTexture, vec2(vertexUV.x,vertexUV.y)).b;
	height = 0.0007*height - 10.0 - scale;
	if(height <= -10){
		height = -10;
	}
	vec3 vextexPos = vec3(vertexPosition_ocs.x, vertexPosition_ocs.y + height, vertexPosition_ocs.z);
	//gl_Position =  vec4(vertexPosition_ocs, 1.0);
	//gl_Position =  MVP * vec4(vextexPos, 1.0);
	gl_Position =  vec4(vextexPos, 1.0);
	sscale = scale;

	// UV of the vertex. No special space for this one.
	UV = vertexUV;
}

