#include "Object.h"

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
