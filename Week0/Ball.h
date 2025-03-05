#pragma once
#include "Object.h"
#include "Define.h"

class SharkShark;
class UBall : public UObject
{
public:
	UBall();
	virtual ~UBall();

	virtual void Update(float deltaTime) override; // 업데이트 함수
	virtual void BeginOverllaped(UObject* _pOther) override;
	
	void		SetMainGame(SharkShark* _pMainGame);
	void		Attack();
public:
	// 클래스 이름과, 아래 두개의 변수 이름은 변경하지 않습니다.
	float Radius;
	float Mass;
	float AngularVelocity = 0.01f;
	float RotationAngle = 0.1f;

	bool bDead = false;
	
	SharkShark* m_pMainGame;
private:
	const float sphereRadius = 1.0f;
	const float ballSpeed = 0.000005f;
	float scaleMod = 0.1f;
	const float leftBorder = -1.0f;
	const float rightBorder = 1.0f;
	const float topBorder = -1.0f;
	const float bottomBorder = 1.0f;

	float AttackCDT = 5.0f;
	float AttackTimer = 5.0f;
private:

	void ResolveOverlap(FVector3& pos1, FVector3& pos2, float penetrationDepth); // 겹쳤을 때 떨어뜨리기
	void BoundaryHandle(); // 화면 경계 확인 함수
	void CollisionHandle(UObject* _pOther); // 충돌 처리 함수
	void Move(float deltaTime); // 이동 함수
	void Rotate(float deltaTime); // 회전 함수
};