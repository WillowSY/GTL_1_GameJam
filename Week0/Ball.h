#pragma once
#include "Object.h"
#include "Define.h"

class SharkShark;
class UBall : public UObject
{
public:
	UBall();
	virtual ~UBall();

	virtual void Update(float deltaTime) override; // ������Ʈ �Լ�
	virtual void BeginOverllaped(UObject* _pOther) override;
	
	void		SetMainGame(SharkShark* _pMainGame);
	void		Attack();
public:
	// Ŭ���� �̸���, �Ʒ� �ΰ��� ���� �̸��� �������� �ʽ��ϴ�.
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

	void ResolveOverlap(FVector3& pos1, FVector3& pos2, float penetrationDepth); // ������ �� ����߸���
	void BoundaryHandle(); // ȭ�� ��� Ȯ�� �Լ�
	void CollisionHandle(UObject* _pOther); // �浹 ó�� �Լ�
	void Move(float deltaTime); // �̵� �Լ�
	void Rotate(float deltaTime); // ȸ�� �Լ�
};