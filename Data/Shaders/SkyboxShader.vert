#version 330

layout (location = 0) in vec3 Position;

uniform mat4 ModelViewProjectionMatrix;

out vec3 TexCoord0;

void main()
{
	vec4 p = ModelViewProjectionMatrix * vec4(Position, 1.0);
	gl_Position = p.xyww;
	TexCoord0 = Position;
}