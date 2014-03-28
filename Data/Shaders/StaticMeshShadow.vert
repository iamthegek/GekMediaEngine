#version 400

layout (location = 0) in vec3 Position;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 LightMvpMatrix;

void main()
{
	vec4 localSpace = ModelMatrix * vec4(Position.xyz, 1.0);
	vec4 worldSpace = ViewMatrix * vec4(localSpace.xyz, 1.0);
	gl_Position = LightMvpMatrix * worldSpace;
}