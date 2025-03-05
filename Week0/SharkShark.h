#pragma once
#include "Define.h"
class UObject;
class CGameMode;
class ULeaderboard;
class SharkShark
{
public:
	SharkShark();
	~SharkShark();

	void Initialize();
	void Update(float deltaTime);
	void Release();
	void Render();
	void FixedUpdate();
	void DeleteRandomBall(int& ballCount);
	bool CreateBall();
private:
	std::vector<std::list<UObject*>> m_pObjectList;
	CGameMode* m_pMyGameMode = nullptr;
	ULeaderboard* m_pLeaderBoard = nullptr;
public:
	std::vector<std::list<UObject*>>& GetpObejectList() { return m_pObjectList; }
	std::list<UObject*>& GetBallList() { return m_pObjectList[OL_BALL]; }
	std::list<UObject*>& GetDaggerList() { return m_pObjectList[OL_DAGGER]; }
	UObject* GetPlayer() { return m_pObjectList[OL_PLAYER].front(); }
	CGameMode* GetGameMode() { return m_pMyGameMode; }
	ULeaderboard* GetLeaderboard() { return m_pLeaderBoard; }
};

