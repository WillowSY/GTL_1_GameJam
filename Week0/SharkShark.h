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
	void CreateUI(int index, FVector3 pos, FVector3 rot, FVector3 scale);
	void DeleteAllUI();
private:
	std::vector<std::list<UObject*>> m_pObjectList;
	CGameMode* m_pMyGameMode = nullptr;
	ULeaderboard* m_pLeaderBoard = nullptr;
public:
	std::vector<std::list<UObject*>>& GetpObejectList() { return m_pObjectList; }
	std::list<UObject*>& GetBallList() { return m_pObjectList[OL_BALL]; }
	std::list<UObject*>& GetDaggerList() { return m_pObjectList[OL_DAGGER]; }
	std::list<UObject*>& GetUIList() { return m_pObjectList[OL_UI]; }
	UObject* GetPlayer() { return m_pObjectList[OL_PLAYER].front(); }
	CGameMode* GetGameMode() { return m_pMyGameMode; }
	ULeaderboard* GetLeaderboard() { return m_pLeaderBoard; }
};

