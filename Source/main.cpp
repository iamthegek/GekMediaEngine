#include <iostream>					//for std::cout <<
#include <vector>					//for std::vector<T>
#include <string>					//for std::string
#include <cassert>
#include <GL\glew.h>				//MUST include before SFML\OpenGL
#include <SFML\Window.hpp>			//sf::window
#include <SFML\OpenGL.hpp>			//core sf/gl functionality
#include <SFML\Graphics.hpp>
#include <SFML\Audio.hpp>		

#include "Input.h"
#include "DeferredRenderer.h"
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
GLfloat			fieldOfView = 90.0f;
GLuint			frameLimit = 60;
GLfloat			deltaTime = 1.0f / (float)frameLimit;
const GLchar*	windowTitle = "GME v0.0.1";
const GLfloat	nearDistance = 1.0f;
const GLfloat	farDistance = 500.0f;
sf::Window		myWindow;

//Game Variables
LocalGame* localGame = 0;
DeferredRenderer* myRenderer;
glm::vec3 myCamPos; 
glm::vec3 myCamTarget = glm::vec3(0,0,1); 
glm::vec3 myCamUp = glm::vec3(0,1,0);

BoneMesh* tmp = 0;

//CORE
bool Initialize();
void Update();
void Draw();
void DeInitialize();

//OPTIONS
void UpdateCamera();
void Resize(const int& resolution);
void SetFrameLimit(const int& framelim);
void SetVsync(bool flag);
void SetFOV(const float& fov);

//AUDIO
std::vector<sf::SoundBuffer> mySoundBuffers; //Preloaded Sound Files to be attached to sf::Sound
sf::Music myMusic;							 //Only 1 instance of music is necessary
GLfloat musicVolume = 100.0f;
GLfloat soundVolume = 100.0f;
GLfloat masterVolume = 100.0f;
bool SetMusic(const std::string& m);
void PlayMusic();
void PauseMusic();
void StopMusic();
void SetMusicVolume(const GLfloat& v);		//0-100
void SetSoundVolume(const GLfloat& v);		//0-100
void SetMasterVolume(const GLfloat& f);		//0-100

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
				case sf::Event::JoystickConnected:		break;
				case sf::Event::JoystickDisconnected:	break;
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
	myWindow.setVerticalSyncEnabled(true);

	glewInit();
	GLint maxColorAttachments, maxFbos;
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachments);
	glGetIntegerv(GL_MAX_DRAW_BUFFERS,		&maxFbos);
	std::cout << windowTitle << '\n';
	std::cout << "OpenGL Version: "			<< glGetString(GL_VERSION)	<< "\n";
	std::cout << "Max Color Attachments: "	<< maxColorAttachments		<< "\n";
	std::cout << "Max Framebuffers: "		<< maxFbos					<< "\n\n";

	myRenderer = new DeferredRenderer();
	myRenderer->Build(1280,720,fieldOfView,nearDistance,farDistance);
	myRenderer->SetGlobalLightDirection(glm::vec3(-1,-1,0));
	myRenderer->NewPointLight(glm::vec3(1,0,0),glm::vec3(-5,4,2),10.0f);
	tmp = myRenderer->NewBoneMesh("Data/BoneMeshes/boblampclean.md5mesh");
	//tmp->AddColorTexture("Data/ModelTextures/Color.png");
	tmp->AddNormalTexture("Data/ModelTextures/Normal.png");

	localGame = new LocalGame();
	localGame->EnablePlayer(0, ControllerIds::C_JOYSTICK0);
	localGame->GetPlayer(0)->AttachBoneMesh(tmp);

	return true;
}
void Update()
{
	localGame->Update(deltaTime);
	myRenderer->Update(deltaTime);

}
float t = 0.0f;
float angleX = 0.0f;
float angleY = 0.0f;
void Draw()
{
	t+= deltaTime;
	glm::mat4 anim = glm::mat4(1.0);
	glm::vec2 d = localGame->GetPlayer(0)->GetPosition();
	anim*= glm::translate(d.x, 0.0f, d.y);
	anim*= glm::scale(glm::vec3(.1,.1,.1));
	anim*= glm::rotate(90.0f, glm::vec3(1,0,0));

	UpdateCamera();

	//DRAW CASCADED SHADOWS
	if(myRenderer->DoCloseShadows())
	{
		myRenderer->DrawToShadowBuffer(tmp, t, anim);
	}
	if(myRenderer->DoMediumShadows())
	{
	}
	if(myRenderer->DoFarShadows())
	{
	}

	//ENABLE GBUFFER
	myRenderer->SetUpGBuffer();
	{
		glm::vec2 f = (localGame->GetController(0)->GetFaceDirection());
		float targetX = f.y * 60.0f;
		angleX += (targetX-angleX) * (deltaTime*10.0f);
		float targetY = f.x * 60.0f;
		angleY += (targetY-angleY) * (deltaTime*10.0f);

		glm::mat4 m;
		m = glm::rotate(angleX, glm::vec3(0,0,11));
		m *= glm::rotate(angleY, glm::vec3(0,1,0));

		tmp->GetBoneInfo(tmp->GetBoneIndex("neck"))->BoneControl = GlmMatrixToAiMatrix(m);
		myRenderer->DrawToGBuffer(tmp, t, anim);
	}
	//RENDER TO SCREEN
	myRenderer->RenderToScreen();
}
void DeInitialize()
{
	myMusic.stop();
	delete localGame;
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
		myRenderer->Build(1920,1080,fieldOfView,nearDistance,farDistance);
		break;
	case WindowResolutions::R_1440x960:
		myWindow.setSize(sf::Vector2u(1440,960));
		myRenderer->Build(1440,960,fieldOfView,nearDistance,farDistance);
		break;
	case WindowResolutions::R_1280x720:
		myWindow.setSize(sf::Vector2u(1280,720));
		myRenderer->Build(1280,720,fieldOfView,nearDistance,farDistance);
		break;
	case WindowResolutions::R_800x600:
		myWindow.setSize(sf::Vector2u(800,600));
		myRenderer->Build(800,600,fieldOfView,nearDistance,farDistance);
		break;
	case WindowResolutions::R_640x480:
		myWindow.setSize(sf::Vector2u(640,480));
		myRenderer->Build(640,480,fieldOfView,nearDistance,farDistance);
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

bool SetMusic(const std::string& m)
{
	if(!myMusic.openFromFile(m))
		return false;
	myMusic.setLoop(true);
	return true;
}
void PlayMusic()
{
	myMusic.play();
}
void PauseMusic()
{
	myMusic.pause();
}
void StopMusic()
{
	myMusic.stop();
}
void SetMusicVolume(const GLfloat& v)
{
	musicVolume = v;
	myMusic.setVolume((float)musicVolume);
}
void SetSoundVolume(const GLfloat& v)
{
	soundVolume = v;
}
void SetMasterVolume(const GLfloat& f)
{
	masterVolume = f;
	sf::Listener::setGlobalVolume(f);
}