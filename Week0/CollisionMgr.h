#pragma once
class UBall;
class UPlayer;
class CollisionMgr
{
public:
	static void CollisionPlayerAndBall(UPlayer* _pPlayer, UBall* _pBall);

private:
	static bool CheckCollision(UPlayer* _pPlayer, UBall* _pBall);
};

