#version 330

uniform sampler2D ColorMap;
uniform sampler2D PositionMap;
uniform sampler2D NormalMap;

uniform vec3 LightColor;
uniform vec3 LightDirection;
uniform vec3 EyeWorldPosition;
uniform vec2 WindowSize = vec2(1280, 720);

out vec4 Color;

vec3 CalcLightDirectional(vec3 WorldPos, vec3 Normal, vec3 BaseColor)
{
    float DiffuseFactor = dot(Normal, -LightDirection);
    vec3 DiffuseColor  = vec3(0);
    vec3 SpecularColor = vec3(0);
    if (DiffuseFactor > 0) 
	{
        DiffuseColor = LightColor * DiffuseFactor;
        vec3 VertexToEye = normalize(EyeWorldPosition - WorldPos);
        vec3 LightReflect = normalize(reflect(LightDirection, Normal));
        float SpecularFactor = dot(VertexToEye, LightReflect);
        SpecularFactor = pow(SpecularFactor, 100.0); //shininess
        if (SpecularFactor > 0) 
		{
            SpecularColor = LightColor * SpecularFactor;
        }
    }
    return (DiffuseColor * BaseColor + SpecularColor);
}

void main()
{
	vec2 st = gl_FragCoord.xy / WindowSize;
	Color = vec4(CalcLightDirectional(texture2D(PositionMap, st).rgb, normalize(texture2D(NormalMap, st).rgb * 2.0 - 1.0),texture2D(ColorMap, st).rgb), 1.0);
}