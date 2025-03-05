#include "Windows.h"
#include "Player.h"
#include "Define.h"
#include "SharkShark.h"
#include "Ball.h"
#include "Dagger.h"
#include "Sound.h"
extern FVector3 MousePosition;

UPlayer::UPlayer() : UObject(FVector3(0.0f, -1.0f, 0.0f), FVector3(0.05f,0.05f,0.05f)
	, FVector3(), FVector3(), OL_PLAYER)
{
}

UPlayer::~UPlayer()
{
}

void UPlayer::Initialize()
{
	m_Loc = FVector3(0.0f, -1.0f, 0.0f);
	//m_Scale = FVector3(0.05f, 0.05f, 0.05f);
	m_Rot = FVector3(0.0f, 0.0f, 0.0f);
	m_Velocity = FVector3(0.0f, 0.0f, 0.0f);
	m_MaxHp = 100.0f;
	m_Hp = 100.0f;
	m_Dead = false;
	m_Dashing = false;
	m_Scale = 0.05f;
}

void UPlayer::Update(float deltaTime)
{
	if ((m_DashTimer -= deltaTime) < 0.f)
		m_DashTimer = 0.f;

	m_AttackTimer -= deltaTime;
	if ((m_ReflectionTimer -= deltaTime) < 0.f)
		m_ReflectionTimer = 0.f;
	
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
	if (m_bReflecting)
	{
		Rotate();
		if ((m_Reflectionlasting -= deltaTime) < 0)
			FinishReflection();
	}
	if (m_bDragonBlading)
	{
		if ((m_DragonBladeLasting -= deltaTime) < 0)
			FinishDragonBlade();
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

	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		if (m_bJumping)
			DoubleJump();
		else
			Jump();
	}
	if (GetAsyncKeyState('A') & 0x8000)
	{
		Move(-0.01f, D_RIGHT);
	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		Move(0.01f, D_RIGHT);
	}
	if (GetAsyncKeyState('E') & 0x8000)
	{
		Reflection();
	}
	if (GetAsyncKeyState('Q') & 0x8000)
	{
		DragonBlade();
	}
	// Reposition 이후 Dash 재적용 되는 문제 해결
	if (GetAsyncKeyState(VK_RBUTTON) & 0x0001)
	{
		Dash();
	}	
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
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
	if (m_Loc.y - m_Scale < -0.8f)
	{
		m_Loc.y = -0.8f + m_Scale;
		m_bJumping = false;
		m_bDoubleJump = true;
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
	UObject* newDagger = new UDagger(m_Loc, CurDir);
	static_cast<UDagger*>(newDagger)->SetInstigator(m_Type);
	m_pMainGame->GetDaggerList().push_back(newDagger);
	newDagger = new UDagger(m_Loc, CurDir2);
	static_cast<UDagger*>(newDagger)->SetInstigator(m_Type);
	m_pMainGame->GetDaggerList().push_back(newDagger);
	newDagger = new UDagger(m_Loc, CurDir3);
	static_cast<UDagger*>(newDagger)->SetInstigator(m_Type);
	m_pMainGame->GetDaggerList().push_back(newDagger);
	m_AttackTimer = m_AttackCDT;
	SoundManager::GetInstance().PlayEffect(L"Attack_0.mp3");

}

void UPlayer::Dash()
{
	if (m_DashTimer > 0.0f)
		return;
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
	m_DashTimer = m_DashCDT;
	SoundManager::GetInstance().PlayEffect(L"Dash.mp3");
}

void UPlayer::Rotate()
{
	m_Rot.z += 20;
}



void UPlayer::Reflection()
{
	if (m_ReflectionTimer > 0.0f)
		return;
	m_bReflecting = true;
}

void UPlayer::FinishReflection()
{
	m_bReflecting = false;
	m_Reflectionlasting = 2.0f;
	m_ReflectionTimer = m_ReflectionCDT;
	m_Rot.z = 0;
}

void UPlayer::DragonBlade()
{
	if (m_DragonBladeGage < m_NeedGage)
		return;
	DashReset();
	m_bDragonBlading = true;
	m_DragonBladeGage = 0.0f;
	m_Scale *= 2;
	SoundManager::GetInstance().PlayEffect(L"DragonBlade.mp3");
}

void UPlayer::FinishDragonBlade()
{
	m_bDragonBlading = false;
	m_DragonBladeGage = 0.0f;
	m_DragonBladeLasting = 5.0f;
	m_Scale /= 2;
}

void UPlayer::DoubleJump()
{

	if (m_bJumping && m_bDoubleJump && (m_Loc.x < -0.95f + m_Scale || m_Loc.x == 1.0f - m_Scale))
	{

		m_Velocity.y = 0.02f;
		m_bDoubleJump = false;
	}
}

void UPlayer::AddDragonBladeGage(float _Add)
{
	m_DragonBladeGage += _Add;
	if (m_DragonBladeGage > m_NeedGage)
		m_DragonBladeGage = m_NeedGage;
}

void UPlayer::DashReset()
{
	m_DashTimer = 0.0f;
}

void UPlayer::TakeDamage(float _Damage)
{
	m_Hp -= _Damage;
	if (m_Hp <= 0.f)
	{
		m_Dead = true;
		m_Hp = 0.0f;
	}
}

void UPlayer::Reposition()
{
	m_Loc = FVector3(0.0f, -1.0f, 0.0f);
	m_Rot = FVector3(0.0f, 0.0f, 0.0f);
	m_Velocity = FVector3(0.0f, 0.0f, 0.0f);
	m_Dead = false;
	m_Dashing = false;
	if (m_bDragonBlading) { // 이겼을 때는 유지? 
		FinishDragonBlade();
		m_Scale = 0.05f;
	}
	if (m_bReflecting)
		FinishReflection();
}

void UPlayer::BeginOverllaped(UObject* _pOther)
{
	UBall* pBall = dynamic_cast<UBall*>(_pOther);
	if (pBall && !m_Dashing)
	{
		TakeDamage(2.0f);
	}
	UDagger* pDagger = dynamic_cast<UDagger*>(_pOther);
	if (pDagger && pDagger->GetIsntigator() != GetType())
	{
		if (!m_bReflecting)
			TakeDamage(1.0f);
		else 
		{
			pDagger->SetVel(pDagger->GetVelocity() * -1);
			pDagger->SetInstigator(m_Type);
		}
	}
}