#include "GameMode.h"
#include "Windows.h"

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
	bTryAgain = false;
	m_fStageTime = 0.0f;
}

void CGameMode::Update(float deltaTime)
{
	if (bGameOver)
	{
		highScore = (score > highScore) ? score : highScore;
		// Game over logic
		if (GetAsyncKeyState('R') & 0x8000)
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