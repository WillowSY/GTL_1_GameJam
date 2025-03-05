#pragma once
#include <string>
class UObject;
class CollisionMgr
{
public:
	static void CollisionPlayerAndBall(UObject* _pPlayer, UObject* _pBall);
	static void CollisionBallAndBall(UObject* _pSrc, UObject* _pDst);
	static void CollisionUIAndBall(UObject* _pBox, UObject* _pBall);
private:
	static bool CheckCollisionBoxAndCircle(UObject* _pSrc, UObject* _pDst);
	static bool CheckCollisionBallAndBall(UObject* _pSrc, UObject* _pDst);
	static bool CheckCollisionUIAndCircle(UObject* _pSrc, UObject* _pDst);
};
