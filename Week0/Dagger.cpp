#include "Dagger.h"

UDagger::UDagger()
{
}

UDagger::UDagger(FVector3 _Loc, FVector3 _Vel) : m_Loc(_Loc), m_Velocity(_Vel)
{

}

UDagger::~UDagger()
{
}

void UDagger::Update(float deltaTime)
{
	Move(deltaTime);
	if (m_Loc.x > 1.0f || m_Loc.x < -1.f || m_Loc.y >1.0f || m_Loc.y < -1.0f)
		m_bDead = true;
}

void UDagger::BeginOverllaped(UObject* _pOther)
{
	m_bDead = true;
}

void UDagger::Move(float deltaTime)
{
	m_Loc = m_Loc + m_Velocity * deltaTime;
}

void UDagger::Rotate(float deltaTime)
{
}
