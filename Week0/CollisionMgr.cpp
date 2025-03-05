#pragma once
#include "CollisionMgr.h"
#include "Define.h"
#include "Player.h"
#include "Ball.h"
#include "Object.h"
void CollisionMgr::CollisionPlayerAndBall(UObject* _pPlayer, UObject* _pBall)
{
    if (CheckCollisionBoxAndCircle(_pPlayer, _pBall))
    {
        _pPlayer->BeginOverllaped(_pBall);
        _pBall->BeginOverllaped(_pPlayer);
    }
}

void CollisionMgr::CollisionBallAndBall(UObject* _pSrc, UObject* _pDst)
{
    if (CheckCollisionBallAndBall(_pSrc, _pDst) )
    {
        _pSrc->BeginOverllaped(_pDst);
        _pDst->BeginOverllaped(_pSrc);
    }
}

float clamp(float value, float minVal, float maxVal)
{
    return std::max(minVal, std::min(value, maxVal));
}

bool CollisionMgr::CheckCollisionBoxAndCircle(UObject* _pSrc, UObject* _pDst)
{
    UPlayer* _pPlayer = static_cast<UPlayer*>(_pSrc);
    UBall* _pBall = static_cast<UBall*>(_pDst);
    float closestX = fmax(_pPlayer->GetLoc().x - _pPlayer->GetScale(), fmin(_pBall->GetLoc().x, _pPlayer->GetLoc().x + _pPlayer->GetScale()));
    float closestY = fmax(_pPlayer->GetLoc().y - _pPlayer->GetScale(), fmin(_pBall->GetLoc().y, _pPlayer->GetLoc().y + _pPlayer->GetScale()));


    // ���� �߽ɰ� ���� ����� �� ������ �Ÿ� ���
    float distanceX = _pBall->GetLoc().x - closestX;
    float distanceY = _pBall->GetLoc().y - closestY;

    // �浹�� �߻��ߴ��� Ȯ�� (�Ÿ��� ���� ���������� �۰ų� ������ �浹)
    float distanceSquared = distanceX * distanceX + distanceY * distanceY;
    return distanceSquared <= (_pBall->Radius * _pBall->Radius);
}

bool CollisionMgr::CheckCollisionBallAndBall(UObject* _pSrc, UObject* _pDst)
{
    UBall* pSrc = static_cast<UBall*>(_pSrc);
    UBall* pDst = static_cast<UBall*>(_pDst);

    float distance = (pSrc->GetLoc() - pDst->GetLoc()).Magnitude();
    float radiusSum = pSrc->Radius + pDst->Radius;
    return distance <= radiusSum;
}
