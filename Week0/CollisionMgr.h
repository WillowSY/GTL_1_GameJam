#pragma once
class UObject;
class CollisionMgr
{
public:
	static void CollisionPlayerAndBall(UObject* _pPlayer, UObject* _pBall);
	static void CollisionBallAndBall(UObject* _pSrc, UObject* _pDst);
private:
	static bool CheckCollisionBoxAndCircle(UObject* _pSrc, UObject* _pDst);
	static bool CheckCollisionBallAndBall(UObject* _pSrc, UObject* _pDst);
};

