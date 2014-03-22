#include "DeferredRenderer.h"

DeferredRenderer::DeferredRenderer()
{
	ptLightMesh = 0;
	dirLight = 0;
	dirLightMesh = 0;
	gBuffer = 0;
	frameCounter = 0;
	cShadowPassMesh = 0;
}

DeferredRenderer::~DeferredRenderer()
{
	Clear(CLEAR_TYPE::GME_ALL);

	delete ptLightMesh;
	delete dirLightMesh;
	delete dirLight;
	delete cShadowPassMesh;
	delete gBuffer;
}

void DeferredRenderer::Clear(const int& flag)
{
	switch(flag)
	{
	case CLEAR_TYPE::GME_ALL:
		{
			ClearPointLights();
			ClearStaticMeshes();
			ClearBoneMeshes();
			ClearParticleEmitters();
			break;
		}
	case CLEAR_TYPE::GME_ALL_LIGHTS:
		{
			ClearPointLights();
			dirLight->SetDirection(glm::vec3(0,-1,0));
			break;
		}
	case CLEAR_TYPE::GME_ALL_MESHES:
		{
			ClearStaticMeshes();
			ClearBoneMeshes();
			break;
		}
	case CLEAR_TYPE::GME_PARTICLE_EMITTERS:
		{
			ClearParticleEmitters();
			break;
		}
	case CLEAR_TYPE::GME_POINT_LIGHTS:
		{
			ClearPointLights();
			break;
		}
	case CLEAR_TYPE::GME_BONE_MESHES:
		{
			ClearBoneMeshes();
			break;
		}
	case CLEAR_TYPE::GME_STATIC_MESHES:
		{
			ClearStaticMeshes();
			break;
		}
	}
}

void DeferredRenderer::ClearPointLights()
{
	for(unsigned int i = 0; i < ptLights.size(); i++)
	{
		delete ptLights[i];
		ptLights[i] = 0;
	}
	ptLights.clear();
}

void DeferredRenderer::ClearStaticMeshes()
{
	for(unsigned int i = 0; i < sMeshes.size(); i++)
	{
		delete sMeshes[i];
		sMeshes[i] = 0;
	}
	sMeshes.clear();
}

void DeferredRenderer::ClearBoneMeshes()
{
	for(unsigned int i = 0; i < bnMeshes.size(); i++)
	{
		delete bnMeshes[i];
		bnMeshes[i] = 0;
	}
	bnMeshes.clear();
}

void DeferredRenderer::ClearParticleEmitters()
{
	for(unsigned int i = 0; i < particleEmitters.size(); i++)
	{
		delete particleEmitters[i];
		particleEmitters[i] = 0;
	}
	particleEmitters.clear();
}

void DeferredRenderer::Build(const GLsizei& width, 
							 const GLsizei& height, 
							 const GLfloat& fov, 
							 const GLfloat& neardist, 
							 const GLfloat& fardist)
{
	//For Rebuilding to Adjust Window Size
	if(gBuffer != 0)
	{
		delete gBuffer;
		gBuffer = 0;
	}
	dirLightUniformLocs.clear();
	sMeshUniformLocs.clear();
	bnMeshUniformLocs.clear();
	ptLightUniformLocs.clear();
	cShadowBnMeshUniformLocs.clear();
	cShadowSMeshUniformLocs.clear();
	cShadowPassUniformLocs.clear();

	//MAIN VARIABLES
	windowWidth = width; 
	windowHeight = height; 
	fieldOfView = fov; 
	nearRenderDistance = neardist; 
	farRenderDistance = fardist; 
	windowRatio = (GLfloat)windowWidth/(GLfloat)windowHeight;
	worldMatrices.SetPerspective(fieldOfView, windowRatio, nearRenderDistance, farRenderDistance);
	glViewport(0, 0, width, height);
	glClearColor(0,0,0,1);

	//GBUFFER VARIABLES
	gBuffer = new GBuffer();
	gBuffer->Build(width, height);

	//POINT LIGHT VARIABLES
	ptLightShader.Load("Data/Shaders/PointLightShader.vert","Data/Shaders/PointLightShader.frag");
	if(ptLightMesh == 0)
		ptLightMesh = new LightMesh();
	ptLightMesh->LoadMesh("Data/LightMeshes/PointLight.obj");
	ptLightUniformLocs.push_back(glGetUniformLocation(ptLightShader.GetProgram(), "PositionMap"));	//0
	ptLightUniformLocs.push_back(glGetUniformLocation(ptLightShader.GetProgram(), "ColorMap"));		//1
	ptLightUniformLocs.push_back(glGetUniformLocation(ptLightShader.GetProgram(), "NormalMap"));	//2

	//STENCIL LIGHTING VARIABLES
	stencilShader.Load("Data/Shaders/StencilShader.vert","Data/Shaders/StencilShader.frag");
	uniLocStencilMVP = glGetUniformLocation(stencilShader.GetProgram(), "ModelViewProjectionMatrix");
	
	//DIRECTIONAL LIGHT VARIABLES
	dirLightShader.Load("Data/Shaders/DirectionalLightShader.vert", "Data/Shaders/DirectionalLightShader.frag");
	if(dirLightMesh == 0)
	{
		dirLightMesh = new ScreenQuadMesh();
		dirLightMesh->SetUp();
	}
	if(dirLight == 0)
		dirLight = new DirectionalLight(dirLightShader);
	dirLightUniformLocs.push_back(glGetUniformLocation(dirLightShader.GetProgram(), "PositionMap"));	//0
	dirLightUniformLocs.push_back(glGetUniformLocation(dirLightShader.GetProgram(), "ColorMap"));		//1
	dirLightUniformLocs.push_back(glGetUniformLocation(dirLightShader.GetProgram(), "NormalMap"));		//2
	
	//STATIC MESH VARIABLES
	sMeshShader.Load("Data/Shaders/StaticMeshShader.vert", "Data/Shaders/StaticMeshShader.frag");
	sMeshUniformLocs.push_back(glGetUniformLocation(sMeshShader.GetProgram(), "ModelViewProjectionMatrix"));		//0 - MVP
	sMeshUniformLocs.push_back(glGetUniformLocation(sMeshShader.GetProgram(), "ModelMatrix"));						//1 - Model
	sMeshUniformLocs.push_back(glGetUniformLocation(sMeshShader.GetProgram(), "ColorMap"));							//2	- Color Map
	sMeshUniformLocs.push_back(glGetUniformLocation(sMeshShader.GetProgram(), "NormalMap"));						//3	- Normal Map

	//BONE MESH VARIABLES
	bnMeshShader.Load("Data/Shaders/BoneMeshShader.vert", "Data/Shaders/BoneMeshShader.frag");
	bnMeshUniformLocs.push_back(glGetUniformLocation(bnMeshShader.GetProgram(), "ModelViewProjectionMatrix"));		//0 - MVP
	bnMeshUniformLocs.push_back(glGetUniformLocation(bnMeshShader.GetProgram(), "ModelMatrix"));					//1 - Model
	bnMeshUniformLocs.push_back(glGetUniformLocation(bnMeshShader.GetProgram(), "ColorMap"));						//2	- Color Map
	bnMeshUniformLocs.push_back(glGetUniformLocation(bnMeshShader.GetProgram(), "NormalMap"));						//3	- Normal Map
	bnMeshUniformLocs.push_back(glGetUniformLocation(bnMeshShader.GetProgram(), "gBones"));							//4 - Bone Array

	//PARTICLE VARIABLES
	particleShader.Load("Data/Shaders/ParticleShader.vert", "Data/Shaders/ParticleShader.frag");
	
	//SHADOW VARIABLES
	cShadowMap.SetLightDirection(dirLight->direction);
	cShadowMap.SetUp(ShadowResolutions::S_2048x2048);
	frameCounter = curShadowLevel = 0;
	cShadowSMeshShader.Load("Data/Shaders/StaticMeshShadowShader.vert", "Data/Shaders/StaticMeshShadowShader.frag");
	cShadowSMeshUniformLocs.push_back(glGetUniformLocation(cShadowSMeshShader.GetProgram(), "ModelMatrix"));		//0
	cShadowSMeshUniformLocs.push_back(glGetUniformLocation(cShadowSMeshShader.GetProgram(), "ViewMatrix"));			//1
	cShadowSMeshUniformLocs.push_back(glGetUniformLocation(cShadowSMeshShader.GetProgram(), "LightMvpMatrix"));		//2

	cShadowBnMeshShader.Load("Data/Shaders/BoneMeshShadowShader.vert","Data/Shaders/BoneMeshShadowShader.frag");
	cShadowBnMeshUniformLocs.push_back(glGetUniformLocation(cShadowBnMeshShader.GetProgram(), "ModelMatrix"));		//0
	cShadowBnMeshUniformLocs.push_back(glGetUniformLocation(cShadowBnMeshShader.GetProgram(), "ViewMatrix"));		//1
	cShadowBnMeshUniformLocs.push_back(glGetUniformLocation(cShadowBnMeshShader.GetProgram(), "LightMvpMatrix"));	//2
	cShadowBnMeshUniformLocs.push_back(glGetUniformLocation(cShadowBnMeshShader.GetProgram(), "gBones"));			//3

	cShadowPassShader.Load("Data/Shaders/CascadedShadowShader.vert", "Data/Shaders/CascadedShadowShader.frag");
	cShadowPassUniformLocs.push_back(glGetUniformLocation(cShadowPassShader.GetProgram(), "ModelViewProjectionMatrix"));	//0
	cShadowPassUniformLocs.push_back(glGetUniformLocation(cShadowPassShader.GetProgram(), "DepthMap"));						//1
	cShadowPassUniformLocs.push_back(glGetUniformLocation(cShadowPassShader.GetProgram(), "ShadowMap0"));					//2
	cShadowPassUniformLocs.push_back(glGetUniformLocation(cShadowPassShader.GetProgram(), "ShadowMap1"));					//3
	cShadowPassUniformLocs.push_back(glGetUniformLocation(cShadowPassShader.GetProgram(), "ShadowMap2"));					//4
	cShadowPassUniformLocs.push_back(glGetUniformLocation(cShadowPassShader.GetProgram(), "InverseProjMatrix"));			//5
	cShadowPassUniformLocs.push_back(glGetUniformLocation(cShadowPassShader.GetProgram(), "LightMvp0"));					//6
	cShadowPassUniformLocs.push_back(glGetUniformLocation(cShadowPassShader.GetProgram(), "LightMvp1"));					//7
	cShadowPassUniformLocs.push_back(glGetUniformLocation(cShadowPassShader.GetProgram(), "LightMvp2"));					//8
	if(cShadowPassMesh == 0)
	{
		cShadowPassMesh = new ScreenQuadMesh();
		cShadowPassMesh->SetUp();
	}
}

void DeferredRenderer::SetFieldOfView(const GLfloat& fov)
{
	fieldOfView = fov; 
	worldMatrices.SetPerspective(fieldOfView,windowRatio,nearRenderDistance,farRenderDistance);
}

void DeferredRenderer::SetShadowMapResolution(const int& resolution)
{
	cShadowMap.SetUp(resolution);
}

PointLight * DeferredRenderer::NewPointLight(const glm::vec3& color, const glm::vec3& position, const float& radius)
{
	PointLight* ptLight = new PointLight(ptLightShader);
	ptLight->SetColor(color)->SetPosition(position)->SetRadius(radius);
	ptLights.push_back(ptLight);
	return ptLight;
}

StaticMesh * DeferredRenderer::NewStaticMesh(const std::string& Filename)
{
	StaticMesh* sMesh = new StaticMesh();
	sMesh->LoadMesh(Filename);
	sMeshes.push_back(sMesh);
	return sMesh;
}

BoneMesh * DeferredRenderer::NewBoneMesh(const std::string& Filename)
{
	BoneMesh* bnMesh = new BoneMesh();
	bnMesh->LoadMesh(Filename);
	bnMeshes.push_back(bnMesh);
	return bnMesh;
}

ParticleEmitter * DeferredRenderer::NewParticleEmitter(const std::string& Filename, const glm::vec3& Force)
{
	ParticleEmitter * emit = new ParticleEmitter(Filename, Force);
	particleEmitters.push_back(emit);
	return emit;
}

void DeferredRenderer::SetGlobalLightDirection(const glm::vec3& direction)
{
	dirLight->SetDirection(direction);
	cShadowMap.SetLightDirection(dirLight->direction);
}

void DeferredRenderer::LookAt(const glm::vec3& eye, 
							  const glm::vec3& target, 
							  const glm::vec3& up)
{
	worldMatrices.SetPerspective(fieldOfView, windowRatio, nearRenderDistance, farRenderDistance);
	worldMatrices.SetMatrixMode(Matrices::VIEW_MATRIX);
	worldMatrices.LoadIdentity();
	worldMatrices.LookAt(eye, target, up);
	worldViewMatrix = worldMatrices.GetViewMatrix();
	worldMatrices.SetMatrixMode(Matrices::MODEL_MATRIX);
	worldMatrices.LoadIdentity();
	camPosition = eye;
	camDirection = glm::normalize(eye-target);
}

Matrices& DeferredRenderer::GetMatrices()
{
	return worldMatrices;
}

void DeferredRenderer::Update(const float& deltaTime)
{
	frameCounter++;
	for(std::size_t i = 0; i < particleEmitters.size(); i++)
	{
		particleEmitters[i]->Update(deltaTime);
	}
}

bool DeferredRenderer::DoCloseShadows()
{
	curShadowLevel = 0;
	glPolygonOffset(4.2f,1.3f);
	glEnable(GL_POLYGON_OFFSET_FILL);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	cShadowMap.UpdateMatrices(worldMatrices.GetViewMatrix(), camDirection, camPosition);
	cShadowMap.Start(0);
	return true;
}

bool DeferredRenderer::DoMediumShadows()
{
	if(frameCounter % 2 != 1) //Draw every other frame
		return false;

	curShadowLevel = 1;
	cShadowMap.Start(1);
	return true;
}

bool DeferredRenderer::DoFarShadows()
{
	if(frameCounter % 2 == 1) //Draw every other-other frame
		return false;

	curShadowLevel = 2;
	cShadowMap.Start(2);
	return true;
}

void DeferredRenderer::DrawToShadowBuffer(StaticMesh* Mesh, const glm::mat4& objMatrix)
{
	cShadowSMeshShader.Bind();

	glUniformMatrix4fv(cShadowSMeshUniformLocs[0], 1, GL_FALSE, &objMatrix[0][0]);									//0
	glUniformMatrix4fv(cShadowSMeshUniformLocs[1], 1, GL_FALSE, &worldMatrices.GetViewMatrix()[0][0]);				//1
	glUniformMatrix4fv(cShadowSMeshUniformLocs[2], 1, GL_FALSE, &cShadowMap.GetLightMatrix(curShadowLevel)[0][0]);	//2
	
	Mesh->Render();

	cShadowSMeshShader.Unbind();
}

void DeferredRenderer::DrawToShadowBuffer(BoneMesh* Mesh, const float& time, const glm::mat4& objMatrix)
{
	if(curShadowLevel != 0) //only draw up-close bone mesh shadows
		return;

	cShadowBnMeshShader.Bind();

	std::vector<glm::mat4> transforms;
	Mesh->InterpolateBones(time, transforms);
	glUniformMatrix4fv(cShadowBnMeshUniformLocs[3], transforms.size(), GL_FALSE, &transforms[0][0][0]);
	glUniformMatrix4fv(cShadowBnMeshUniformLocs[0], 1, GL_FALSE, &objMatrix[0][0]);						//0
	glUniformMatrix4fv(cShadowBnMeshUniformLocs[1], 1, GL_FALSE, &worldMatrices.GetViewMatrix()[0][0]);	//1
	glUniformMatrix4fv(cShadowBnMeshUniformLocs[2], 1, GL_FALSE, &cShadowMap.GetLightMatrix(0)[0][0]);	//2

	Mesh->Render();

	cShadowBnMeshShader.Unbind();
}

void DeferredRenderer::SetUpGBuffer()
{
	cShadowMap.Stop(0);
	curShadowLevel = 0;
	glDisable(GL_POLYGON_OFFSET_FILL);
	glViewport(0,0,windowWidth, windowHeight);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	gBuffer->BindForGeomPass();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void DeferredRenderer::DrawToGBuffer(StaticMesh* Mesh, const glm::mat4& objMatrix)
{
	sMeshShader.Bind();
	
	for(int i = 0; i < 2; i++)
	{
		if(Mesh->textures[i] != 0)
		{
			Mesh->textures[i]->Bind(GL_TEXTURE0 + i);
			glUniform1i(sMeshUniformLocs[2+i], i);
		}
	}
	glUniformMatrix4fv(sMeshUniformLocs[0], 1, GL_FALSE, &(worldMatrices.GetProjectionMatrix() * worldMatrices.GetViewMatrix() * objMatrix)[0][0]);	//0
	glUniformMatrix4fv(sMeshUniformLocs[1], 1, GL_FALSE, &objMatrix[0][0]);																			//1

	Mesh->Render();
	
	Texture::Unbind(GL_TEXTURE2);
	Texture::Unbind(GL_TEXTURE1);
	Texture::Unbind(GL_TEXTURE0);

	sMeshShader.Unbind();
}

void DeferredRenderer::DrawToGBuffer(BoneMesh* Mesh, const float& time, const glm::mat4& objMatrix)
{
	bnMeshShader.Bind();

	for(int i = 0; i < 2; i++)
	{
		if(Mesh->textures[i] != 0)
		{
			Mesh->textures[i]->Bind(GL_TEXTURE0 + i);
			glUniform1i(bnMeshUniformLocs[2+i], i);
		}
	}
	std::vector<glm::mat4> transforms;
	Mesh->InterpolateBones(time, transforms);
	glUniformMatrix4fv(bnMeshUniformLocs[4], transforms.size(), GL_FALSE, &transforms[0][0][0]);
	glUniformMatrix4fv(bnMeshUniformLocs[0], 1, GL_FALSE, &(worldMatrices.GetProjectionMatrix() * worldMatrices.GetViewMatrix() * objMatrix)[0][0]);	//0
	glUniformMatrix4fv(bnMeshUniformLocs[1], 1, GL_FALSE, &objMatrix[0][0]);																			//1

	Mesh->Render();

	Texture::Unbind(GL_TEXTURE2);
	Texture::Unbind(GL_TEXTURE1);
	Texture::Unbind(GL_TEXTURE0);
	
	bnMeshShader.Unbind();
}

void DeferredRenderer::RenderToScreen()
{
	glDepthMask(GL_FALSE);
	glEnable(GL_STENCIL_TEST);
	ptLightMesh->Bind(); //Bind LightMesh VAO
	for(std::vector<PointLight*>::iterator it = ptLights.begin(); it != ptLights.end(); it++)
	{
		DoLightStencil(*it);
		DoLightPass(*it);
	}
	ptLightMesh->Unbind();
	glDisable(GL_STENCIL_TEST);
	DoDirectionalLight();
	DoShadowPass();
	DoFinalPass();
	ForwardRenderParticles();
}

void DeferredRenderer::ForwardRenderParticles()
{
	particleShader.Bind();

	particleShader.Unbind();
}

void DeferredRenderer::DoLightStencil(PointLight* light)
{
	stencilShader.Bind();
	gBuffer->BindForStencilPass();
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glClear(GL_STENCIL_BUFFER_BIT);
	glStencilFunc(GL_ALWAYS, 0, 0);
	glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
	glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
	worldMatrices.SetMatrixMode(Matrices::MODEL_MATRIX);
	worldMatrices.PushMatrix();
	worldMatrices.Translate(light->position.x, light->position.y, light->position.z);
	worldMatrices.Scale(light->radius,light->radius,light->radius);
	glUniformMatrix4fv(uniLocStencilMVP, 1, GL_FALSE, &worldMatrices.GetModelViewProjectionMatrix()[0][0]);
	ptLightMesh->Render();
	worldMatrices.PopMatrix();
}

void DeferredRenderer::DoLightPass(PointLight* light)
{
	ptLightShader.Bind();
	gBuffer->BindForLightPass();
	glUniform1i(ptLightUniformLocs[0], 0);	//POSIITON
	glUniform1i(ptLightUniformLocs[1], 1);	//COLOR
	glUniform1i(ptLightUniformLocs[2], 2);	//NORMAL
	glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	light->SendToShader(worldMatrices, camPosition);
	ptLightMesh->Render();
    glCullFace(GL_BACK);
	glDisable(GL_BLEND);
}

void DeferredRenderer::DoDirectionalLight()
{
	gBuffer->BindForLightPass();
	dirLightShader.Bind();
	glUniform1i(dirLightUniformLocs[0], 0);	//POSITION
	glUniform1i(dirLightUniformLocs[1], 1);	//COLOR
	glUniform1i(dirLightUniformLocs[2], 2);	//NORMAL
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);
	dirLight->SendToShader(worldMatrices, camPosition);
	dirLightMesh->Render();
	glDisable(GL_BLEND);
}

void DeferredRenderer::DoShadowPass()
{
	cShadowPassShader.Bind();

	glm::mat4 invproj = glm::inverse(glm::perspective(fieldOfView, windowRatio, nearRenderDistance, farRenderDistance));

	worldMatrices.SetMatrixMode(Matrices::VIEW_MATRIX);
	worldMatrices.PushMatrix();
	worldMatrices.LoadIdentity();
	worldMatrices.SetMatrixMode(Matrices::MODEL_MATRIX);
	worldMatrices.PushMatrix();
	worldMatrices.LoadIdentity();
	worldMatrices.SetOrtho(-1,1,-1,1,0,1);

	glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, gBuffer->GetDepthTexture());//Texture(GBuffer::GBUFFER_POSITION));
	glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, cShadowMap.GetDepthTexture(0));
	glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, cShadowMap.GetDepthTexture(1));
	glActiveTexture(GL_TEXTURE3); glBindTexture(GL_TEXTURE_2D, cShadowMap.GetDepthTexture(2));

	glUniformMatrix4fv(cShadowPassUniformLocs[0], 1, GL_FALSE, &worldMatrices.GetModelViewProjectionMatrix()[0][0]);
	glUniform1i(cShadowPassUniformLocs[1], 0); //1 - Depth Map
	glUniform1i(cShadowPassUniformLocs[2], 1); //2 - Shadow Map 0
	glUniform1i(cShadowPassUniformLocs[3], 2); //3 - Shadow Map 1
	glUniform1i(cShadowPassUniformLocs[4], 3); //4 - Shadow Map 2
	glUniformMatrix4fv(cShadowPassUniformLocs[5], 1, GL_FALSE, &(invproj)[0][0]);
	glUniformMatrix4fv(cShadowPassUniformLocs[6], 1, GL_FALSE, &cShadowMap.GetLightMatrix(0)[0][0]);
	glUniformMatrix4fv(cShadowPassUniformLocs[7], 1, GL_FALSE, &cShadowMap.GetLightMatrix(1)[0][0]);
	glUniformMatrix4fv(cShadowPassUniformLocs[8], 1, GL_FALSE, &cShadowMap.GetLightMatrix(2)[0][0]);

	glEnable(GL_BLEND);
	glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
	cShadowPassMesh->Render();
	glDisable(GL_BLEND);

	worldMatrices.PopMatrix(); //model
	worldMatrices.SetMatrixMode(Matrices::VIEW_MATRIX);
	worldMatrices.PopMatrix(); //view
	worldMatrices.SetMatrixMode(Matrices::MODEL_MATRIX);

	worldMatrices.SetPerspective(fieldOfView, windowRatio, nearRenderDistance, farRenderDistance);

	cShadowPassShader.Unbind();
}

void DeferredRenderer::DoFinalPass()
{
	gBuffer->BindForFinalPass();
	glUseProgram(0);
	glBlitFramebuffer(0, 0, windowWidth, windowHeight, 0, 0, windowWidth, windowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}