#include "Ball.h"
#include "Player.h"
#include "Dagger.h"
#include "SharkShark.h"
#include "Sound.h"
UBall::UBall() : UObject(OL_BALL)
{
}

UBall::~UBall()
{
}


void UBall::Update(float deltaTime)
{
	AttackTimer -= deltaTime;
	Move(deltaTime);
	//Rotate(deltaTime);
	if (Radius < 0.03f)
		bDead = true;
	if(AttackTimer<0)
		Attack();
}

void UBall::BeginOverllaped(UObject* _pOther)
{
	UPlayer* pPlayer = dynamic_cast<UPlayer*>(_pOther);
	if (pPlayer && pPlayer->IsDash())
	{
		SoundManager::GetInstance().PlayEffect(L"Hit_Dash.mp3");
		bDead = true;
	}
	UBall* pBall = dynamic_cast<UBall*>(_pOther);
	if(pBall)
		CollisionHandle(_pOther);
	UDagger* pDagger = dynamic_cast<UDagger*>(_pOther);
	if (pDagger) {
		if(pDagger->GetIsntigator() != m_Type)
			Radius *= 0.8f;
	}
}

void UBall::SetMainGame(SharkShark* _pMainGame)
{
	m_pMainGame = _pMainGame;
}

void UBall::Attack()
{
	FVector3 AttackDir = m_pMainGame->GetPlayer()->GetLoc() - GetLoc();
	AttackDir = AttackDir.Normalize();

	UObject* newDagger = new UDagger(GetLoc(), AttackDir);
	static_cast<UDagger*>(newDagger)->SetInstigator(m_Type);
	m_pMainGame->GetDaggerList().push_back(newDagger);
	AttackTimer = AttackCDT;
}

void UBall::ResolveOverlap(FVector3& pos1, FVector3& pos2, float penetrationDepth)
{
	FVector3 normal = (pos1 - pos2).Normalize();

	float move1 = penetrationDepth * 0.55f; // 0.5이지만 안정적인 충돌 처리를 위해 좀더 멀리 떨어뜨림
	float move2 = penetrationDepth * 0.55f;

	pos1 = pos1 + normal * move1;
	pos2 = pos2 - normal * move2;
}

void UBall::BoundaryHandle()
{
	if (m_Loc.x < leftBorder + Radius)
	{
		m_Velocity.x *= -1.0f;
	}
	if (m_Loc.x > rightBorder - Radius)
	{
		m_Velocity.x *= -1.0f;
	}
	if (m_Loc.y < topBorder + Radius)
	{
		m_Velocity.y *= -1.0f;
	}
	if (m_Loc.y > bottomBorder - Radius)
	{
		m_Velocity.y *= -1.0f;
	}
	if (m_Loc.x < leftBorder + Radius)
	{
		m_Loc.x = leftBorder + Radius;
	}
	if (m_Loc.x > rightBorder - Radius)
	{
		m_Loc.x = rightBorder - Radius;
	}
	if (m_Loc.y < topBorder + Radius)
	{
		m_Loc.y = topBorder + Radius;
	}
	if (m_Loc.y > bottomBorder - Radius)
	{
		m_Loc.y = bottomBorder - Radius;
	}
}

void UBall::CollisionHandle(UObject* _pOther)
{
	UBall* pOther = static_cast<UBall*>(_pOther);
	float distance = (m_Loc - pOther->m_Loc).Magnitude();
	float radiusSum = Radius + pOther->Radius;
	float penetrationDepth = radiusSum - distance;
	if (distance <= radiusSum)
	{
		FVector3 normal = (m_Loc - pOther->m_Loc).Normalize();
		ResolveOverlap(m_Loc, pOther->m_Loc, penetrationDepth);

		FVector3 relativeVelocity = m_Velocity - pOther->m_Velocity;

		float velocityAlongNormal = relativeVelocity.Dot(normal);

		if (velocityAlongNormal > 0) return;

		float impulse = -2.f * velocityAlongNormal / (Mass + pOther->Mass);
		m_Velocity = m_Velocity + normal * (impulse * pOther->Mass);
		pOther->m_Velocity = pOther->m_Velocity - normal * (impulse * Mass);

		// 회전 추가 
		FVector3 tangent = FVector3(-normal.y, normal.x, 0); // 접선 방향
		float relativeTangentialVelocity = relativeVelocity.Dot(tangent);

		float rotationalImpulse = relativeTangentialVelocity / (Radius + pOther->Radius);
		AngularVelocity += rotationalImpulse / Radius;
		pOther->AngularVelocity -= rotationalImpulse / pOther->Radius;
	}

}

void UBall::Move(float deltaTime)
{
	//if (bGravity)
//	Velocity = Velocity - gravity * deltaTime;
//if (bBlackHole)
//{
//	Velocity = Velocity + (locBlackHole - Location).Normalize() * blackHolePower * 0.00001;
//}
	m_Loc = m_Loc + m_Velocity * deltaTime * 1000;
	BoundaryHandle();
}

void UBall::Rotate(float deltaTime)
{
	RotationAngle += AngularVelocity * deltaTime;

	// 2π를 넘어서지 않도록 정리 (불필요한 큰 값 방지)
	if (RotationAngle > 2 * 3.14f)
		RotationAngle -= 2 * 3.14f;
	else if (RotationAngle < -2 * 3.14f)
		RotationAngle += 2 * 3.14f;
	m_Rot.z += 10;
}

void UBall::RestrictVel(FVector3 _Vec) {
	// 속도가 최대 속도를 초과하면 정규화 후 조정
	if (_Vec.Magnitude() > MaxSpeed) {
		_Vec = _Vec.Normalize() * MaxSpeed;
	}

	// 속도 설정
	m_Velocity = _Vec;
}