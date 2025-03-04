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
		// ���� �Ÿ� ���
		FVector3 remaining = m_DashTarget - m_Loc;
		float distance = remaining.Magnitude();

		// �Ӱ�ġ �����̸� ��ǥ�� ������ ������ ó��
		if (distance < 0.05f)  // �Ӱ�ġ
		{
			m_Loc = m_DashTarget;
			m_Dashing = false;
		}
		else
		{
			// ���� �������� ���� ��eh��ŭ �̵�
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
	// ��ǥ ������ ���� �뽬 �Ÿ� ��� (�ּ� 0.2, �ִ� 0.7)
	float dashDistance = min((MousePosition - m_Loc).Magnitude() + 0.2f, 0.7f);
	FVector3 potentialDashTarget = m_Loc + direction * dashDistance;

	// ���� ��踦 ħ������ �ʵ��� �뽬 ��ǥ ���� ����
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

