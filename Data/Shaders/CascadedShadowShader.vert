#version 400

layout (location = 0) in vec3 Position;	

uniform mat4 ModelViewProjectionMatrix;

void main()
{
	gl_Position = ModelViewProjectionMatrix * vec4(Position, 1.0);
}