#include "Dagger.h"

UDagger::UDagger()
{
}

UDagger::UDagger(FVector3 _Loc, FVector3 _Vel) : UObject(_Loc, _Vel)
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
	Rotate(deltaTime);
}

void UDagger::BeginOverllaped(UObject* _pOther)
{
	if(_pOther->GetType() != m_Instigator)
		m_bDead = true;
}

void UDagger::SetInstigator(OBJECTLIST _Instigator)
{
	m_Instigator = _Instigator;
}

void UDagger::Move(float deltaTime)
{
	m_Loc = m_Loc + m_Velocity * deltaTime;
}

void UDagger::Rotate(float deltaTime)
{
	m_Rot.z += 30;
}
