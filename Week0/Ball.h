#pragma once
#include "Object.h"
#include "Define.h"


class UBall : public UObject
{
public:
	UBall();
	virtual ~UBall();

	virtual void Update(float deltaTime) override; // ������Ʈ �Լ�
	virtual void BeginOverllaped(UObject* _pOther) override;
public:
	// Ŭ���� �̸���, �Ʒ� �ΰ��� ���� �̸��� �������� �ʽ��ϴ�.
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

	bool CreateBall(); // ���ο� �� ����
	void DeleteRandomBall(); // ���� ��ġ�� �� �����
	void DeleteBall(); // ���� �� �����
	
private:
	const float sphereRadius = 1.0f;
	const float ballSpeed = 0.000005f;
	float scaleMod = 0.1f;
	const float leftBorder = -1.0f;
	const float rightBorder = 1.0f;
	const float topBorder = -1.0f;
	const float bottomBorder = 1.0f;

private:

	void ResolveOverlap(FVector3& pos1, FVector3& pos2, float penetrationDepth); // ������ �� ����߸���
	void BoundaryHandle(); // ȭ�� ��� Ȯ�� �Լ�
	void CollisionHandle(UObject* _pOther); // �浹 ó�� �Լ�
	void Move(float deltaTime); // �̵� �Լ�
	void Rotate(float deltaTime); // ȸ�� �Լ�
};