#version 420 core

// Input
in vec2 UV_tese;
//in vec3 lightDir;
//in vec3 viewDir;
in float sscale;
in vec4 pos;
// Output
out vec3 color;
//Uniforms
uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;
uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform float offset;
layout (binding=0) uniform sampler2D rockSampler;
layout (binding=1) uniform sampler2D heightTexture;
layout (binding=2) uniform sampler2D rockshadingSampler;
layout (binding=3) uniform sampler2D GrassSampler;
layout (binding=4) uniform sampler2D SnowSampler;
layout (binding=5) uniform sampler2D normalSampler;
layout (binding=6) uniform sampler2D grassNSampler;
layout (binding=7) uniform sampler2D grassRSampler;
layout (binding=8) uniform sampler2D snowNSampler;
layout (binding=9) uniform sampler2D snowRSampler;

void main(){
	//calculate vertex normal: u,v value from 0 to 1, 1/200 = 0.005.
	float h0 = 0.0007*(texture(heightTexture, vec2(UV_tese.x-offset,UV_tese.y+offset)).r*65536.0 + texture(heightTexture, vec2(UV_tese.x-offset,UV_tese.y+offset)).g*256.0 + texture(heightTexture, vec2(UV_tese.x-offset,UV_tese.y+offset)).b)-10.0 - sscale;
	if(h0 <= -10){
		h0 = -10;
	}
	float h1 = 0.0007*(texture(heightTexture, vec2(UV_tese.x-offset,UV_tese.y)).r*65536.0 + texture(heightTexture, vec2(UV_tese.x-offset,UV_tese.y)).g*256.0 + texture(heightTexture, vec2(UV_tese.x-offset,UV_tese.y)).b)-10.0 - sscale;
	if(h1 <= -10){
		h1 = -10;
	}
	float h2 = 0.0007*(texture(heightTexture, vec2(UV_tese.x-offset,UV_tese.y-offset)).r*65536.0 + texture(heightTexture, vec2(UV_tese.x-offset,UV_tese.y-offset)).g*256.0 + texture(heightTexture, vec2(UV_tese.x-offset,UV_tese.y-offset)).b)-10.0 - sscale;
	if(h2 <= -10){
		h2 = -10;
	}
	float h3 = 0.0007*(texture(heightTexture, vec2(UV_tese.x,UV_tese.y+offset)).r*65536.0 + texture(heightTexture, vec2(UV_tese.x,UV_tese.y+offset)).g*256.0 + texture(heightTexture, vec2(UV_tese.x,UV_tese.y+offset)).b)-10.0 - sscale;
	if(h3 <= -10){
		h3 = -10;
	}
	float h5 = 0.0007*(texture(heightTexture, vec2(UV_tese.x,UV_tese.y-offset)).r*65536.0 + texture(heightTexture, vec2(UV_tese.x,UV_tese.y-offset)).g*256.0 + texture(heightTexture, vec2(UV_tese.x,UV_tese.y-offset)).b)-10.0 - sscale;
	if(h5 <= -10){
		h5 = -10;
	}
	float h6 = 0.0007*(texture(heightTexture, vec2(UV_tese.x+offset,UV_tese.y+offset)).r*65536.0 + texture(heightTexture, vec2(UV_tese.x+offset,UV_tese.y+offset)).g*256.0 + texture(heightTexture, vec2(UV_tese.x+offset,UV_tese.y+offset)).b)-10.0 - sscale;
	if(h6 <= -10){
		h6 = -10;
	}
	float h7 = 0.0007*(texture(heightTexture, vec2(UV_tese.x+offset,UV_tese.y)).r*65536.0 + texture(heightTexture, vec2(UV_tese.x+offset,UV_tese.y)).g*256.0 + texture(heightTexture, vec2(UV_tese.x+offset,UV_tese.y)).b)-10.0 - sscale;
	if(h7 <= -10){
		h7 = -10;
	}
	float h8 = 0.0007*(texture(heightTexture, vec2(UV_tese.x+offset,UV_tese.y-offset)).r*65536.0 + texture(heightTexture, vec2(UV_tese.x+offset,UV_tese.y-offset)).g*256.0 + texture(heightTexture, vec2(UV_tese.x+offset,UV_tese.y-offset)).b)-10.0 - sscale;
	if(h8 <= -10){
		h8 = -10;
	}
	float nz = 0.3*(h0-h6)+0.4*(h1-h7)+0.3*(h2-h8);
	float nx = 0.3*(h0-h2)+0.4*(h3-h5)+0.3*(h6-h8);
	vec3 aNormal = vec3(abs(nx),0.01,abs(nz));

	//calculate TBN matrix - Gram-Schmidt method
	vec3 T = normalize(vec3(1, 0, 0));
	vec3 B = normalize(vec3(0, 0, 1));
	vec3 N = normalize(aNormal);
	T = normalize(T - dot(T, N) * N);
	B = normalize(cross(T, N));
	mat3 TBN = mat3(T, B, N);

	vec3 fragPos = TBN * (M * vec4(pos.x,pos.y,pos.z, 1.0)).xyz;
	vec3 lightPos = TBN *  (vec4(lightPos, 1.0)).xyz;
	vec3 cameraPos = TBN *  (vec4(cameraPos, 1.0)).xyz;
	vec3 lightDir = normalize(lightPos - fragPos);
	vec3 viewDir = normalize(cameraPos - fragPos);


	vec2 TUV =2.0 * UV_tese;
	vec3 rockColor = texture(rockSampler, vec2(TUV.x,TUV.y)).rgb;
	vec3 grassColor = texture(GrassSampler, vec2(TUV.x,TUV.y)).rgb;
	vec3 snowColor = texture(SnowSampler, vec2(TUV.x,TUV.y)).rgb;

	float rock_r = texture(rockshadingSampler, vec2(TUV.x,TUV.y)).g;
	float rockshininess = clamp((2.0/(pow(rock_r,4)+1e-2))-2.0, 0.0, 500.0f);
	float grass_r = texture(grassRSampler, vec2(TUV.x,TUV.y)).g;
	float grassshininess = clamp((2.0/(pow(grass_r,4)+1e-2))-2.0, 0.0, 500.0f);
	float snow_r = texture(snowRSampler, vec2(TUV.x,TUV.y)).g;
	float snowshininess = clamp((2.0/(pow(snow_r,4)+1e-2))-2.0, 0.0, 500.0f);

	vec3 rocknormal = texture(normalSampler,vec2(TUV.x,TUV.y)).rgb;
	rocknormal = normalize((rocknormal * 2.0 - 1.0));
	vec3 grassnormal = texture(grassNSampler,vec2(TUV.x,TUV.y)).rgb;
	grassnormal = normalize((grassnormal * 2.0 - 1.0));
	vec3 snownormal = texture(snowNSampler,vec2(TUV.x,TUV.y)).rgb;
	snownormal = normalize((snownormal * 2.0 - 1.0));

	vec3 specularColor = vec3(0.1,0.1,0.1);

	float height =texture(heightTexture, vec2(UV_tese.x,UV_tese.y)).r*65536.0 + texture(heightTexture, vec2(UV_tese.x,UV_tese.y)).g*256.0 + texture(heightTexture, vec2(UV_tese.x,UV_tese.y)).b;
	height = 0.0007*height - 10.0 - sscale;
	if(height <= -10){
		height = -10;
	}

	vec3 textureColor = rockColor;
	float shininess = rockshininess;
	vec3 normal = rocknormal;
	float snowheight = -6.0;
	float grassheight = -7.0;
	if(height > snowheight){
		float factor = (height - snowheight)/2.0;
		if(factor<=1.0){
			textureColor = factor*snowColor+(1.0-factor)*rockColor;
			normal = factor*snownormal + (1.0-factor)*rocknormal;
			shininess = factor*snowshininess + (1.0-factor)*rockshininess;
		}else{
			textureColor = snowColor;
			normal = snownormal;
			shininess = snowshininess;
		}
	}else if(height < grassheight){
		float factor = (grassheight - height)/2.0;
		if(factor<=1.0){
			textureColor = factor*grassColor+(1.0-factor)*rockColor;
			normal = factor*grassnormal + (1.0-factor)*rocknormal;
			shininess = factor*grassshininess + (1.0-factor)*rockshininess;
		}else{
			textureColor = grassColor;
			normal = grassnormal;
			shininess = grassshininess;
		}
	}

	//ambient
	float ambientStrength = 0.7;
	vec3 ambient = ambientStrength * textureColor;

	//diffuse
	float diff = max(dot(normal, lightDir),0);
	vec3 diffuse =  diff * textureColor;

	//specular
	vec3 reflectDir = normalize(reflect(-lightDir, normal));
	float spec = max(dot(viewDir, reflectDir),0.0);
	spec = clamp(pow(spec,shininess),0,1);
	vec3 specular = specularColor * spec;

	//color =specular;
	color = ambient + diffuse + specular;
	//color = vec3(0.0, 0.0, 0.0);
	//color = texture(rockSampler, vec2(UV.x,UV.y)).rgb;
	//color = vec3(abs(normal.x), abs(normal.y), abs(normal.z));
}