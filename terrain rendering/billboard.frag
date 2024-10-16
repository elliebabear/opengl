#version 420 core
out vec4 FragColor;

in vec2 texCoords;
in float ISbillboard;

layout (binding=11) uniform sampler2D billboardSample;

void main()
{
    if(ISbillboard>0.993f){
        FragColor = texture(billboardSample, texCoords);
    }else{
        discard;
    }
}