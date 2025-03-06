#include <Windows.h>
#include "Player.h"
#include "SharkShark.h"
#include "Level.h"
#include "Ball.h"
ULevel::ULevel() : UObject(OL_UI)
{
}

ULevel::~ULevel()
{
}

void ULevel::BeginOverllaped(UObject* _pOther) {
    //MessageBoxW(nullptr, L"Begin1", L"오류", MB_OK | MB_ICONERROR);
	UBall* pBall = static_cast<UBall*>(_pOther);
	if (pBall) {
        //MessageBoxW(nullptr, L"Begin2", L"오류", MB_OK | MB_ICONERROR);
		CollisionHandle(_pOther);
	}

}

void ULevel::CollisionHandle(UObject* _pOther) {
    UBall* pOther = static_cast<UBall*>(_pOther);

    FVector3 minV, maxV, normalV(0, 0, 0);
    minV.x = m_Loc.x - m_Scale.x;
    minV.y = m_Loc.y - m_Scale.y;
    maxV.x = m_Loc.x + m_Scale.x;
    maxV.y = m_Loc.y + m_Scale.y;

    FVector3 ballPos = pOther->GetLoc();
    float ballRadius = pOther->Radius;  // 공의 반지름을 가져오는 함수가 필요함.

    // 왼쪽 벽 충돌
    if (ballPos.x - ballRadius < minV.x) normalV.x = -1;
    // 오른쪽 벽 충돌
    else if (ballPos.x + ballRadius > maxV.x) normalV.x = 1;

    // 아래 벽 충돌
    if (ballPos.y - ballRadius < minV.y) normalV.y = -1;
    // 위쪽 벽 충돌
    else if (ballPos.y + ballRadius > maxV.y) normalV.y = 1;

    if (normalV.x != 0 || normalV.y != 0) {
        FVector3 velocity = pOther->GetVelocity();
        FVector3 reflectedVelocity = velocity - normalV * (2 * velocity.Dot(normalV));
        pOther->RestrictVel(reflectedVelocity);
    }
}

