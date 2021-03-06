#include <iostream>					//for std::cout <<
#include <vector>					//for std::vector<T>
#include <string>					//for std::string
#include <cassert>
#include <GL\glew.h>				//MUST include before SFML\OpenGL
#include <SFML\Window.hpp>			//sf::window
#include <SFML\OpenGL.hpp>			//core sf/gl functionality
#include <SFML\Graphics.hpp>
#include <SFML\Audio.hpp>		

#include "DeferredBuffers.h"
#include "ModelContainer.h"
#include "LocalGame.h"

enum WindowResolutions
{
	R_1920x1080,
	R_1440x960,
	R_1280x720,
	R_800x600,
	R_640x480
};

enum FrameLimits
{
	F_60hz,
	F_120hz
};

//Window Variables
GLfloat			fieldOfView = 75.0f;
GLuint			frameLimit = 60;
GLfloat			deltaTime = 1.0f / (float)frameLimit;
const GLchar*	windowTitle = "GME v0.0.1";
const GLfloat	nearDistance = 0.5f;
const GLfloat	farDistance = 800.0f;
sf::Window		myWindow;

//Game Variables
LocalGame* localGame = 0;
DeferredBuffers* renderBuffers;
ModelContainer* modelContainer;
glm::vec3 myCamPos; 
glm::vec3 myCamTarget = glm::vec3(0,0,1); 
glm::vec3 myCamUp = glm::vec3(0,1,0);

StaticModel* land = 0;
PointLight* lght = 0;

//CORE
bool Initialize();
void Update();
void Draw();
void DeInitialize();

//OPTIONS
void UpdateCamera();
void Resize(const int& resolution);
void SetFrameLimit(const int& framelim);

void SetFOV(const float& fov);

int main()
{
	if(!Initialize())
	{
		return -1;
	}

	bool RUN = true;
	while(RUN)
	{
		sf::Event evnt;
		while(myWindow.pollEvent(evnt))
		{
			switch(evnt.type)
			{
				case sf::Event::Closed:					RUN = false; break;
				case sf::Event::JoystickButtonPressed:	localGame->JoystickButtonPressed(evnt.joystickButton.button, evnt.joystickButton.joystickId); break;
				case sf::Event::JoystickButtonReleased: localGame->JoystickButtonReleased(evnt.joystickButton.button, evnt.joystickButton.joystickId);	break;
				case sf::Event::JoystickMoved:			localGame->JoystickMoved(evnt.joystickMove.axis, evnt.joystickMove.joystickId, evnt.joystickMove.position); break;
				case sf::Event::KeyPressed:				localGame->KeyPressed(evnt.key.code); break;
				case sf::Event::KeyReleased:			localGame->KeyReleased(evnt.key.code); break;
				case sf::Event::MouseButtonPressed:		localGame->MousePressed(evnt.mouseButton.button); break;
				case sf::Event::MouseButtonReleased:	localGame->MouseReleased(evnt.mouseButton.button); break;
				case sf::Event::MouseMoved:				localGame->MouseMoved(evnt.mouseMove.x, evnt.mouseMove.y); break;
			}
		}
		Update();
		Draw();
		myWindow.display();
	}
	DeInitialize();
	myWindow.close();
	return 0;
}

bool Initialize()
{
	myWindow.create(sf::VideoMode(1280,720), windowTitle, sf::Style::Close);
	myWindow.setFramerateLimit(frameLimit);
	myWindow.setVerticalSyncEnabled(false);

	glewInit();
	GLint maxColorAttachments, maxFbos;
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachments);
	glGetIntegerv(GL_MAX_DRAW_BUFFERS,		&maxFbos);
	std::cout << windowTitle << '\n';
	std::cout << "OpenGL Version: "			<< glGetString(GL_VERSION)	<< "\n";
	std::cout << "Max Color Attachments: "	<< maxColorAttachments		<< "\n";
	std::cout << "Max Framebuffers: "		<< maxFbos					<< "\n\n";

	renderBuffers = new DeferredBuffers();
	renderBuffers->Build(1280,720,fieldOfView,nearDistance,farDistance);
	renderBuffers->SetGlobalLightDirection(glm::vec3(-1,-1,-1));
	renderBuffers->SetGlobalLightColor(glm::vec3(0.9f));
	lght = renderBuffers->NewPointLight(glm::vec3(1,1,0),glm::vec3(-5,4,2),10.0f);

	modelContainer = new ModelContainer();

	land = new StaticModel();
	StaticMesh * s = new StaticMesh();
	s->LoadMesh("Data/CollisionMeshes/test.obj");
	land->AddLevelOfDetail(s, 0.0f);
	land->AddColorTexture("Data/ModelTextures/Grass.png");
//	land->AddNormalTexture("Data/ModelTextures/GrassNormal.png");
	land->SetShading(MAPPED_DIFFUSE);
	modelContainer->AddStaticModel(land);

	localGame = new LocalGame();
	localGame->GetPlayer()->LoadModel(modelContainer);

	return true;
}
void Update()
{
	localGame->Update(deltaTime);
	modelContainer->Update(deltaTime);

}
void Draw()
{
	UpdateCamera();
	modelContainer->Render(renderBuffers, deltaTime);
}
void DeInitialize()
{
	delete localGame;
	delete modelContainer;
	delete renderBuffers;
}

void UpdateCamera()
{
	//renderBuffers->LookAt(myCamPos, myCamTarget, myCamUp);
	renderBuffers->LookAt(localGame->GetPlayer()->GetCamPosition(), localGame->GetPlayer()->GetPosition(), glm::vec3(0,1,0));
}

void Resize(const int& resolution)
{
	switch(resolution)
	{
		case WindowResolutions::R_1920x1080:
			myWindow.setSize(sf::Vector2u(1920,1080));
			renderBuffers->Build(1920,1080,fieldOfView,nearDistance,farDistance);
			break;
		case WindowResolutions::R_1440x960:
			myWindow.setSize(sf::Vector2u(1440,960));
			renderBuffers->Build(1440,960,fieldOfView,nearDistance,farDistance);
			break;
		case WindowResolutions::R_1280x720:
			myWindow.setSize(sf::Vector2u(1280,720));
			renderBuffers->Build(1280,720,fieldOfView,nearDistance,farDistance);
			break;
		case WindowResolutions::R_800x600:
			myWindow.setSize(sf::Vector2u(800,600));
			renderBuffers->Build(800,600,fieldOfView,nearDistance,farDistance);
			break;
		case WindowResolutions::R_640x480:
			myWindow.setSize(sf::Vector2u(640,480));
			renderBuffers->Build(640,480,fieldOfView,nearDistance,farDistance);
			break;
	}
}
void SetFrameLimit(const int& framelim)
{
	switch(framelim)
	{
		case FrameLimits::F_120hz:	myWindow.setFramerateLimit(120); break;
		case FrameLimits::F_60hz:	myWindow.setFramerateLimit(60);	break;
	}
}
void SetFOV(const float& fov)
{
	fieldOfView = fov;
	renderBuffers->SetFieldOfView(fov);
}