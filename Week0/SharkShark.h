#pragma once
#include "Define.h"

class UObject;
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

public:
	std::vector<std::list<UObject*>>& GetpObejectList() { return m_pObjectList; }
	std::list<UObject*>& GetBallList() { return m_pObjectList[OL_BALL]; }
	std::list<UObject*>& GetDaggerList() { return m_pObjectList[OL_DAGGER]; }
	std::list<UObject*>& GetUIList() { return m_pObjectList[OL_UI]; }
	UObject* GetPlayer() { return m_pObjectList[OL_PLAYER].front(); }
};

