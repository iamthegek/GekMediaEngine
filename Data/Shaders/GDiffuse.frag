#version 330

uniform sampler2D DiffuseMap;

in vec2 TexCoord0;
in vec3 WorldPosition0;
in vec3 Normal0;

layout (location = 0) out vec4 Position;
layout (location = 1) out vec4 Color;
layout (location = 2) out vec4 Normal;

void main()
{
    Position =  vec4(WorldPosition0,1.0);
	Color = vec4(texture2D(DiffuseMap, TexCoord0).rgb, 1.0);
	Normal = vec4((Normal0+1.0)*0.5, 1.0);
}