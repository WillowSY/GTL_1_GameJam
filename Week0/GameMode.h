#pragma once
class CGameMode
{
public:
	CGameMode();
	~CGameMode();

public:
	int stage = 0;
	int score = 0;
	int highScore = 0;
	bool bGameOver = false;
	bool bHasInit = false;
	bool bGameStart = false;
	bool bStageClear = false;
	bool bTryAgain = false;

public:
	void	Initialize();
	void	Update(float deltaTime);
	float	GetStageTime() { return m_fStageTime; }
	void	AddScore(int _Add);
private:
	float m_fStageTime = 0.0f;
};