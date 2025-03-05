#pragma once
#include "Object.h"
#include "Define.h"
class UDagger : public UObject
{
	public:
		UDagger();
		UDagger(FVector3 _Loc, FVector3 _Vel);
		virtual ~UDagger();

		virtual void Update(float deltaTime) override; // ������Ʈ �Լ�
		virtual void BeginOverllaped(UObject* _pOther) override;
	public:
		// Ŭ���� �̸���, �Ʒ� �ΰ��� ���� �̸��� �������� �ʽ��ϴ�.


	private:
		FVector3 m_Loc;
		FVector3 m_Velocity;
		const float m_Scale = 0.01f;

		float AngularVelocity = 0.01f;
		float RotationAngle = 0.1f;

		bool m_bDead = false;

	private:
		void Move(float deltaTime); // �̵� �Լ�
		void Rotate(float deltaTime); // ȸ�� �Լ�

	public:
		FVector3	GetLoc() { return m_Loc; }
		float		GetScale() { return m_Scale; }
		bool		IsDead() { return m_bDead; }
};

