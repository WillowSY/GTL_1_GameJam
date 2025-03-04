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
	bool bStageClear = false;

public:
	void Initialize();
	void Update(float deltaTime);
	float GetStageTime() { return m_fStageTime; }

private:
	float m_fStageTime = 0.0f;
};