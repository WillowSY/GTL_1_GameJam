#include "SharkShark.h"
#include "Player.h"
#include "Ball.h"
#include "Dagger.h"
#include "CollisionMgr.h"

const float sphereRadius = 1.0f;
const float ballSpeed = 0.000005f;
float scaleMod = 0.1f;

SharkShark::SharkShark()
{
}

SharkShark::~SharkShark()
{
	Release();
}

void SharkShark::Initialize()
{
	for (int i = 0;i < OL_END;i++)
	{
		m_pObjectList.push_back(std::list<UObject*>());
	}
	UObject* pPlayer = new UPlayer;
	static_cast<UPlayer*>(pPlayer)->Initialize();
	static_cast<UPlayer*>(pPlayer)->SetMainGame(this);
	m_pObjectList[OL_PLAYER].push_back(pPlayer);
}

void SharkShark::Update(float deltaTime)
{
	for (auto iter = m_pObjectList.begin(); iter != m_pObjectList.end(); iter++)
	{
		for (auto iter1 = (*iter).begin(); iter1 != (*iter).end(); iter1++)
		{
			(*iter1)->Update(deltaTime);
		}
	}

}

void SharkShark::Release()
{
	for (auto iter = m_pObjectList.begin(); iter != m_pObjectList.end(); iter++)
	{
		for (auto iter1 = (*iter).begin(); iter1 != (*iter).end(); iter1++)
		{
			delete *iter1;
		}
	}
}

void SharkShark::Render()
{
}

void SharkShark::FixedUpdate()
{
	for (auto iter = GetBallList().begin(); iter != GetBallList().end();) {
		CollisionMgr::CollisionPlayerAndBall(GetPlayer(), *iter);
		if (static_cast<UBall*>(*iter)->bDead)
		{
			if (!static_cast<UPlayer*>(GetPlayer())->IsDragonBlading())
				static_cast<UPlayer*>(GetPlayer())->AddDragonBladeGage(1.0f);
			else
				static_cast<UPlayer*>(GetPlayer())->DashReset();
			delete* iter;
			iter = GetBallList().erase(iter);
		}
		else
			iter++;
	}
	for (auto iter = GetDaggerList().begin(); iter != GetDaggerList().end();iter++) {
		CollisionMgr::CollisionPlayerAndBall(GetPlayer(), *iter);
	}
	for (auto iter = GetBallList().begin(); iter != GetBallList().end();iter++) {
		for (auto iter2 = iter; iter2 != GetBallList().end(); iter2++)
		{
			CollisionMgr::CollisionBallAndBall((*iter), (*iter2));
		}
	}
	for (auto iter = GetBallList().begin(); iter != GetBallList().end();iter++) {
		for (auto iter2 = GetDaggerList().begin(); iter2 != GetDaggerList().end(); iter2++)
		{
			CollisionMgr::CollisionBallAndBall((*iter), (*iter2));
		}
	}
	for (auto iter = GetDaggerList().begin(); iter != GetDaggerList().end();) {
		if (static_cast<UDagger*>(*iter)->IsDead())
		{
			delete* iter;
			iter = GetDaggerList().erase(iter);
		}
		else
			iter++;
	}
	for (auto iter = GetUIList().begin(); iter != GetUIList().end();) {
		for (auto iter2 = GetBallList().begin(); iter != GetBallList().end();) {
			CollisionMgr::CollisionUIAndBall((*iter), (*iter2));
		}
	}

}

void SharkShark::DeleteRandomBall(int& ballCount)
{
	int deleteIdx = (rand() % GetBallList().size()) + 1;
	int count = 1;
	auto iter = GetBallList().begin();
	while (*iter)
	{
		if (count == deleteIdx)
		{
			delete (*iter);
			iter = GetBallList().erase(iter);
			break;
		}
		count++;
	}
}

bool SharkShark::CreateBall()
{
	UObject* PossibleBall = new UBall;
	// 생성할 위치 초기화
	FVector3 newLocation;
	float newRadius = (sphereRadius * scaleMod) * (1.f - ((rand() % 1001) / 1000.0) * 0.9);
	bool locationValid = false;

	// 최대 시도 횟수 (너무 많은 시도가 되지 않도록 제한)
	int maxAttempts = 10;
	int attempts = 0;

	//생성 가능 여부 확인
	while (!locationValid && attempts < maxAttempts)
	{
		newLocation = FVector3(((rand() % 2000) / 1000.0f) - 1.0f, ((rand() % 2000) / 1000.0f) - 1.0f, 0.f);


		locationValid = true;
		for (auto iter = m_pObjectList[OL_BALL].begin(); iter != m_pObjectList[OL_BALL].end(); iter++)
		{
			float distance = (newLocation - static_cast<UBall*>(*iter)->GetLoc()).Magnitude();
			float radiusSum = newRadius + static_cast<UBall*>(*iter)->Radius;
			if (distance < radiusSum)  // 겹침 발생
			{
				locationValid = false; // 겹침이 있으면 유효하지 않음
				break;
			}
		}

		attempts++;
	}
	// 생성 가능 위치를 찾았다면 생성
	if (locationValid)
	{
		static_cast<UBall*>(PossibleBall)->SetLoc(newLocation);
		static_cast<UBall*>(PossibleBall)->SetVel(FVector3(((float)(rand() % 100 - 50)) * ballSpeed, ((float)(rand() % 100 - 50)) * ballSpeed, 0.0f));
		static_cast<UBall*>(PossibleBall)->Radius = (sphereRadius * scaleMod) * (1.f - ((rand() % 1001) / 1000.0) * 0.9);
		static_cast<UBall*>(PossibleBall)->Mass = static_cast<UBall*>(PossibleBall)->Radius * 100.0f;
		static_cast<UBall*>(PossibleBall)->SetMainGame(this);
		m_pObjectList[OL_BALL].push_back(PossibleBall);
		return true;
	}
	else
	{
		delete PossibleBall;
		return false;
	}
}

void  SharkShark::CreateUI(int index, FVector3 pos, FVector3 scale, FVector3 rot) {
	UObject* NewObj = new UObject;
	NewObj->SetIndex(index);
	NewObj->SetLoc(pos);
	NewObj->SetRot(rot);
	NewObj->SetScale(scale);
	m_pObjectList[OL_UI].push_back(NewObj);
}
