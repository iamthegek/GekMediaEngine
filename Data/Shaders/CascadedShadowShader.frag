#version 330

uniform sampler2D DepthMap;
uniform sampler2D ShadowMap0;
uniform sampler2D ShadowMap1;
uniform sampler2D ShadowMap2;

uniform mat4 InverseProjMatrix;

uniform mat4 LightMvp0;
uniform mat4 LightMvp1;
uniform mat4 LightMvp2;
uniform vec2 WindowSize = vec2(1280, 720);

out vec4 Color;

void main()
{	
	vec2 TexCoord = gl_FragCoord.xy / WindowSize;
	float depthScreen = texture2D(DepthMap, TexCoord).x;

	if (depthScreen==1.0)
	{  
		Color = vec4(1.0,1.0,1.0,1.0);
	}
	else
	{
		float depth = depthScreen*2.0-1.0;
		vec3 pos = vec3(TexCoord * 2.0 - 1.0, depth);
		vec4 worldPos = InverseProjMatrix * vec4(pos,1.0);
		float shadowTerm = 1.0;
		vec2 offset = vec2(0.000244,0.000244);

		if (depth<0.93) //map 0
		{
			vec4 lightWorld = LightMvp0 * worldPos;
			lightWorld.xyz /= lightWorld.w;
			lightWorld = lightWorld * 0.5 + 0.5;

			for(float x = -1.0; x < 2.0; x += 1.0) 
				for(float y = -1.0; y < 2.0; y += 2.0) 
					if (lightWorld.z > texture2D(ShadowMap0,lightWorld.xy+vec2(offset.x * x,offset.y * y)).x)
						shadowTerm-=0.06;
		}
		else if (depth<0.985) //map 1
		{
			vec4 lightWorld = LightMvp1 * worldPos;
			lightWorld.xyz /= lightWorld.w;
			lightWorld = lightWorld * 0.5 + 0.5;

			for(float x = -1.0; x < 2.0; x += 1.0) 
				for(float y = -1.0; y < 2.0; y += 2.0) 
					if (lightWorld.z > texture2D(ShadowMap1, lightWorld.xy+vec2(offset.x * x,offset.y * y)).x)
						shadowTerm-=0.06;
		}
		else //map 2
		{
			vec4 lightWorld = LightMvp2 * worldPos;
			lightWorld.xyz /= lightWorld.w;
			lightWorld = lightWorld * 0.5 + 0.5;

			for(float x = -1.0; x < 2.0; x += 1.0) 
				for(float y = -1.0; y < 2.0; y += 2.0)
					if (lightWorld.z > texture2D(ShadowMap2,lightWorld.xy+vec2(offset.x * x,offset.y * y)).x)
						shadowTerm-=0.06;
		}
		Color = vec4(shadowTerm*0.8,shadowTerm*0.8,shadowTerm,1.0);
	}
}