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

	lodCalcTimer++;
	if(lodCalcTimer > 30)
	{
		for(i = 0; i < staticModels.size(); i++)
			staticModels[i]->DecideLevelOfDetail(bufs->camPosition);

		for(i = 0; i < animatedModels.size(); i++)
		{
			aTmp = animatedModels[i];

			glm::vec3 toVector = aTmp->GetPosition() - bufs->camPosition;
			float dist = glm::length(toVector);

			if(dist > 40.0f)
				aTmp->SetStatus(STATUS_SHADING, SHADING_DIFFUSE);
			else
				if(aTmp->GetTexture(1) != 0)
					aTmp->SetStatus(STATUS_SHADING, SHADING_DIFFUSE_AND_NORMAL);
		}
		lodCalcTimer = 0;
	}

	for(i = 0; i < animatedModels.size(); i++)
		animatedModels[i]->Update(deltaTime);

	//SHADOW PRE RENDERING
	for(int shadow = 0; shadow < 3; shadow++)
	{
		if(bufs->DoShadows(shadow))
		{
			sShadowShader.Bind();
			for(i = 0; i < staticModels.size(); i++)
			{
				sTmp = staticModels[i];
				if(sTmp->GetStatus(STATUS_SHADOW) >= SHADOW_CLOSE + shadow && sTmp->GetStatus(STATUS_CULLING) != CULL_ENABLED)
				{
					sShadowShader.Send(0, sTmp->GetMatrix());
					sShadowShader.Send(1, bufs->GetViewMatrix());
					sShadowShader.Send(2, bufs->GetShadowMap()->GetLightMatrix(shadow));

					sTmp->GetMesh()->Render();
				}
			}

			aShadowShader.Bind();
			for(i = 0; i < animatedModels.size(); i++)
			{
				aTmp = animatedModels[i];
				if(aTmp->GetStatus(STATUS_SHADOW) >= SHADOW_CLOSE + shadow && aTmp->GetStatus(STATUS_CULLING) != CULL_ENABLED)
				{
					aShadowShader.Send(0, aTmp->GetMatrix());
					aShadowShader.Send(1, bufs->GetViewMatrix());
					aShadowShader.Send(2, bufs->GetShadowMap()->GetLightMatrix(shadow));
					aShadowShader.Send(3, aTmp->GetBoneTransforms());

					aTmp->GetMesh()->Render();
				}
			}
		}
	}


	//PREPARE GBUFFER
	bufs->SetUpGBuffer();

	//DRAW ALL STATIC MESHES TO GBUFFER
	for(i = 0; i < staticModels.size(); i++)
	{
		sTmp = staticModels[i];
		if(sTmp->GetStatus(STATUS_CULLING) != CULL_ENABLED)
		{
			if(sTmp->GetStatus(STATUS_SHADING) == SHADING_DIFFUSE)
			{
				sTmp->GetTexture(0)->Bind(GL_TEXTURE0);

				sdShader.Bind();
				sdShader.Send(0, bufs->GetProjectionMatrix() * bufs->GetViewMatrix() * sTmp->GetMatrix()); 
				sdShader.Send(1, sTmp->GetMatrix());
				sdShader.Send(2, 0);

				sTmp->GetMesh()->Render();
			}
			else //SHADING_DIFFUSE_AND_NORMAL
			{
				sTmp->GetTexture(0)->Bind(GL_TEXTURE0);
				sTmp->GetTexture(1)->Bind(GL_TEXTURE1);

				sdnShader.Bind();
				sdnShader.Send(0, bufs->GetProjectionMatrix() * bufs->GetViewMatrix() * sTmp->GetMatrix()); 
				sdnShader.Send(1, sTmp->GetMatrix());
				sdnShader.Send(2, 0); //Texture 0
				sdnShader.Send(3, 1); //Texture 1

				sTmp->GetMesh()->Render();
			}
		}
	}

	//DRAW ALL ANIMATED MESHES TO GBUFFER
	for(i = 0; i < animatedModels.size(); i++)
	{
		aTmp = animatedModels[i];
		if(aTmp->GetStatus(STATUS_CULLING) != CULL_ENABLED)
		{
			if(aTmp->GetStatus(STATUS_SHADING) == SHADING_DIFFUSE)
			{
				aTmp->GetTexture(0)->Bind(GL_TEXTURE0);

				adShader.Bind();
				adShader.Send(0, bufs->GetProjectionMatrix() * bufs->GetViewMatrix() * aTmp->GetMatrix()); 
				adShader.Send(1, aTmp->GetMatrix());
				adShader.Send(2, 0);
				adShader.Send(3, aTmp->GetBoneTransforms());
				
				aTmp->GetMesh()->Render();
			}
			else
			{
				aTmp->GetTexture(0)->Bind(GL_TEXTURE0);
				aTmp->GetTexture(1)->Bind(GL_TEXTURE1);

				adnShader.Bind();
				adnShader.Send(0, bufs->GetProjectionMatrix() * bufs->GetViewMatrix() * aTmp->GetMatrix()); 
				adnShader.Send(1, aTmp->GetMatrix());
				adnShader.Send(2, 0);
				adnShader.Send(3, 1);
				adnShader.Send(4, aTmp->GetBoneTransforms());
				
				aTmp->GetMesh()->Render();
			}
		}
	}


	//DRAW TO SCREEN
	bufs->RenderToScreen();

	aTmp = 0;
	sTmp = 0;
}