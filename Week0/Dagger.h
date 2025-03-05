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
		// 클래스 이름과, 아래 두개의 변수 이름은 변경하지 않습니다.


	private:
		FVector3 m_Loc;
		FVector3 m_Velocity;
		const float m_Scale = 0.01f;

		float AngularVelocity = 0.01f;
		float RotationAngle = 0.1f;

		bool m_bDead = false;

	private:
		void Move(float deltaTime); // 이동 함수
		void Rotate(float deltaTime); // 회전 함수

	public:
		FVector3	GetLoc() { return m_Loc; }
		float		GetScale() { return m_Scale; }
		bool		IsDead() { return m_bDead; }
};

