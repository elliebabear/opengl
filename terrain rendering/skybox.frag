#version 420 core
out vec4 FragColor;

in vec3 texCoords;

layout (binding=10) uniform samplerCube skybox;

void main()
{    
    FragColor = texture(skybox, texCoords);
}