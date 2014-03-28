#include "DeferredBuffers.h"

DeferredBuffers::DeferredBuffers()
{
	ptLightMesh = 0;
	dirLight = 0;
	dirLightMesh = 0;
	gBuffer = 0;
	frameCounter = 0;
	cShadowPassMesh = 0;
	skyboxCubemap = 0;
	skyboxMesh = 0;
}

DeferredBuffers::~DeferredBuffers()
{
	ClearPointLights();

	delete ptLightMesh;
	delete dirLightMesh;
	delete dirLight;
	delete cShadowPassMesh;
	delete gBuffer;
}

void DeferredBuffers::ClearPointLights()
{
	for(unsigned int i = 0; i < ptLights.size(); i++)
	{
		delete ptLights[i];
		ptLights[i] = 0;
	}
	ptLights.clear();
}

void DeferredBuffers::Build(const GLsizei& width, 
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

	//MAIN VARIABLES
	windowWidth = width; 
	windowHeight = height; 
	fieldOfView = fov; 
	nearRenderDistance = neardist; 
	farRenderDistance = fardist; 
	windowRatio = (GLfloat)windowWidth/(GLfloat)windowHeight;
	projectionMatrix = glm::perspective(fieldOfView, windowRatio, nearRenderDistance, farRenderDistance);
	glViewport(0, 0, width, height);
	glClearColor(0,0,0,1);

	//GBUFFER VARIABLES
	gBuffer = new GBuffer();
	gBuffer->Build(width, height);

	//POINT LIGHT VARIABLES
	ptLightShader.Load("Data/Shaders/PointLightShader.vert","Data/Shaders/PointLightShader.frag");
	if(ptLightMesh == 0)
	{
		ptLightMesh = new LightMesh();
		ptLightMesh->LoadMesh("Data/LightMeshes/PointLight.obj");
	}
	ptLightShader.Locate("PositionMap");				//0
	ptLightShader.Locate("ColorMap");					//1
	ptLightShader.Locate("NormalMap");					//2
	ptLightShader.Locate("LightColor");					//3
	ptLightShader.Locate("LightRadius");				//4
	ptLightShader.Locate("LightPosition");				//5
	ptLightShader.Locate("EyeWorldPosition");			//6
	ptLightShader.Locate("ModelViewProjectionMatrix");	//7

	//STENCIL LIGHTING VARIABLES
	stencilShader.Load("Data/Shaders/StencilShader.vert","Data/Shaders/StencilShader.frag");
	stencilShader.Locate("ModelViewProjectionMatrix");
	
	//DIRECTIONAL LIGHT VARIABLES
	dirLightShader.Load("Data/Shaders/DirectionalLightShader.vert", "Data/Shaders/DirectionalLightShader.frag");
	if(dirLightMesh == 0)
	{
		dirLightMesh = new ScreenQuadMesh();
		dirLightMesh->SetUp();
	}
	if(dirLight == 0)
		dirLight = new DirectionalLight();
	dirLightShader.Locate("PositionMap");				//0
	dirLightShader.Locate("ColorMap");					//1
	dirLightShader.Locate("NormalMap");					//2
	dirLightShader.Locate("ModelViewProjectionMatrix"); //3
	dirLightShader.Locate("LightColor");				//4
	dirLightShader.Locate("LightDirection");			//5
	dirLightShader.Locate("EyeWorldPosition");			//6
	
	//SHADOW VARIABLES
	cShadowMap.SetLightDirection(dirLight->direction);
	cShadowMap.SetUp(ShadowResolutions::S_2048x2048);
	frameCounter = 0;
	cShadowPassShader.Load("Data/Shaders/CascadedShadowShader.vert", "Data/Shaders/CascadedShadowShader.frag");
	cShadowPassShader.Locate("ModelViewProjectionMatrix");	//0
	cShadowPassShader.Locate("DepthMap");					//1
	cShadowPassShader.Locate("ShadowMap0");					//2
	cShadowPassShader.Locate("ShadowMap1");					//3
	cShadowPassShader.Locate("ShadowMap2");					//4
	cShadowPassShader.Locate("InverseProjMatrix");			//5
	cShadowPassShader.Locate("LightMvp0");					//6
	cShadowPassShader.Locate("LightMvp1");					//7
	cShadowPassShader.Locate("LightMvp2");					//8
	if(cShadowPassMesh == 0)
	{
		cShadowPassMesh = new ScreenQuadMesh();
		cShadowPassMesh->SetUp();
	}
	
	//SKYBOX VARIABLES
	if(skyboxMesh == 0)
	{
		skyboxMesh = new LightMesh();
		skyboxMesh->LoadMesh("Data/LightMeshes/Skybox.obj");
	}
	skyboxShader.Load("Data/Shaders/SkyboxShader.vert", "Data/Shaders/SkyboxShader.frag");
	skyboxShader.Locate("ModelViewProjectionMatrix");		//0
	skyboxShader.Locate("CubeMap");							//1
	LoadSkybox("Data/Skyboxes/DefaultPX.png","Data/Skyboxes/DefaultNX.png","Data/Skyboxes/DefaultPY.png","Data/Skyboxes/DefaultNY.png","Data/Skyboxes/DefaultPZ.png","Data/Skyboxes/DefaultNZ.png");
}

void DeferredBuffers::SetFieldOfView(const GLfloat& fov)
{
	fieldOfView = fov; 
	projectionMatrix = glm::perspective(fieldOfView,windowRatio,nearRenderDistance,farRenderDistance);
}

void DeferredBuffers::SetShadowMapResolution(const int& resolution)
{
	cShadowMap.SetUp(resolution);
}

PointLight * DeferredBuffers::NewPointLight(const glm::vec3& color, const glm::vec3& position, const float& radius)
{
	PointLight* ptLight = new PointLight();
	ptLight->color = color;
	ptLight->position = position;
	ptLight->radius = radius;
	ptLights.push_back(ptLight);
	return ptLight;
}

void DeferredBuffers::SetGlobalLightDirection(const glm::vec3& direction)
{
	dirLight->direction = direction;
	cShadowMap.SetLightDirection(dirLight->direction);
}

void DeferredBuffers::SetGlobalLightColor(const glm::vec3& color)
{
	dirLight->color = color;
}

void DeferredBuffers::LoadSkybox(const std::string& positiveX, const std::string& negativeX,
								  const std::string& positiveY, const std::string& negativeY,
								  const std::string& positiveZ, const std::string& negativeZ)
{
	if(skyboxCubemap != 0)
		delete skyboxCubemap;
	skyboxCubemap = new Texture(positiveX, negativeX, positiveY, negativeY, positiveZ, negativeZ);
}

void DeferredBuffers::LookAt(const glm::vec3& eye, const glm::vec3& target, const glm::vec3& up)
{
	projectionMatrix = glm::perspective(fieldOfView, windowRatio, nearRenderDistance, farRenderDistance);
	viewMatrix = glm::lookAt(eye, target, up);
	camPosition = eye;
	camDirection = glm::normalize(eye-target);
	cShadowMap.UpdateMatrices(viewMatrix, camDirection, camPosition);
}

CascadedShadowMap * DeferredBuffers::GetShadowMap()
{
	return &cShadowMap;
}

bool DeferredBuffers::DoShadows(const GLint& which)
{
	if(which == 0)
	{
		glPolygonOffset(4.2f,1.3f);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
		cShadowMap.Start(0);
		return true;
	}

	if(which == 1)
	{
		if(frameCounter % 2 != 1)
			return false;
		cShadowMap.Start(1);
		return true;
	}

	if(which == 2)
	{
		if(frameCounter % 2 == 1)
			return false;
		cShadowMap.Start(2);
		return true;
	}
	return false;
}

void DeferredBuffers::SetUpGBuffer()
{
	cShadowMap.Stop(0);
	glDisable(GL_POLYGON_OFFSET_FILL);
	glViewport(0,0,windowWidth, windowHeight);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	gBuffer->BindForGeomPass();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void DeferredBuffers::RenderToScreen()
{
	frameCounter++;
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
	DoSkybox();
	DoFinalPass();
}

void DeferredBuffers::DoLightStencil(PointLight* light)
{
	stencilShader.Bind();
	gBuffer->BindForStencilPass();
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glClear(GL_STENCIL_BUFFER_BIT);
	glStencilFunc(GL_ALWAYS, 0, 0);
	glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
	glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
	glm::mat4 m = glm::translate(light->position) * glm::scale(glm::vec3(light->radius));
	stencilShader.Send(0, projectionMatrix * viewMatrix * m);
	ptLightMesh->Render();
}

void DeferredBuffers::DoLightPass(PointLight* light)
{
	ptLightShader.Bind();
	gBuffer->BindForLightPass();
	ptLightShader.Send(0, 0); //position
	ptLightShader.Send(1, 1); //color
	ptLightShader.Send(2, 2); //normal
	glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	glm::mat4 m = glm::translate(light->position) * glm::scale(glm::vec3(light->radius));
	ptLightShader.Send(3, light->color);	
	ptLightShader.Send(4, light->radius);
	ptLightShader.Send(5, light->position);
	ptLightShader.Send(6, camPosition);					
	ptLightShader.Send(7, projectionMatrix*viewMatrix*m);
	
	ptLightMesh->Render();
    glCullFace(GL_BACK);
	glDisable(GL_BLEND);
}

void DeferredBuffers::DoDirectionalLight()
{
	gBuffer->BindForLightPass();

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);

	dirLightShader.Bind();
	dirLightShader.Send(0, 0); //position
	dirLightShader.Send(1, 1); //color
	dirLightShader.Send(2, 2); //normal
	dirLightShader.Send(3, glm::ortho(-1.0f,1.0f,-1.0f,1.0f,0.0f,1.0f));
	dirLightShader.Send(4, dirLight->color);
	dirLightShader.Send(5, dirLight->direction);
	dirLightShader.Send(6, camPosition);

	dirLightMesh->Render();
	glDisable(GL_BLEND);
}

void DeferredBuffers::DoShadowPass()
{
	cShadowPassShader.Bind();

	glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, gBuffer->GetDepthTexture());
	glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, cShadowMap.GetDepthTexture(0));
	glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, cShadowMap.GetDepthTexture(1));
	glActiveTexture(GL_TEXTURE3); glBindTexture(GL_TEXTURE_2D, cShadowMap.GetDepthTexture(2));

	cShadowPassShader.Send(0, glm::ortho(-1.0f,1.0f,-1.0f,1.0f,0.0f,1.0f));
	cShadowPassShader.Send(1, 0);
	cShadowPassShader.Send(2, 1);
	cShadowPassShader.Send(3, 2);
	cShadowPassShader.Send(4, 3);
	cShadowPassShader.Send(5, glm::inverse(projectionMatrix));
	cShadowPassShader.Send(6, cShadowMap.GetLightMatrix(0));
	cShadowPassShader.Send(7, cShadowMap.GetLightMatrix(1));
	cShadowPassShader.Send(8, cShadowMap.GetLightMatrix(2));

	glEnable(GL_BLEND);
	glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
	cShadowPassMesh->Render();
	glDisable(GL_BLEND);

	cShadowPassShader.Unbind();
}

void DeferredBuffers::DoSkybox()
{
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);

	skyboxShader.Bind();
	skyboxCubemap->Bind(GL_TEXTURE0);
	glCullFace(GL_FRONT);

	skyboxShader.Send(0, projectionMatrix * glm::mat4(glm::mat3(viewMatrix)));
	skyboxShader.Send(1, 0);

	skyboxMesh->Bind();
	skyboxMesh->Render();
	skyboxMesh->Unbind();

	glCullFace(GL_BACK);
	skyboxCubemap->Unbind(GL_TEXTURE0);
	skyboxShader.Unbind();
}

void DeferredBuffers::DoFinalPass()
{
	gBuffer->BindForFinalPass();
	glUseProgram(0);
	glBlitFramebuffer(0, 0, windowWidth, windowHeight, 0, 0, windowWidth, windowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}