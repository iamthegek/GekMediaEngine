#version 330

layout (location = 0) in vec3 Position;                
layout (location = 1) in vec2 TexCoord;                
layout (location = 2) in vec3 Normal;
layout (location = 3) in vec3 Tangent;
layout (location = 4) in ivec2 BoneIds;
layout (location = 5) in vec2 BoneWeights;

uniform mat4 ModelViewProjectionMatrix;
uniform mat4 ModelMatrix;
uniform mat4 gBones[64];

out vec2 TexCoord0;
out vec3 WorldPosition0;
out mat3 Tbn0;

void main()
{
    mat4 boneTransform = gBones[BoneIds[0]] * BoneWeights[0] + gBones[BoneIds[1]] * BoneWeights[1];
    vec4 boneSpace	= boneTransform * vec4(Position, 1.0);
    gl_Position		= ModelViewProjectionMatrix * vec4(boneSpace.xyz, 1.0);
	WorldPosition0	= (ModelMatrix * vec4(boneSpace.xyz, 1.0)).xyz;

	//setup TBN
	vec4 boneNormal	 = boneTransform * vec4(Normal, 0.0);
	vec4 boneTangent = boneTransform * vec4(Tangent, 0.0);
	vec3 n = normalize( ModelMatrix * vec4(boneNormal.xyz,0.0) ).xyz; 
	vec3 t = normalize( ModelMatrix * vec4(boneTangent.xyz,0.0) ).xyz; 
	vec3 b = normalize( ModelMatrix * vec4(cross(boneNormal.xyz, boneTangent.xyz), 0.0) ).xyz; 
	Tbn0 = mat3(t,b,n);

	//Texture Coordinates
	TexCoord0 = TexCoord;
}