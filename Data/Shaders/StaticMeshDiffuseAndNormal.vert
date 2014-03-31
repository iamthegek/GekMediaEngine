#version 330                                           

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;                
layout (location = 2) in vec3 Normal;
layout (location = 3) in vec3 Tangent;                              

uniform mat4 ModelViewProjectionMatrix;
uniform mat4 ModelMatrix;
					                                   
out vec2 TexCoord0;
out vec3 WorldPosition0;
out mat3 Tbn0;
                          
void main()                                            
{
	gl_Position = ModelViewProjectionMatrix * vec4( Position.xyz, 1.0 ); 
	TexCoord0 = TexCoord;

	vec3 n = normalize( ModelMatrix * vec4(Normal,0.0) ).xyz; 
	vec3 t = normalize( ModelMatrix * vec4(Tangent,0.0) ).xyz; 
	vec3 b = normalize( ModelMatrix * vec4(cross(Normal, Tangent), 0.0) ).xyz; 
	Tbn0 = mat3(t,b,n);

	WorldPosition0 = (ModelMatrix * vec4(Position, 1.0)).xyz;
}