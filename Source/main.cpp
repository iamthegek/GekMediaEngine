#include <iostream>
#include <sstream>
#include <GL\glew.h>
#include <SFML\Window.hpp>
#include <SFML\OpenGL.hpp>
#include <SFML\Graphics.hpp>
#include <SFML\OpenGL.hpp>

#include "Controller.h"
#include "DeferredRenderer.h"

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

#define NEAR_DISTANCE 1.0f
#define FAR_DISTANCE 500.0f

//Window Variables
sf::Window myWindow;
float fieldOfView = 90.0f;
unsigned int frameLimit = 60;
GLfloat deltaTime = 1.0f / (float)frameLimit;
const char* windowTitle = "GME v0.0.1";

//Game Variables
DeferredRenderer * myRenderer;
glm::vec3 myCamPos, myCamTarget, myCamUp;
std::vector<JoystickController> myJoysticks;
PcController myPcController;
BoneMesh* tmp = 0;

//Game Functions
bool Initialize();
void Update();
void Draw();
void DeInitialize();

void UpdateCamera();
void Resize(const int& resolution);
void SetFrameLimit(const int& framelim);
void SetVsync(bool flag);
void SetFOV(const float& fov);

void ConnectJoystick(const int& id);
void DisconnectJoystick(const int& id);
void JoystickButtonPressed(const int& buttonId, const int& stickId);
void JoystickButtonReleased(const int& buttonId, const int& stickId);
void JoystickMoved(const int& axis, const int& stickId, const float& amount);

void KeyPressed(const int& key);
void KeyReleased(const int& key);
void MouseMoved(const int& x, const int& y);
void MousePressed(const int& button);
void MouseReleased(const int& button);

int main()
{
	myWindow.create(sf::VideoMode(1280, 720), windowTitle, sf::Style::Close);
	myWindow.setFramerateLimit(frameLimit);
	myWindow.setVerticalSyncEnabled(true);

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
				case sf::Event::JoystickConnected:		ConnectJoystick(evnt.joystickConnect.joystickId); break;
				case sf::Event::JoystickDisconnected:	DisconnectJoystick(evnt.joystickConnect.joystickId); break;
				case sf::Event::JoystickButtonPressed:	JoystickButtonPressed(evnt.joystickButton.button, evnt.joystickButton.joystickId); break;
				case sf::Event::JoystickButtonReleased: JoystickButtonReleased(evnt.joystickButton.button, evnt.joystickButton.joystickId);	break;
				case sf::Event::JoystickMoved:			JoystickMoved(evnt.joystickMove.axis, evnt.joystickMove.joystickId, evnt.joystickMove.position); break;
				case sf::Event::KeyPressed:				myPcController.PressKey(evnt.key.code); break;
				case sf::Event::KeyReleased:			myPcController.ReleaseKey(evnt.key.code); break;
				case sf::Event::MouseButtonPressed:		myPcController.PressMouse(evnt.mouseButton.button); break;
				case sf::Event::MouseButtonReleased:	myPcController.ReleaseMouse(evnt.mouseButton.button); break;
				case sf::Event::MouseMoved:				myPcController.MouseMove(evnt.mouseMove.x, evnt.mouseMove.y); break;
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
	glewInit();
	std::cout<<windowTitle<<'\n';
	std::cout<<"OpenGL Version: "<< glGetString(GL_VERSION) <<"\n";
	int maxColorAttachments, maxFbos;
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachments);
	glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxFbos);
	std::cout << "Max Color Attachments: " << maxColorAttachments << std::endl;
	std::cout << "Max Framebuffers: " << maxFbos << "\n\n";

	myRenderer = new DeferredRenderer();
	myRenderer->Build(1280, 720, fieldOfView, NEAR_DISTANCE, FAR_DISTANCE);

	myRenderer->SetGlobalLightDirection(glm::vec3(-1,-1,0));
	myRenderer->NewPointLight(glm::vec3(1,0,0), glm::vec3(-5,4,2), 10.0f);
	//myRenderer->SetShadowMapResolution(ShadowResolutions::S_1024x1024);
	tmp = myRenderer->NewBoneMesh("Data/BoneMeshes/boblampclean.md5mesh");
	//tmp->AddColorTexture("Data/ModelTextures/Color.png");
	tmp->AddNormalTexture("Data/ModelTextures/Normal.png");

	for(unsigned int i = 0; i < 4; i++)
		if(sf::Joystick::isConnected(i))
			ConnectJoystick(i);

	return true;
}
void Update()
{
	myRenderer->Update(deltaTime);
	myPcController.Update();
	for(unsigned int i = 0; i < myJoysticks.size(); i++)
		myJoysticks[i].Update();
}
float t = 0.0f;
void Draw()
{
	t+= deltaTime;
	glm::mat4 anim = glm::mat4(1.0);
	anim*= glm::scale(glm::vec3(.1,.1,.1));
	anim*= glm::rotate(90.0f, glm::vec3(1,0,0));

	//UPDATE CAMERA
	UpdateCamera();

	//DRAW CASCADED SHADOWS
	if(myRenderer->DoCloseShadows())
	{
		//DRAW BONE MESHES AND STATIC MESHES
		myRenderer->DrawToShadowBuffer(tmp, t, anim);
	}
	if(myRenderer->DoMediumShadows())
	{
		//DRAW STATIC MESHES
	}
	if(myRenderer->DoFarShadows())
	{
		//DRAW STATIC MESHES
	}

	//ENABLE GBUFFER
	myRenderer->SetUpGBuffer();

	//GBUFFER DRAW CODE
	myRenderer->DrawToGBuffer(tmp, t, anim);

	//RENDER TO SCREEN
	myRenderer->RenderToScreen();
}
void DeInitialize()
{
	delete myRenderer;
}

void UpdateCamera()
{
	myCamUp = glm::vec3(0,1,0);
	myCamPos = glm::vec3(0,4,5);
	myCamTarget = glm::vec3(0,3,0);
	myRenderer->LookAt(myCamPos, myCamTarget, myCamUp);
}

void Resize(const int& resolution)
{
	switch(resolution)
	{
	case WindowResolutions::R_1920x1080:
		myWindow.setSize(sf::Vector2u(1920,1080));
		myRenderer->Build(1920,1080,fieldOfView,NEAR_DISTANCE,FAR_DISTANCE);
		break;
	case WindowResolutions::R_1440x960:
		myWindow.setSize(sf::Vector2u(1440,960));
		myRenderer->Build(1440,960,fieldOfView,NEAR_DISTANCE,FAR_DISTANCE);
		break;
	case WindowResolutions::R_1280x720:
		myWindow.setSize(sf::Vector2u(1280,720));
		myRenderer->Build(1280,720,fieldOfView,NEAR_DISTANCE,FAR_DISTANCE);
		break;
	case WindowResolutions::R_800x600:
		myWindow.setSize(sf::Vector2u(800,600));
		myRenderer->Build(800,600,fieldOfView,NEAR_DISTANCE,FAR_DISTANCE);
		break;
	case WindowResolutions::R_640x480:
		myWindow.setSize(sf::Vector2u(640,480));
		myRenderer->Build(640,480,fieldOfView,NEAR_DISTANCE,FAR_DISTANCE);
		break;
	}
}

void SetFrameLimit(const int& framelim)
{
	switch(framelim)
	{
	case FrameLimits::F_120hz:
		myWindow.setFramerateLimit(120);
		break;
	case FrameLimits::F_60hz:
		myWindow.setFramerateLimit(60);
		break;
	}
}

void SetVsync(bool flag)
{
	myWindow.setVerticalSyncEnabled(flag);
}

void SetFOV(const float& fov)
{
	fieldOfView = fov;
	myRenderer->SetFieldOfView(fov);
}

void ConnectJoystick(const int& id)
{
	std::cout << "Gamepad Connected: " << id << '\n';
	JoystickController jc(id);
	myJoysticks.push_back(jc);
}

void DisconnectJoystick(const int& id)
{
	std::cout << "Gamepad Disconnected: " << id << '\n';
	for(unsigned int i = 0; i < myJoysticks.size(); i++)
	{
		if(myJoysticks[i].GetJoystickId() == id)
		{
			myJoysticks.erase(myJoysticks.begin() + i);
			return;
		}
	}
}

void JoystickButtonPressed(const int& buttonId, const int& stickId)
{
	for(unsigned int i = 0; i < myJoysticks.size(); i++)
	{
		if(myJoysticks[i].GetJoystickId() == stickId)
		{
			myJoysticks[i].PressButton(buttonId);
			return;
		}
	}
}

void JoystickButtonReleased(const int& buttonId, const int& stickId)
{
	for(unsigned int i = 0; i < myJoysticks.size(); i++)
	{
		if(myJoysticks[i].GetJoystickId() == stickId)
		{
			myJoysticks[i].ReleaseButton(buttonId);
			return;
		}
	}
}

void JoystickMoved(const int& axis, const int& stickId, const float& amount)
{
	for(unsigned int i = 0; i < myJoysticks.size(); i++)
	{
		if(myJoysticks[i].GetJoystickId() == stickId)
		{
			myJoysticks[i].PlaceAxis(amount, axis);
			return;
		}
	}
}