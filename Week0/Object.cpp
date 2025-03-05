#include "Object.h"

static const float MaxSpeed = 0.003f;
UObject::UObject()
{
}

UObject::UObject(OBJECTLIST _OL) : m_Type(_OL)
{
}

UObject::UObject(FVector3 _Loc, FVector3 _Vel): m_Loc(_Loc), m_Velocity(_Vel)
{
}

UObject::UObject(FVector3 _Loc, FVector3 _Scale, FVector3 _Rot, FVector3 _Vel) : m_Loc(_Loc), m_Velocity(_Vel), m_Rot(_Rot), m_Scale(_Scale)
{
}

UObject::UObject(FVector3 _Loc, FVector3 _Scale, FVector3 _Rot, FVector3 _Vel, OBJECTLIST _Type)
	: m_Loc(_Loc), m_Velocity(_Vel), m_Rot(_Rot), m_Scale(_Scale), m_Type(_Type)
{
}

UObject::~UObject()
{
}

void UObject::Update(float deltaTime)
{
}

void UObject::BeginOverllaped(UObject* _pOther)
{

}

void UObject::SetVel(FVector3 _Vec) {

	// 속도가 최대 속도를 초과하면 정규화 후 조정
	if (_Vec.Magnitude() > MaxSpeed) {
		_Vec = _Vec.Normalize() * MaxSpeed;
	}

	// 속도 설정
	m_Velocity = _Vec;
}