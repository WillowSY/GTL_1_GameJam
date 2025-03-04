#include "Player.h"
#include "Define.h"
#include "Windows.h"

extern FVector3 MousePosition;

UPlayer::UPlayer()
{
}

UPlayer::~UPlayer()
{
}

void UPlayer::Initialize()
{
}

void UPlayer::Update(float deltaTime)
{
	if (m_Dashing)
	{
		// 남은 거리 계산
		FVector3 remaining = m_DashTarget - m_Loc;
		float distance = remaining.Magnitude();

		// 임계치 이하이면 목표에 도달한 것으로 처리
		if (distance < 0.05f)  // 임계치
		{
			m_Loc = m_DashTarget;
			m_Dashing = false;
		}
		else
		{
			// 남은 방향으로 일정 속eh만큼 이동
			FVector3 moveStep = remaining.Normalize() * 0.03f;
			m_Loc = m_Loc + moveStep;
		}
		return;
	}

	m_Velocity.y -= 0.0005f; // gravity
	Move();
}
void UPlayer::Release()
{
}

void UPlayer::Move()
{
	m_Loc = m_Loc + m_Velocity;
	if (m_Velocity.x > 0)
	{
		m_Velocity.x -= 0.001f;
		if (m_Velocity.x < 0)
			m_Velocity.x = 0.f;
	}
	else if (m_Velocity.x < 0)
	{
		m_Velocity.x += 0.001f;
		if (m_Velocity.x > 0)
			m_Velocity.x = 0.f;
	}
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		Jump();
	}
	if (GetAsyncKeyState('A') & 0x8000)
	{
		Move(-0.005f, D_RIGHT);
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		Move(0.005f, D_RIGHT);
	}
	if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
	{
		Dash();
	}
	SideCheck();
}

void UPlayer::Move(float _Scale, Direction _Dir)
{
	if (_Dir == D_UP)
		m_Loc.y += _Scale;
	if (_Dir == D_RIGHT)
		m_Loc.x += _Scale;

	//if (_Dir == D_RIGHT)
	//	m_Velocity.x = _Scale;
	SideCheck();
}



void UPlayer::SideCheck()
{
	if (m_Loc.x - m_Scale< -1.0f)
		m_Loc.x = -1.0f + m_Scale;
	if (m_Loc.x + m_Scale> 1.0f)
		m_Loc.x = 1.0f - m_Scale;
	if (m_Loc.y - m_Scale< -1.0f)
		m_Loc.y = -1.0f + m_Scale;
	if (m_Loc.y + m_Scale > 1.0f)
		m_Loc.y = 1.0f - m_Scale;
}

void UPlayer::Jump()
{
	m_Velocity.y = 0.02f;
}

void UPlayer::Dash()
{
	FVector3 direction = (MousePosition - m_Loc).Normalize();
	// 목표 지점에 따른 대쉬 거리 계산 (최소 0.2, 최대 0.7)
	float dashDistance = min((MousePosition - m_Loc).Magnitude() + 0.2f, 0.7f);
	FVector3 potentialDashTarget = m_Loc + direction * dashDistance;

	// 벽의 경계를 침범하지 않도록 대쉬 목표 지점 조정
	if (potentialDashTarget.x - m_Scale < -1.0f)
		potentialDashTarget.x = -1.0f + m_Scale;
	if (potentialDashTarget.x + m_Scale > 1.0f)
		potentialDashTarget.x = 1.0f - m_Scale;
	if (potentialDashTarget.y - m_Scale < -1.0f)
		potentialDashTarget.y = -1.0f + m_Scale;
	if (potentialDashTarget.y + m_Scale > 1.0f)
		potentialDashTarget.y = 1.0f - m_Scale;

	m_DashTarget = potentialDashTarget;
	m_Velocity.y = 0;
	m_Dashing = true;

}

void UPlayer::BeginOverllaped(UObject* _pOther)
{
}

