#pragma once
#include "CollisionMgr.h"
#include "Define.h"
#include "Player.h"
#include "Ball.h"
#include "Object.h"
void CollisionMgr::CollisionPlayerAndBall(UObject* _pPlayer, UObject* _pBall)
{
    if (CheckCollision(_pPlayer, _pBall))
    {
        _pPlayer->BeginOverllaped(_pBall);
        _pBall->BeginOverllaped(_pPlayer);
    }
}

float clamp(float value, float minVal, float maxVal)
{
    return std::max(minVal, std::min(value, maxVal));
}

bool CollisionMgr::CheckCollision(UObject* _pSrc, UObject* _pDst)
{
    UPlayer* _pPlayer = static_cast<UPlayer*>(_pSrc);
    UBall* _pBall = static_cast<UBall*>(_pDst);
    float closestX = fmax(_pPlayer->GetLoc().x - _pPlayer->GetScale(), fmin(_pBall->Location.x, _pPlayer->GetLoc().x + _pPlayer->GetScale()));
    float closestY = fmax(_pPlayer->GetLoc().y - _pPlayer->GetScale(), fmin(_pBall->Location.y, _pPlayer->GetLoc().y + _pPlayer->GetScale()));


    // ���� �߽ɰ� ���� ����� �� ������ �Ÿ� ���
    float distanceX = _pBall->Location.x - closestX;
    float distanceY = _pBall->Location.y - closestY;

    // �浹�� �߻��ߴ��� Ȯ�� (�Ÿ��� ���� ���������� �۰ų� ������ �浹)
    float distanceSquared = distanceX * distanceX + distanceY * distanceY;
    return distanceSquared <= (_pBall->Radius * _pBall->Radius);
}
