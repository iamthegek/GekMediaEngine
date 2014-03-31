#include <iostream>
#include <fstream>
#include <ostream>
#include <string>
#include <sstream>

#include "Player.h"
#include <SFML\Window.hpp>

const std::string configFiles[2] = {"Data/CONFIG/js.sav","Data/CONFIG/kb.sav"};

std::string sfJoystickCodeToString(const int& button)
{
	switch(button)
	{
	case 0: return "a"; break;
	case 1: return "b"; break;
	case 2: return "x"; break;
	case 3: return "y"; break;
	case 4: return "lb"; break;
	case 5: return "rb"; break;
	case 6: return "back"; break;
	case 7: return "start"; break;
	case 8: return "ls"; break;
	case 9: return "rs"; break;
	}

	return "unknown";
}

int stringToSfJoystickCode(std::string button)
{
	const char* str = button.c_str();
	if(strcmp(str, "a") == 0) return 0;
	if(strcmp(str, "b") == 0) return 1;
	if(strcmp(str, "x") == 0) return 2;
	if(strcmp(str, "y") == 0) return 3;
	if(strcmp(str, "lb") == 0) return 4;
	if(strcmp(str, "rb") == 0) return 5;
	if(strcmp(str, "back") == 0) return 6;
	if(strcmp(str, "start") == 0) return 7;
	if(strcmp(str, "ls") == 0) return 8;
	if(strcmp(str, "rs") == 0) return 9;

	return -1;
}

std::string sfKeycodeToString(const int& key)
{
	std::stringstream sstr;
	if(key >= 0 && key <= 25) //qwerty keys
	{
		sstr << char('a' + key);
		return sstr.str();
	}
	if(key >= 26 && key <= 35) //number keys
	{
		sstr << '0' + (key - 26);
		return sstr.str();
	}
	if(key == sf::Keyboard::Space)	return "spacebar";
	if(key == sf::Keyboard::LShift) return "lshift";
	if(key == sf::Keyboard::RShift) return "rshift";
	if(key == sf::Keyboard::BackSpace) return "backspace";
	if(key == sf::Keyboard::Up) return "up";
	if(key == sf::Keyboard::Down) return "down";
	if(key == sf::Keyboard::Left) return "left";
	if(key == sf::Keyboard::Right) return "right";
	if(key == sf::Keyboard::LControl) return "lctrl";
	if(key == sf::Keyboard::RControl) return "rctrl";

	return "unknown key";
}

int stringToSfKeycode(std::string key)
{
	if(key.length() == 1)
	{
		char c = key[0];
		if(c >= 'a' && c <= 'z') return int(c) - int('a');
		if(c >= '0' && c <= '9') return sf::Keyboard::Num0 + (int(c) - int('0'));
	}

	std::transform(key.begin(), key.end(), key.begin(), ::tolower);
	const char* str = key.c_str();

	if(strcmp(str, "up") == 0) return sf::Keyboard::Up;
	if(strcmp(str, "down") == 0) return sf::Keyboard::Down;
	if(strcmp(str, "left") == 0) return sf::Keyboard::Left;
	if(strcmp(str, "right") == 0) return sf::Keyboard::Right;
	if(strcmp(str, "spacebar") == 0) return sf::Keyboard::Space;
	if(strcmp(str, "backspace") == 0) return sf::Keyboard::BackSpace;
	if(strcmp(str, "lshift") == 0) return sf::Keyboard::LShift;
	if(strcmp(str, "rshift") == 0) return sf::Keyboard::RShift;
	if(strcmp(str, "lctrl") == 0) return sf::Keyboard::LControl;
	if(strcmp(str, "rctrl") == 0) return sf::Keyboard::RControl;

	return -1;
}

void ConfigJoystickFromFile(ControlSetJoystick& in)
{
	in.AXES[AXIS_L_X].ID = sf::Joystick::Axis::X; in.AXES[AXIS_L_X].POSITION = 0.0f;
	in.AXES[AXIS_L_Y].ID = sf::Joystick::Axis::Y; in.AXES[AXIS_L_Y].POSITION = 0.0f;
	in.AXES[AXIS_R_X].ID = sf::Joystick::Axis::U; in.AXES[AXIS_R_X].POSITION = 0.0f;
	in.AXES[AXIS_R_Y].ID = sf::Joystick::Axis::R; in.AXES[AXIS_R_Y].POSITION = 0.0f;
	in.AXES[AXIS_DPAD_X].ID = sf::Joystick::Axis::PovY; in.AXES[AXIS_DPAD_X].POSITION = 0.0f;
	in.AXES[AXIS_DPAD_Y].ID = sf::Joystick::Axis::PovX; in.AXES[AXIS_DPAD_Y].POSITION = 0.0f;
	in.AXES[AXIS_TRIGGER].ID = sf::Joystick::Axis::Z; in.AXES[AXIS_TRIGGER].POSITION = 0.0f;

	in.BUTTONS[BUTTON_JUMP].ID = 0;
	in.BUTTONS[BUTTON_JUMP].FLAGS = 0x00;
	in.BUTTONS[BUTTON_USE].ID = 0;
	in.BUTTONS[BUTTON_USE].FLAGS = 0x00;
	in.INVERT_X = false;
	in.INVERT_Y = false;

	std::string varname;
	std::string svar;
	std::ifstream file(configFiles[0]);
	if(file.is_open())
	{
		while(file >> varname)
		{
			file >> svar;
			if(strcmp(varname.c_str(), "<jump>") == 0)	in.BUTTONS[BUTTON_JUMP].ID = stringToSfJoystickCode(svar);
			if(strcmp(varname.c_str(), "<use>") == 0)	in.BUTTONS[BUTTON_USE].ID = stringToSfJoystickCode(svar);
			if(strcmp(varname.c_str(), "<inv_x>")==0)	in.INVERT_X = (strcmp(varname.c_str(), "true")==0);
			if(strcmp(varname.c_str(), "<inv_y>")==0)	in.INVERT_Y = (strcmp(varname.c_str(), "true")==0);
		}
		file.close();
	}
	else
	{
		std::cout << "Error, could not open joystick config file. \n";
	}

	std::cout << "Joystick Configuration: \n";
	std::cout << "  <jump>     " << in.BUTTONS[BUTTON_JUMP].ID << std::endl;
	std::cout << "  <use>      " << in.BUTTONS[BUTTON_USE].ID << std::endl;
	std::cout << "  <inv_x>    " << (in.INVERT_X ? "true" : "false") << std::endl;
	std::cout << "  <inv_y>    " << (in.INVERT_Y ? "true" : "false") << std::endl;
}

void ConfigKeyboardFromFile(ControlSetKeyboard& in)
{
	for(int i = 0; i < KEYBOARD_BUTTON_COUNT; i++)
	{
		in.BUTTONS[i].ID = 0;
		in.BUTTONS[i].FLAGS = 0x00;
	}
	in.INVERT_X = false;
	in.INVERT_Y = false;
	in.MOUSE_LEFT_FLAGS = 0x00;
	in.MOUSE_RIGHT_FLAGS = 0x00;

	std::string varname;
	std::string svar;
	std::ifstream file(configFiles[1]);
	if(file.is_open())
	{
		while(file >> varname)
		{
			file >> svar;
			if(strcmp(varname.c_str(), "<jump>")==0)	in.BUTTONS[BUTTON_JUMP].ID = stringToSfKeycode(svar);
			if(strcmp(varname.c_str(), "<use>")==0)		in.BUTTONS[BUTTON_USE].ID = stringToSfKeycode(svar);
			if(strcmp(varname.c_str(), "<forward>")==0)	in.BUTTONS[BUTTON_FORWARD].ID = stringToSfKeycode(svar);
			if(strcmp(varname.c_str(), "<backward>")==0)in.BUTTONS[BUTTON_BACKWARD].ID = stringToSfKeycode(svar);
			if(strcmp(varname.c_str(), "<left>")==0)	in.BUTTONS[BUTTON_LEFT].ID = stringToSfKeycode(svar);
			if(strcmp(varname.c_str(), "<right>")==0)	in.BUTTONS[BUTTON_RIGHT].ID = stringToSfKeycode(svar);
			if(strcmp(varname.c_str(), "<inv_x>")==0)	in.INVERT_X = (strcmp(varname.c_str(), "true")==0);
			if(strcmp(varname.c_str(), "<inv_y>")==0)	in.INVERT_Y = (strcmp(varname.c_str(), "true")==0);
		}
		file.close();
	}
	else
	{
		std::cout << "Error, could not open keyboard config file. \n";
	}

	std::cout << "\nKeyboard Configuration: \n";
	std::cout << "  <jump>     " << sfKeycodeToString(in.BUTTONS[BUTTON_JUMP].ID) << std::endl;
	std::cout << "  <use>      " << sfKeycodeToString(in.BUTTONS[BUTTON_USE].ID) << std::endl;
	std::cout << "  <forward>  " << sfKeycodeToString(in.BUTTONS[BUTTON_FORWARD].ID) << std::endl;
	std::cout << "  <backward> " << sfKeycodeToString(in.BUTTONS[BUTTON_BACKWARD].ID) << std::endl;
	std::cout << "  <left>     " << sfKeycodeToString(in.BUTTONS[BUTTON_LEFT].ID) << std::endl;
	std::cout << "  <right>    " << sfKeycodeToString(in.BUTTONS[BUTTON_RIGHT].ID) << std::endl;
	std::cout << "  <inv_x>    " << (in.INVERT_X ? "true" : "false") << std::endl;
	std::cout << "  <inv_y>    " << (in.INVERT_Y ? "true" : "false") << std::endl;
}

void SaveJoystickConfig(ControlSetJoystick& in)
{
	std::ofstream file(configFiles[0], std::ios::out);
	if(file.is_open())
	{
		file << "<jump> "	<< sfJoystickCodeToString(in.BUTTONS[BUTTON_JUMP].ID) << "\n";
		file << "<use> "	<< sfJoystickCodeToString(in.BUTTONS[BUTTON_USE].ID) << "\n";
		file << "<inv_x> "	<< (in.INVERT_X ? "true" : "false") << "\n";
		file << "<inv_y> "	<< (in.INVERT_Y ? "true" : "false") << "\n";
		file.close();
	}
}

void SaveKeyboardConfig(ControlSetKeyboard& in)
{
	std::ofstream file(configFiles[1], std::ios::out);
	if(file.is_open())
	{
		file << "<jump> "	  << sfKeycodeToString(in.BUTTONS[BUTTON_JUMP].ID) << "\n";
		file << "<use> "	  << sfKeycodeToString(in.BUTTONS[BUTTON_USE].ID) << "\n";
		file << "<forward> "  << sfKeycodeToString(in.BUTTONS[BUTTON_FORWARD].ID) << "\n";
		file << "<backward> " << sfKeycodeToString(in.BUTTONS[BUTTON_BACKWARD].ID) << "\n";
		file << "<left> "	  << sfKeycodeToString(in.BUTTONS[BUTTON_LEFT].ID) << "\n";
		file << "<right> "	  << sfKeycodeToString(in.BUTTONS[BUTTON_RIGHT].ID) << "\n";
		file << "<inv_x> "    << (in.INVERT_X ? "true" : "false") << "\n";
		file << "<inv_y> "	  << (in.INVERT_Y ? "true" : "false") << "\n";
		file.close();
	}
}

Player::Player()
{
	ConfigJoystickFromFile(joystick);
	ConfigKeyboardFromFile(keyboard);
	controllerId = CONTROL_JOYSTICK;
	hitbox = new Collision::AABB(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
	packet = new Collision::CollisionPacket();
	packet->radius = glm::vec3(1.0f);
	camDist = 10.0f;
	collisionRecurDepth = 0;
	position.y = 10.0f;
}

Player::~Player()
{
	SaveJoystickConfig(joystick);
	SaveKeyboardConfig(keyboard);
	delete hitbox;
	delete packet;
}

void Player::LoadModel(ModelContainer* deposit)
{
	model = new AnimatedModel();
	BoneMesh* mesh = new BoneMesh();
	mesh->LoadMesh("Data/BoneMeshes/Worm.dae");
	model->SetMesh(mesh);
	model->AddColorTexture("Data/ModelTextures/Normal.png");
	model->AddNormalTexture("Data/ModelTextures/Normal.png");
	model->AddAnimation(0.0f, 1.0f, "default", true);
	model->SetAnimation("default");
	model->SetShading(MAPPED_DIFFUSE | MAPPED_NORMAL);
	deposit->AddAnimatedModel(model);
}

void Player::JoystickButtonPressed(const int& buttonId)
{
	if(controllerId != CONTROL_JOYSTICK) return;
	for(int i = 0; i < JOYSTICK_BUTTON_COUNT; i++)
		if(joystick.BUTTONS[i].ID == buttonId)
			joystick.BUTTONS[i].FLAGS = BUTTON_STATE_DOWN | BUTTON_STATE_JUST_DOWN;
}

void Player::JoystickButtonReleased(const int& buttonId)
{
	if(controllerId != CONTROL_JOYSTICK) return;
	for(int i = 0; i < JOYSTICK_BUTTON_COUNT; i++)
		if(joystick.BUTTONS[i].ID == buttonId)
			joystick.BUTTONS[i].FLAGS = BUTTON_STATE_JUST_UP;
}

void Player::JoystickMoved(const int& axis, const float& amount)
{
	if(controllerId != CONTROL_JOYSTICK) return;
	for(int i = 0; i < JOYSTICK_AXIS_COUNT; i++)
		if(joystick.AXES[i].ID  == axis)
			joystick.AXES[i].POSITION = amount;
}

void Player::KeyPressed(const int& key)
{
	if(controllerId != CONTROL_KEYBOARD_MOUSE)	return;
	for(int i = 0; i < KEYBOARD_BUTTON_COUNT; i++)
		if(keyboard.BUTTONS[i].ID == key)
			keyboard.BUTTONS[i].FLAGS = BUTTON_STATE_DOWN | BUTTON_STATE_JUST_DOWN;
}

void Player::KeyReleased(const int& key)
{
	if(controllerId != CONTROL_KEYBOARD_MOUSE)	return;
	for(int i = 0; i < KEYBOARD_BUTTON_COUNT; i++)
		if(keyboard.BUTTONS[i].ID == key)
			keyboard.BUTTONS[i].FLAGS = BUTTON_STATE_JUST_UP;
}

void Player::MousePressed(const int& button)
{
	if(controllerId != CONTROL_KEYBOARD_MOUSE)	return;
	if(sf::Mouse::Button::Left == button)	keyboard.MOUSE_LEFT_FLAGS = BUTTON_STATE_DOWN | BUTTON_STATE_JUST_DOWN;
	if(sf::Mouse::Button::Right == button)	keyboard.MOUSE_RIGHT_FLAGS = BUTTON_STATE_DOWN | BUTTON_STATE_JUST_DOWN;
}

void Player::MouseReleased(const int& button)
{
	if(controllerId != CONTROL_KEYBOARD_MOUSE)	return;
	if(sf::Mouse::Button::Left == button)	keyboard.MOUSE_LEFT_FLAGS = BUTTON_STATE_JUST_UP;
	if(sf::Mouse::Button::Right == button)	keyboard.MOUSE_RIGHT_FLAGS = BUTTON_STATE_JUST_UP;
}

void Player::MouseMoved(const int& x, const int& y)
{
	if(controllerId != CONTROL_KEYBOARD_MOUSE)	return;
	keyboard.MOUSEOLD = keyboard.MOUSE;
	keyboard.MOUSE = glm::vec2(x,y);
}

void Player::Update(const float& deltaTime, Collision::Terrain* terr, const glm::vec3& gravity)
{
	glm::vec3 targetMove;
	float dYaw, dPitch;
	UpdateInput(targetMove, dYaw, dPitch);
	UpdateCamera(dYaw, dPitch, deltaTime);
	UpdateCollisionObjects();

	float l = glm::length(targetMove);
	if(l > 0.0f)
		characterDir = -glm::normalize(targetMove*glm::mat3(glm::rotate(camYaw, glm::vec3(0.0f, 1.0f, 0.0f))));
	velocity = characterDir * l * 10.0f * deltaTime;

	packet->r3Pos = position;
	packet->r3Vel = velocity;
	glm::vec3 eSpacePosition = packet->r3Pos / packet->radius;
	glm::vec3 eSpaceVelocity = packet->r3Vel / packet->radius;
	collisionRecurDepth = 0;
	glm::vec3 finalPosition = CollideWithTerrain(terr, eSpacePosition, eSpaceVelocity);

	packet->r3Pos = finalPosition*packet->radius;
	packet->r3Vel = gravity * deltaTime;
	eSpaceVelocity = packet->r3Vel/packet->radius;
	collisionRecurDepth = 0;
	finalPosition = CollideWithTerrain(terr, finalPosition,eSpaceVelocity);

	position = finalPosition;

	UpdateModel();
	ResetButtonFlags();
}

void Player::UpdateInput(glm::vec3& targetMove, float& dYaw, float& dPitch)
{
	if(controllerId == CONTROL_JOYSTICK)
	{
		targetMove.x = int(joystick.AXES[AXIS_L_X].POSITION) / float(100.0f);
		targetMove.z = int(joystick.AXES[AXIS_L_Y].POSITION) / float(100.0f);
		float length = glm::length(targetMove);
		if(length > 1.0f)
			targetMove /= length;
		if(length < 0.1f)
			targetMove = glm::vec3(0.0f);
	
		dYaw = joystick.AXES[AXIS_R_X].POSITION;
		dPitch = joystick.AXES[AXIS_R_Y].POSITION;
		if(glm::abs(dYaw) < 5.0f)	dYaw = 0.0f;
		if(glm::abs(dPitch) < 5.0f) dPitch = 0.0f;

		jumpFlags = joystick.BUTTONS[BUTTON_JUMP].FLAGS;
		useFlags = joystick.BUTTONS[BUTTON_USE].FLAGS;
		invX = joystick.INVERT_X;
		invY = joystick.INVERT_Y;
	}
	else
	{
		if(keyboard.BUTTONS[BUTTON_LEFT].FLAGS & BUTTON_STATE_DOWN)		targetMove.x -= 1.0f;
		if(keyboard.BUTTONS[BUTTON_RIGHT].FLAGS & BUTTON_STATE_DOWN)	targetMove.x += 1.0f;
		if(keyboard.BUTTONS[BUTTON_FORWARD].FLAGS & BUTTON_STATE_DOWN)	targetMove.z += 1.0f;
		if(keyboard.BUTTONS[BUTTON_BACKWARD].FLAGS & BUTTON_STATE_DOWN)	targetMove.z -= 1.0f;
		float length = glm::length(targetMove);
		if(length > 1.0f)
			targetMove /= length;

		glm::vec2 dMouse = keyboard.MOUSE - keyboard.MOUSEOLD;
		dYaw = dMouse.x * 20.0f;
		dPitch = dMouse.y * 20.0f;

		jumpFlags = keyboard.BUTTONS[BUTTON_JUMP].FLAGS;
		useFlags = keyboard.BUTTONS[BUTTON_USE].FLAGS;
		invX = keyboard.INVERT_X;
		invY = keyboard.INVERT_Y;
	}
}

void Player::UpdateCollisionObjects()
{
	hitbox->x[0] = position.x - 1.5f;
	hitbox->x[1] = position.x + 1.5f;
	hitbox->y[0] = position.y - 1.5f;
	hitbox->y[1] = position.y + 1.5f;
	hitbox->z[0] = position.z - 1.5f;
	hitbox->z[1] = position.z + 1.5f;
}

void Player::UpdateCamera(const float& dYaw, const float& dPitch, const float& deltaTime)
{
	if(invX) camYaw -= dYaw * 1.5f * deltaTime;
	else	 camYaw += dYaw * 1.5f * deltaTime;
	if(invY) camPitch -= dPitch * 1.5f * deltaTime;
	else	 camPitch += dPitch * 1.5f * deltaTime;

	if(camYaw >= 360.0f) camYaw = 0.0f;
	if(camYaw <=-360.0f) camYaw = 0.0f;
	if(camPitch < -60.0f) camPitch =-60.0f;
	if(camPitch > 35.0f) camPitch = 35.0f;

	glm::mat4 rotation = glm::rotate(camPitch, glm::vec3(1.0f, 0.0f, 0.0f));
	rotation *= glm::rotate(camYaw, glm::vec3(0.0f, 1.0f, 0.0f));
	camPosition = glm::vec3(0.0f, 0.0f, -camDist) * glm::mat3(rotation) + position;
}

glm::vec3 Player::CollideWithTerrain(Collision::Terrain* terr, const glm::vec3& pos, const glm::vec3& vel)
{
	float veryCloseDistance = 0.005f;
	if (collisionRecurDepth>5)	return pos;
	packet->eVel = vel;
	packet->eVelNormal = glm::normalize(vel);
	packet->eBasePoint = pos;
	packet->foundCollision = false;

	std::vector<Collision::Triangle> culledTriangles;
	terr->QuereyAABB(*hitbox, culledTriangles);
	for(std::size_t i = 0; i < culledTriangles.size(); i++)
		Collision::CheckTriangle(packet, culledTriangles[i].verts[0], culledTriangles[i].verts[1], culledTriangles[i].verts[2]);

	if(!packet->foundCollision)
		return pos + vel;

	glm::vec3 destinationPoint = pos + vel;
	glm::vec3 newBasePoint = pos;

	if(packet->nearestDistance >= veryCloseDistance)
	{
		glm::vec3 v = glm::normalize(vel);
		newBasePoint = packet->eBasePoint + v * float(packet->nearestDistance-veryCloseDistance);
		packet->contactPoint -= veryCloseDistance * v;
	}
	glm::vec3 slidePlaneOrigin = packet->contactPoint;
	glm::vec3 slidePlaneNormal = glm::normalize(newBasePoint - packet->contactPoint);
	Collision::Plane slidePlane(slidePlaneOrigin, slidePlaneNormal);
	glm::vec3 newDestinationPoint = destinationPoint - float(slidePlane.SignedDistanceTo(destinationPoint)) * slidePlaneNormal;
	glm::vec3 newVelocity = newDestinationPoint - packet->contactPoint;
	if (glm::length(newVelocity) < veryCloseDistance) return newBasePoint;
	collisionRecurDepth++;
	return CollideWithTerrain(terr, newBasePoint, newVelocity);
}

void Player::UpdateModel()
{
	model->GetMatrix() = glm::translate(position);
}

void Player::ResetButtonFlags()
{
	int i;
	for(i = 0; i < KEYBOARD_BUTTON_COUNT; i++)
	{
		if(keyboard.BUTTONS[i].FLAGS & BUTTON_STATE_JUST_DOWN)	keyboard.BUTTONS[i].FLAGS = keyboard.BUTTONS[i].FLAGS ^ BUTTON_STATE_JUST_DOWN;
		if(keyboard.BUTTONS[i].FLAGS & BUTTON_STATE_JUST_UP)	keyboard.BUTTONS[i].FLAGS = 0x00;
	}
	if(keyboard.MOUSE_LEFT_FLAGS & BUTTON_STATE_JUST_DOWN)	keyboard.MOUSE_LEFT_FLAGS = keyboard.MOUSE_LEFT_FLAGS ^ BUTTON_STATE_JUST_DOWN;
	if(keyboard.MOUSE_RIGHT_FLAGS & BUTTON_STATE_JUST_DOWN)	keyboard.MOUSE_RIGHT_FLAGS = keyboard.MOUSE_RIGHT_FLAGS ^ BUTTON_STATE_JUST_DOWN;
	if(keyboard.MOUSE_LEFT_FLAGS & BUTTON_STATE_JUST_UP)	keyboard.MOUSE_LEFT_FLAGS = 0x00;
	if(keyboard.MOUSE_RIGHT_FLAGS & BUTTON_STATE_JUST_UP)	keyboard.MOUSE_RIGHT_FLAGS = 0x00;
	keyboard.MOUSEOLD = keyboard.MOUSE;
	for(i = 0; i < JOYSTICK_BUTTON_COUNT; i++)
	{
		if(joystick.BUTTONS[i].FLAGS & BUTTON_STATE_JUST_DOWN)	joystick.BUTTONS[i].FLAGS = joystick.BUTTONS[i].FLAGS ^ BUTTON_STATE_JUST_DOWN;
		if(joystick.BUTTONS[i].FLAGS & BUTTON_STATE_JUST_UP)	joystick.BUTTONS[i].FLAGS = 0x00;
	}
}