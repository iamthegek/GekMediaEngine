#version 400                                           

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;                
layout (location = 2) in vec3 Normal;                             

uniform mat4 ModelViewProjectionMatrix;
uniform mat4 ModelMatrix;
					                                   
out vec2 TexCoord0;
out vec3 WorldPosition0;
out vec3 Normal0;
                          
void main()                                            
{
	gl_Position = ModelViewProjectionMatrix * vec4( Position.xyz, 1.0 ); 
	TexCoord0 = TexCoord;
	Normal0 = normalize( ModelMatrix * vec4(Normal,0.0) ).xyz; 
	WorldPosition0 = (ModelMatrix * vec4(Position, 1.0)).xyz;
}