#version 400

uniform sampler2D DepthMap;
uniform sampler2D ShadowMap0;
uniform sampler2D ShadowMap1;
uniform sampler2D ShadowMap2;

uniform mat4 InverseProjMatrix;

uniform mat4 LightMvp0;
uniform mat4 LightMvp1;
uniform mat4 LightMvp2;

out vec4 Color;

void main()
{	
	vec2 winSize = vec2(1280, 720);
	vec2 TexCoord = gl_FragCoord.xy / winSize;

	float depthScreen = texture2D(DepthMap, TexCoord).x;

	if (depthScreen==1.0)
	{  
		Color = vec4(1.0,1.0,1.0,1.0);
	}
	else
	{
		float depth = depthScreen*2.0-1.0;
		vec3 pos = vec3(0.0,0.0,0.0);
		pos.xy = TexCoord * 2.0 - 1.0;
		pos.z = depth;
		vec4 worldPos = InverseProjMatrix * vec4(pos,1.0);
		float shadowTerm = 1.0;

		if (depth<0.93) //map 0
		{
			vec4 lightWorld = LightMvp0 * worldPos;
			lightWorld.xyz /= lightWorld.w;
			lightWorld /= 2.0;
			lightWorld += 0.5;
			float zLight = lightWorld.z;
			vec2 offset = vec2(0.000244,0.000244);

			for(float x = -1.0; x < 2.0; x += 1.0) 
				for(float y = -1.0; y < 2.0; y += 2.0) 
				{
					float lightDepth = texture2D(ShadowMap0,lightWorld.xy+vec2(offset.x * x,offset.y * y)).x;
					if (zLight >lightDepth)
						shadowTerm-=0.10;
				}
			Color = vec4(shadowTerm,shadowTerm,shadowTerm,1);
			return;
		}
		else if (depth<0.985) //map 1
		{
			vec4 lightWorld = LightMvp1 * worldPos;
			lightWorld.xyz /= lightWorld.w;
			lightWorld /= 2.0;
			lightWorld += 0.5;
			float zLight = lightWorld.z;
			vec2 offset = vec2(0.000244,0.000244);

			for(float x = -1.0; x < 2.0; x += 1.0) 
				for(float y = -1.0; y < 2.0; y += 2.0) 
				{
					float lightDepth = texture2D(ShadowMap1, lightWorld.xy+vec2(offset.x * x,offset.y * y)).x;
					if (zLight >lightDepth)
						shadowTerm-=0.10;
				}
			Color = vec4(shadowTerm,shadowTerm,shadowTerm,1);
			return;
		}
		else //map 2
		{
			vec4 lightWorld = LightMvp2 * worldPos;
			lightWorld.xyz /= lightWorld.w;
			lightWorld /= 2.0;
			lightWorld += 0.5;
			float zLight = lightWorld.z;
			vec2 offset = vec2(0.000244,0.000244);

			for(float x = -1.0; x < 2.0; x += 1.0) 
				for(float y = -1.0; y < 2.0; y += 2.0) 
				{
					float lightDepth = texture2D(ShadowMap2,lightWorld.xy+vec2(offset.x * x,offset.y * y)).x;
					if (zLight >lightDepth)
						shadowTerm-=0.10;
				}
			Color = vec4(shadowTerm,shadowTerm,shadowTerm,1);
			return;
		}
	}
}