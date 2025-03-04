#pragma once
class UObject;
class CollisionMgr
{
public:
	static void CollisionPlayerAndBall(UObject* _pPlayer, UObject* _pBall);

private:
	static bool CheckCollision(UObject* _pPlayer, UObject* _pBall);
};

