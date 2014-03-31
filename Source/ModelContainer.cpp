#include "ModelContainer.h"

ModelContainer::ModelContainer()
{
	lodCalcTimer = 0;

	sdShader.Load("Data/Shaders/StaticMeshDiffuse.vert", "Data/Shaders/GDiffuse.frag");
	sdShader.Locate("ModelViewProjectionMatrix");	//0
	sdShader.Locate("ModelMatrix");					//1
	sdShader.Locate("DiffuseMap");					//2

	sdnShader.Load("Data/Shaders/StaticMeshDiffuseAndNormal.vert", "Data/Shaders/GDiffuseAndNormal.frag");
	sdnShader.Locate("ModelViewProjectionMatrix");	//0
	sdnShader.Locate("ModelMatrix");				//1
	sdnShader.Locate("DiffuseMap");					//2
	sdnShader.Locate("NormalMap");					//3

	sShadowShader.Load("Data/Shaders/StaticMeshShadow.vert", "Data/Shaders/MeshShadow.frag");
	sShadowShader.Locate("ModelMatrix");		//0
	sShadowShader.Locate("ViewMatrix");			//1
	sShadowShader.Locate("LightMvpMatrix");		//2

	adShader.Load("Data/Shaders/BoneMeshDiffuse.vert", "Data/Shaders/GDiffuse.frag");
	adShader.Locate("ModelViewProjectionMatrix");	//0
	adShader.Locate("ModelMatrix");					//1
	adShader.Locate("DiffuseMap");					//2
	adShader.Locate("gBones");						//3

	adnShader.Load("Data/Shaders/BoneMeshDiffuseAndNormal.vert", "Data/Shaders/GDiffuseAndNormal.frag");
	adnShader.Locate("ModelViewProjectionMatrix");	//0
	adnShader.Locate("ModelMatrix");				//1
	adnShader.Locate("DiffuseMap");					//2
	adnShader.Locate("NormalMap");					//3
	adnShader.Locate("gBones");						//4

	aShadowShader.Load("Data/Shaders/BoneMeshShadow.vert","Data/Shaders/MeshShadow.frag");
	aShadowShader.Locate("ModelMatrix");		//0
	aShadowShader.Locate("ViewMatrix");			//1
	aShadowShader.Locate("LightMvpMatrix");		//2
	aShadowShader.Locate("gBones");				//3

	//PRE SEND UNIFORMS FOR TEXTURES
	sdShader.Bind();
	sdShader.Send(2, 0);

	sdnShader.Bind();
	sdnShader.Send(2, 0);
	sdnShader.Send(3, 1);

	adShader.Bind();
	adShader.Send(2, 0);

	adnShader.Bind();
	adnShader.Send(2, 0);
	adnShader.Send(3, 1);

	glUseProgram(0);
}

ModelContainer::~ModelContainer()
{
	for(unsigned int i = 0; i < animatedModels.size(); i++)
	{
		delete animatedModels[i];
		animatedModels[i] = 0;
	}

	for(unsigned int i = 0; i < staticModels.size(); i++)
	{
		delete staticModels[i];
		staticModels[i] = 0;
	}

	staticModels.clear();
	animatedModels.clear();
}

void ModelContainer::AddAnimatedModel(AnimatedModel* model)
{
	animatedModels.push_back(model);
}

void ModelContainer::AddStaticModel(StaticModel* model)
{
	staticModels.push_back(model);
}

void ModelContainer::Update(const float& deltaTime)
{
}

void ModelContainer::Render(DeferredBuffers* bufs, const float& deltaTime)
{
	std::size_t i = 0;
	StaticModel* sTmp = 0;
	AnimatedModel* aTmp = 0;
	glm::mat4 projView = bufs->GetProjectionMatrix() * bufs->GetViewMatrix();

	unsigned int shadowBits[3] = {SHADOW_CLOSE, SHADOW_MEDIUM, SHADOW_FAR};

	lodCalcTimer++;
	if(lodCalcTimer > 30)
	{
		for(i = 0; i < staticModels.size(); i++)
			staticModels[i]->DecideLevelOfDetail(bufs->camPosition);

		for(i = 0; i < animatedModels.size(); i++)
		{
			aTmp = animatedModels[i];

			glm::vec3 toVector = aTmp->GetPosition() - bufs->camPosition;
			float dist = glm::dot(toVector, toVector);

			if(dist > 40.0f*40.0f)
				aTmp->SetShading(MAPPED_DIFFUSE);
			else
				if(aTmp->GetTexture(1) != 0)
					aTmp->SetShading(MAPPED_DIFFUSE | MAPPED_NORMAL);
		}
		lodCalcTimer = 0;
	}

	for(i = 0; i < animatedModels.size(); i++)
		animatedModels[i]->Update(deltaTime);

	//SHADOW PRE RENDERING
	for(int shadow = 0; shadow < 3; shadow++)
	{
		(bufs->DoShadows(shadow));
		sShadowShader.Bind();
		for(i = 0; i < staticModels.size(); i++)
		{
			sTmp = staticModels[i];
			if(sTmp->GetShadowFlags() & shadowBits[i] && !sTmp->GetCullingFlag())
			{
				sShadowShader.Send(0, sTmp->GetMatrix());
				sShadowShader.Send(1, bufs->GetViewMatrix());
				sShadowShader.Send(2, bufs->GetShadowMap()->GetLightMatrix(shadow));

				sTmp->GetMesh()->Render(0x00);
			}
		}

		aShadowShader.Bind();
		for(i = 0; i < animatedModels.size(); i++)
		{
			aTmp = animatedModels[i];
			if(aTmp->GetShadowFlags() & shadowBits[i] && !aTmp->GetCullingFlag())
			{
				aShadowShader.Send(0, aTmp->GetMatrix());
				aShadowShader.Send(1, bufs->GetViewMatrix());
				aShadowShader.Send(2, bufs->GetShadowMap()->GetLightMatrix(shadow));
				aShadowShader.Send(3, aTmp->GetBoneTransforms());

				aTmp->GetMesh()->Render(0x00);
			}
		}
	}


	//PREPARE GBUFFER
	bufs->SetUpGBuffer();

	//DRAW ALL STATIC MESHES TO GBUFFER
	sdShader.Bind();
	for(i = 0; i < staticModels.size(); i++)
	{
		sTmp = staticModels[i];
		if(!sTmp->GetCullingFlag() && !(sTmp->GetShadingFlags() & MAPPED_NORMAL))
		{
			sTmp->GetTexture(0)->Bind(GL_TEXTURE0);
			sdShader.Send(0, projView * sTmp->GetMatrix()); 
			sdShader.Send(1, sTmp->GetMatrix());
			sTmp->GetMesh()->Render(SEND_NORMALS | SEND_TEXCOORDS);
		}
	}

	sdnShader.Bind();
	for(i = 0; i < staticModels.size(); i++)
	{
		sTmp = staticModels[i];
		if(!sTmp->GetCullingFlag() && (sTmp->GetShadingFlags() & MAPPED_NORMAL))
		{
			sTmp->GetTexture(0)->Bind(GL_TEXTURE0);
			sTmp->GetTexture(1)->Bind(GL_TEXTURE1);
			sdnShader.Send(0, projView * sTmp->GetMatrix()); 
			sdnShader.Send(1, sTmp->GetMatrix());
			sTmp->GetMesh()->Render(SEND_NORMALS | SEND_TANGENTS | SEND_TEXCOORDS);
		}
	}

	//DRAW ALL ANIMATED MESHES TO GBUFFER
	adShader.Bind();
	for(i = 0; i < animatedModels.size(); i++)
	{
		aTmp = animatedModels[i];
		if(!aTmp->GetCullingFlag() && !(aTmp->GetShadingFlags() & MAPPED_NORMAL))
		{
			aTmp->GetTexture(0)->Bind(GL_TEXTURE0);

			adShader.Send(0, projView * aTmp->GetMatrix()); 
			adShader.Send(1, aTmp->GetMatrix());
			adShader.Send(3, aTmp->GetBoneTransforms());
				
			aTmp->GetMesh()->Render(SEND_NORMALS | SEND_TEXCOORDS);
		}
	}

	adnShader.Bind();
	for(i = 0; i < animatedModels.size(); i++)
	{
		aTmp = animatedModels[i];
		if(!aTmp->GetCullingFlag() && (aTmp->GetShadingFlags() & MAPPED_NORMAL))
		{
			aTmp->GetTexture(0)->Bind(GL_TEXTURE0);
			aTmp->GetTexture(1)->Bind(GL_TEXTURE1);

			adnShader.Send(0, projView * aTmp->GetMatrix()); 
			adnShader.Send(1, aTmp->GetMatrix());
			adnShader.Send(4, aTmp->GetBoneTransforms());
				
			aTmp->GetMesh()->Render(SEND_NORMALS | SEND_TANGENTS | SEND_TEXCOORDS);
		}
	}
	

	//DRAW TO SCREEN
	bufs->RenderToScreen();

	aTmp = 0;
	sTmp = 0;
}