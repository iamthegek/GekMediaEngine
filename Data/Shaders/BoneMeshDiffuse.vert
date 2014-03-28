#version 400

layout (location = 0) in vec3 Position;                
layout (location = 1) in vec2 TexCoord;                
layout (location = 2) in vec3 Normal;
layout (location = 4) in ivec4 BoneIds;
layout (location = 5) in vec4 BoneWeights;

uniform mat4 ModelViewProjectionMatrix;
uniform mat4 ModelMatrix;
uniform mat4 gBones[64];

out vec2 TexCoord0;
out vec3 WorldPosition0;
out vec3 Normal0;

void main()
{
    mat4 boneTransform = gBones[BoneIds[0]] * BoneWeights[0];
    boneTransform     += gBones[BoneIds[1]] * BoneWeights[1];
    boneTransform     += gBones[BoneIds[2]] * BoneWeights[2];
    boneTransform     += gBones[BoneIds[3]] * BoneWeights[3];

    vec4 boneSpace	= boneTransform * vec4(Position, 1.0);
    gl_Position		= ModelViewProjectionMatrix * vec4(boneSpace.xyz, 1.0);
	WorldPosition0	= (ModelMatrix * vec4(boneSpace.xyz, 1.0)).xyz;

	//setup normals
	vec4 boneNormal	 = boneTransform * vec4(Normal, 0.0);
	Normal0 = normalize( ModelMatrix * vec4(boneNormal.xyz,0.0) ).xyz; 

	//Texture Coordinates
	TexCoord0 = TexCoord;
}