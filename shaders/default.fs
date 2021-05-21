#version 330 core
struct Material
{
    sampler2D diffuse;
    sampler2D specular; //is not used yet
};

out vec4 FragColor;
  
in vec2 TexCoord;

uniform Material material;

void main()
{
    FragColor = texture(material.diffuse, TexCoord);
}