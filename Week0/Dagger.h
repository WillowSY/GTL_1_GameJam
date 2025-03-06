#pragma once
#include "Object.h"
#include "Define.h"
class UDagger : public UObject
{
	public:
		UDagger();
		UDagger(FVector3 _Loc, FVector3 _Vel);
		virtual ~UDagger();

		virtual void Update(float deltaTime) override; // 업데이트 함수
		virtual void BeginOverllaped(UObject* _pOther) override;
	public:
		void SetInstigator(OBJECTLIST _Instigator);

	private:

		const float m_Scale = 0.01f;

		float AngularVelocity = 0.01f;
		float RotationAngle = 0.1f;

		bool m_bDead = false;
		OBJECTLIST m_Instigator;
	private:
		void Move(float deltaTime); // 이동 함수
		void Rotate(float deltaTime); // 회전 함수
	public:
		float		GetScale() { return m_Scale; }
		OBJECTLIST	GetIsntigator() { return m_Instigator; }
		bool		IsDead() { return m_bDead; }
};

