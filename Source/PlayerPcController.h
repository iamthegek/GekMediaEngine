/*
LocalPlayerPc - Child of LocalPlayer

This class represents a player controlled by Keyboard and
Mouse. All keys are rebindable, except the crosshair/look
direction will always be determined by mouse position.
*/

#ifndef __LOCPLAYERPC__H
#define __LOCPLAYERPC__H

#include "PlayerController.h"
#include "Input.h"

class PlayerPcController : public PlayerController
{
public:
	enum PcControls
	{
		X_POSITIVE,		//0
		X_NEGATIVE,		//1
		Y_POSITIVE,		//2
		Y_NEGATIVE,		//3
		SHOOT,
		NUM_KEYS
	};

	PlayerPcController();
	~PlayerPcController();

	void AttachToPc(KeyboardMouseTracker* p);
	void Rebind(int control, int newKeyIndex);
	bool GetControlState(const int& control);

	void Update(const float& deltaTime);

private:
	KeyboardMouseTracker * controller;

	int keyIndices[NUM_KEYS];
	int keyFlags[NUM_KEYS];
};

#endif