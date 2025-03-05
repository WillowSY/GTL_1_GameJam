#include "Ball.h"
#include "Player.h"
UBall::UBall()
{
}

UBall::~UBall()
{
}

int UBall::ballCount = 0;
bool UBall::CreateBall()
{
	ballCount++;
	UBall* PossibleBall = new UBall;
	// 생성할 위치 초기화
	FVector3 newLocation;
	bool locationValid = false;

	// 최대 시도 횟수 (너무 많은 시도가 되지 않도록 제한)
	int maxAttempts = 10;
	int attempts = 0;

	//생성 가능 여부 확인
	while (!locationValid && attempts < maxAttempts)
	{
		newLocation = FVector3(((rand() % 2000) / 1000.0f) - 1.0f, ((rand() % 2000) / 1000.0f) - 1.0f, 0.f);


		UBall* pIter = NextBall;
		locationValid = true;
		while (pIter)
		{

			float distance = (newLocation - pIter->Location).Magnitude();
			float radiusSum = NextBall->Radius + pIter->Radius;
			if (distance < radiusSum)  // 겹침 발생
			{
				locationValid = false; // 겹침이 있으면 유효하지 않음
				break;
			}
			pIter = pIter->NextBall;
		}

		attempts++;
	}
	// 생성 가능 위치를 찾았다면 생성
	if (locationValid)
	{
		PossibleBall->Location = newLocation;
		PossibleBall->Velocity = FVector3(((float)(rand() % 100 - 50)) * ballSpeed, ((float)(rand() % 100 - 50)) * ballSpeed, 0.0f);
		PossibleBall->Radius = (sphereRadius * scaleMod) * (1.f - ((rand() % 1001) / 1000.0) * 0.9);
		PossibleBall->Mass = PossibleBall->Radius * 100.0f;
		PossibleBall->NextBall = NextBall;
		PossibleBall->PrevBall = this;
	
		if (NextBall)
			NextBall->PrevBall = PossibleBall;
		NextBall = PossibleBall;
		return true;
	}
	else
	{
		delete PossibleBall;
		ballCount--;
		return false;
	}
}

void UBall::DeleteRandomBall()
{
	int deleteIdx = (rand() % UBall::ballCount) + 1;
	UBall* pIter = this;
	int count = 1;
	while (pIter)
	{
		UBall* pNext = pIter->NextBall;
		if (pNext == nullptr)
			break;
		if (count == deleteIdx)
		{
			pIter->NextBall = pNext->NextBall;
			delete pNext;
			break;
		}
		pIter = pIter->NextBall;
		count++;
	}
	ballCount--;
}

void UBall::DeleteBall()
{
	if (PrevBall)
	{
		PrevBall->NextBall = NextBall;
	}
	if (NextBall)
	{
		NextBall->PrevBall = PrevBall;
	}
	ballCount--;
	delete this;
}

void UBall::Update(float deltaTime)
{
	Move(deltaTime);
}

void UBall::BeginOverllaped(UObject* _pOther)
{
	UPlayer* pPlayer = dynamic_cast<UPlayer*>(_pOther);
	if (pPlayer)
		bDead = true;
	UBall* pBall = dynamic_cast<UBall*>(_pOther);
	if(pBall)
		CollisionHandle(_pOther);
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
	if (Location.x < leftBorder + Radius)
	{
		Velocity.x *= -1.0f;
	}
	if (Location.x > rightBorder - Radius)
	{
		Velocity.x *= -1.0f;
	}
	if (Location.y < topBorder + Radius)
	{
		Velocity.y *= -1.0f;
	}
	if (Location.y > bottomBorder - Radius)
	{
		Velocity.y *= -1.0f;
	}
	if (Location.x < leftBorder + Radius)
	{
		Location.x = leftBorder + Radius;
	}
	if (Location.x > rightBorder - Radius)
	{
		Location.x = rightBorder - Radius;
	}
	if (Location.y < topBorder + Radius)
	{
		Location.y = topBorder + Radius;
	}
	if (Location.y > bottomBorder - Radius)
	{
		Location.y = bottomBorder - Radius;
	}
}

void UBall::CollisionHandle(UObject* _pOther)
{
	UBall* pOther = static_cast<UBall*>(_pOther);
	float distance = (Location - pOther->Location).Magnitude();
	float radiusSum = Radius + pOther->Radius;
	float penetrationDepth = radiusSum - distance;
	if (distance <= radiusSum)
	{
		FVector3 normal = (Location - pOther->Location).Normalize();
		ResolveOverlap(Location, pOther->Location, penetrationDepth);

		FVector3 relativeVelocity = Velocity - pOther->Velocity;

		float velocityAlongNormal = relativeVelocity.Dot(normal);

		if (velocityAlongNormal > 0) return;

		float impulse = -2.f * velocityAlongNormal / (Mass + pOther->Mass);
		Velocity = Velocity + normal * (impulse * pOther->Mass);
		pOther->Velocity = pOther->Velocity - normal * (impulse * Mass);

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
	Location = Location + Velocity * deltaTime;
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

}
