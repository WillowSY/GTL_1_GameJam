#pragma once
#include "Object.h"
#include "Define.h"


class UBall : public UObject
{
public:
	UBall();
	virtual ~UBall();

	virtual void Update(float deltaTime) override; // 업데이트 함수
	virtual void BeginOverllaped(UObject* _pOther) override;
public:
	// 클래스 이름과, 아래 두개의 변수 이름은 변경하지 않습니다.
	FVector3 Location;
	FVector3 Velocity;
	float Radius;
	float Mass;
	float AngularVelocity = 0.01f;
	float RotationAngle = 0.1f;

	UBall* NextBall;
	UBall* PrevBall;
	static int ballCount;

	float Index = 0;

	bool bDead = false;

	bool CreateBall(); // 새로운 공 생성
	void DeleteRandomBall(); // 랜덤 위치의 공 지우기
	void DeleteBall(); // 지금 공 지우기
	
private:
	const float sphereRadius = 1.0f;
	const float ballSpeed = 0.000005f;
	float scaleMod = 0.1f;
	const float leftBorder = -1.0f;
	const float rightBorder = 1.0f;
	const float topBorder = -1.0f;
	const float bottomBorder = 1.0f;

private:

	void ResolveOverlap(FVector3& pos1, FVector3& pos2, float penetrationDepth); // 겹쳤을 때 떨어뜨리기
	void BoundaryHandle(); // 화면 경계 확인 함수
	void CollisionHandle(UObject* _pOther); // 충돌 처리 함수
	void Move(float deltaTime); // 이동 함수
	void Rotate(float deltaTime); // 회전 함수
};