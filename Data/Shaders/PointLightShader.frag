#version 400

uniform sampler2D ColorMap;
uniform sampler2D PositionMap;
uniform sampler2D NormalMap;

uniform vec3 LightPosition;
uniform vec3 LightColor;
uniform float LightRadius;
uniform vec3 EyeWorldPosition;

out vec4 Color;

vec4 CalcLightDirectional(vec3 LightDirection, vec3 WorldPos, vec3 Normal, vec3 BaseColor)
{
    float DiffuseFactor = dot(Normal, -LightDirection);
    vec4 DiffuseColor  = vec4(0, 0, 0, 0);
    vec4 SpecularColor = vec4(0, 0, 0, 0);

    if (DiffuseFactor > 0) 
	{
        DiffuseColor = vec4(LightColor, 1.0) * DiffuseFactor;
        vec3 VertexToEye = normalize(EyeWorldPosition - WorldPos);
        vec3 LightReflect = normalize(reflect(LightDirection, Normal));
        float SpecularFactor = dot(VertexToEye, LightReflect);
        SpecularFactor = pow(SpecularFactor, 100.0); //shininess
        if (SpecularFactor > 0) 
		{
            SpecularColor = vec4(LightColor, 1.0) * SpecularFactor;
        }
    }

    return (DiffuseColor * vec4(BaseColor,1.0) + SpecularColor);
}

vec4 CalcPointLight(vec3 WorldPos, vec3 Normal, vec3 BaseColor)
{
    vec3 LightDirection = WorldPos - LightPosition;
    float Distance = length(LightDirection);
    LightDirection = normalize(LightDirection);

    vec4 LightValue = CalcLightDirectional(LightDirection, WorldPos, Normal, BaseColor);

	vec3 l = (LightPosition - WorldPos) / LightRadius;
    float Attenuation = 1.0 - dot(l,l);
    return LightValue * Attenuation;
}

void main()
{
	vec2 winSize = vec2(1280, 720);
	vec2 st = gl_FragCoord.xy / winSize;
	Color = vec4(CalcPointLight(texture2D(PositionMap, st).rgb,normalize(texture2D(NormalMap, st).rgb * 2.0 - 1.0), texture2D(ColorMap, st).rgb).rgb, 1.0);
}