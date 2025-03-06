#include "GameMode.h"
#include "Windows.h"
#include <Xinput.h>

#pragma comment(lib, "Xinput.lib")
CGameMode::CGameMode()
{
}
CGameMode::~CGameMode()
{
}

void CGameMode::Initialize()
{
	stage = 0;
	score = 0;
	bGameOver = false;
	bHasInit = false;
	bGameStart = false;
	bool bStageClear = false;
	bTryAgain = false;
	m_fStageTime = 0.0f;
}

void CGameMode::Update(float deltaTime)
{
	XINPUT_STATE state;
	bool gamepadConnected = (XInputGetState(0, &state) == ERROR_SUCCESS);
	if (!bGameStart)
	{
		if (GetAsyncKeyState('R') & 0x8000 || (gamepadConnected && (state.Gamepad.wButtons & XINPUT_GAMEPAD_A)))
		{
			bGameStart = true;
		}
	}
	if (bGameOver)
	{
		highScore = (score > highScore) ? score : highScore;
		// Game over logic
		ZeroMemory(&state, sizeof(XINPUT_STATE));
		if (GetAsyncKeyState('R') & 0x8000|| (gamepadConnected && (state.Gamepad.wButtons & XINPUT_GAMEPAD_A)))
		{
			bTryAgain = true;
		}
		if (bTryAgain)
		{
			Initialize();
		}
	}
	else
	{
		// Game logic
		m_fStageTime += deltaTime;
		if (bStageClear)
		{
			stage++;
			bHasInit = false;
			bStageClear = false;
		}
	}
}

void CGameMode::AddScore(int _Add)
{
	score += _Add;
}
