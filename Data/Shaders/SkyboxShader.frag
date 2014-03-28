#version 400

in vec3 TexCoord0;

out vec4 Color;

uniform samplerCube CubeMap;

void main()
{
	Color = texture(CubeMap, TexCoord0);
}