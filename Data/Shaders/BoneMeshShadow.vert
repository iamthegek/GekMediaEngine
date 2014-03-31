#version 330

layout (location = 0) in vec3 Position;
layout (location = 4) in ivec2 BoneIds;
layout (location = 5) in vec2 BoneWeights;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 LightMvpMatrix;
uniform mat4 gBones[64];

void main()
{
	mat4 boneTransform = gBones[BoneIds[0]] * BoneWeights[0] + gBones[BoneIds[1]] * BoneWeights[1];
    vec4 boneSpace	= boneTransform * vec4(Position, 1.0);
	vec4 localSpace = ModelMatrix * vec4(boneSpace.xyz, 1.0);
	vec4 worldSpace = ViewMatrix * vec4(localSpace.xyz, 1.0);
	gl_Position = LightMvpMatrix * worldSpace;
}