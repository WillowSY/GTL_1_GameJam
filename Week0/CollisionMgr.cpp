#pragma once
#include <Windows.h>
#include "CollisionMgr.h"
#include "Define.h"
#include "Player.h"
#include "Ball.h"
#include "Object.h"
#include "Level.h"
using namespace std;
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

void CollisionMgr::CollisionUIAndBall(UObject* _pSrc, UObject* _pDst) {
    
    ULevel* pLevel = static_cast<ULevel*>(_pSrc);
    if (CheckCollisionUIAndCircle(_pSrc, _pDst)) {
        pLevel->BeginOverllaped(_pDst);
        _pDst->BeginOverllaped(_pSrc);
     
    }
}
float clamp(float value, float minVal, float maxVal)
{
    return max(minVal, min(value, maxVal));
}

bool CollisionMgr::CheckCollisionBoxAndCircle(UObject* _pSrc, UObject* _pDst)
{
    UPlayer* _pPlayer = static_cast<UPlayer*>(_pSrc);
    UBall* _pBall = static_cast<UBall*>(_pDst);
    float closestX = fmax(_pPlayer->GetLoc().x - _pPlayer->GetScale(), fmin(_pBall->GetLoc().x, _pPlayer->GetLoc().x + _pPlayer->GetScale()));
    float closestY = fmax(_pPlayer->GetLoc().y - _pPlayer->GetScale(), fmin(_pBall->GetLoc().y, _pPlayer->GetLoc().y + _pPlayer->GetScale()));


    // 원의 중심과 가장 가까운 점 사이의 거리 계산
    float distanceX = _pBall->GetLoc().x - closestX;
    float distanceY = _pBall->GetLoc().y - closestY;

    // 충돌이 발생했는지 확인 (거리가 원의 반지름보다 작거나 같으면 충돌)
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

bool CollisionMgr::CheckCollisionUIAndCircle(UObject* _pSrc, UObject* _pDst)
{
    ULevel* _pLevel = static_cast<ULevel*>(_pSrc);
    UBall* _pBall = static_cast<UBall*>(_pDst);
    float closestX = fmax(_pSrc->GetLoc().x - _pSrc->GetScale().x, fmin(_pBall->GetLoc().x, _pSrc->GetLoc().x + _pSrc->GetScale().x));
    float closestY = fmax(_pSrc->GetLoc().y - _pSrc->GetScale().y, fmin(_pBall->GetLoc().y, _pSrc->GetLoc().y + _pSrc->GetScale().y));
   
    // 원의 중심과 가장 가까운 점 사이의 거리 계산
    float distanceX = _pBall->GetLoc().x - closestX;
    float distanceY = _pBall->GetLoc().y - closestY;

    // 충돌이 발생했는지 확인 (거리가 원의 반지름보다 작거나 같으면 충돌)
    float distanceSquared = distanceX * distanceX + distanceY * distanceY;
    return distanceSquared <= (_pBall->Radius * _pBall->Radius);

}