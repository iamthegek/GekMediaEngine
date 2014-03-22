#version 400

layout (location = 0) in vec3 Position;
layout (location = 4) in ivec4 BoneIds;
layout (location = 5) in vec4 BoneWeights;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 LightMvpMatrix;
uniform mat4 gBones[100];

void main()
{
    mat4 BoneTransform = gBones[BoneIds[0]] * BoneWeights[0];
    BoneTransform     += gBones[BoneIds[1]] * BoneWeights[1];
    BoneTransform     += gBones[BoneIds[2]] * BoneWeights[2];
    BoneTransform     += gBones[BoneIds[3]] * BoneWeights[3];

    vec4 boneSpace	= BoneTransform * vec4(Position, 1.0);
	vec4 localSpace = ModelMatrix * vec4(boneSpace.xyz, 1.0);
	vec4 worldSpace = ViewMatrix * vec4(localSpace.xyz, 1.0);
	gl_Position = LightMvpMatrix * worldSpace;
}