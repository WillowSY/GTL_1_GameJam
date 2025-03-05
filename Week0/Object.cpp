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

UObject::~UObject()
{
}

void UObject::Update(float deltaTime)
{
}

void UObject::BeginOverllaped(UObject* _pOther)
{
}
