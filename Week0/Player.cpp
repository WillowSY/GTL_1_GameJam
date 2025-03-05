#include "Windows.h"
#include "Player.h"
#include "Define.h"
#include "SharkShark.h"
#include "Ball.h"
#include "Dagger.h"
extern FVector3 MousePosition;

UPlayer::UPlayer()
{
}

UPlayer::~UPlayer()
{
}

void UPlayer::Initialize()
{
	m_Loc = FVector3(0.0f, -1.0f, 0.0f);
	m_Rot = FVector3(0.0f, 0.0f, 0.0f);
	m_Velocity = FVector3(0.0f, 0.0f, 0.0f);
	m_Hp = 1.0f;
	m_Dead = false;
	m_Dashing = false;
	m_Scale = 0.1f;
}

void UPlayer::Update(float deltaTime)
{
	m_DashTimer -= deltaTime;
	m_AttackTimer -= deltaTime;
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

void UPlayer::SetMainGame(SharkShark* _MainGame)
{
	m_pMainGame = _MainGame;
}

void UPlayer::Move()
{
	m_Loc = m_Loc + m_Velocity;
	//if (m_Velocity.x > 0)
	//{
	//	m_Velocity.x -= 0.001f;
	//	if (m_Velocity.x < 0)
	//		m_Velocity.x = 0.f;
	//}
	//else if (m_Velocity.x < 0)
	//{
	//	m_Velocity.x += 0.001f;
	//	if (m_Velocity.x > 0)
	//		m_Velocity.x = 0.f;
	//}
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
	// Reposition ���� Dash ������ �Ǵ� ���� �ذ�
	if (GetAsyncKeyState(VK_RBUTTON) & 0x0001)
	{
		Dash();
	}	
	if (GetAsyncKeyState('R') & 0x8000)
	{
		Attack();
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
	if (m_Loc.y - m_Scale < -1.0f)
	{
		m_Loc.y = -1.0f + m_Scale;
		m_bJumping = false;
	}
	if (m_Loc.y + m_Scale > 1.0f)
		m_Loc.y = 1.0f - m_Scale;
}

void UPlayer::Jump()
{
	if (m_bJumping)
		return; 
	m_Velocity.y = 0.02f;
	m_bJumping = true;
}

void UPlayer::Attack()
{
	if (m_AttackTimer > 0)
		return;
	FVector3 CurDir = MousePosition - m_Loc;
	CurDir = CurDir.Normalize();
	FVector3 CurDir2 = CurDir;
	CurDir2.x = CurDir2.x * cos(0.25) - CurDir2.y * sin(0.25);
	CurDir2.y = CurDir2.x * sin(0.25) + CurDir2.y * cos(0.25);
	FVector3 CurDir3 = CurDir;
	CurDir3.x = CurDir3.x * cos(-0.25) - CurDir3.y * sin(-0.25);
	CurDir3.y = CurDir3.x * sin(-0.25) + CurDir3.y * cos(-0.25);

	m_pMainGame->GetDaggerList().push_back(new UDagger(m_Loc, CurDir));
	m_pMainGame->GetDaggerList().push_back(new UDagger(m_Loc, CurDir2));
	m_pMainGame->GetDaggerList().push_back(new UDagger(m_Loc, CurDir3));
	m_AttackTimer = m_AttackCDT;
}

void UPlayer::Dash()
{
	if (m_DashTimer > 0.0f)
		return;
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
	m_DashTimer = m_DashCDT;

}

void UPlayer::Dumbling()
{
}

void UPlayer::Reposition()
{
	m_Loc = FVector3(0.0f, -1.0f, 0.0f);
	m_Rot = FVector3(0.0f, 0.0f, 0.0f);
	m_Velocity = FVector3(0.0f, 0.0f, 0.0f);
	m_Dead = false;
	m_Dashing = false;
	m_Scale = 0.1f;
}

void UPlayer::BeginOverllaped(UObject* _pOther)
{
	UBall* pBall = dynamic_cast<UBall*>(_pOther);
	if (pBall && !m_Dashing)
		m_Dead = true;
}