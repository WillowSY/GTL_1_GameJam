#include "Windows.h"
#include "Player.h"
#include "Define.h"
#include "SharkShark.h"
#include "Ball.h"
#include "Dagger.h"
#include "Sound.h"
#include "LeaderBoard.h"
#include "GameMode.h"
#include <windows.h>
#include <Xinput.h>
#pragma comment(lib, "xinput.lib")

extern FVector3 MousePosition;


void SendMouseClick()
{
	INPUT input[2] = {};

	// 마우스 왼쪽 버튼 누르기
	input[0].type = INPUT_MOUSE;
	input[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

	// 마우스 왼쪽 버튼 떼기
	input[1].type = INPUT_MOUSE;
	input[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;

	SendInput(2, input, sizeof(INPUT));
}

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
	if (m_Velocity.y < -0.05f)
		m_Velocity.y = -0.05f;
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
	XINPUT_STATE state;
	ZeroMemory(&state, sizeof(XINPUT_STATE));
	bool gamepadConnected = (XInputGetState(0, &state) == ERROR_SUCCESS);

	// 위치 업데이트
	m_Loc = m_Loc + m_Velocity;

	// 점프 (키보드: SPACE, 게임패드: A 버튼)
	if ((GetAsyncKeyState(VK_SPACE) & 0x8000) || (gamepadConnected && (state.Gamepad.wButtons & XINPUT_GAMEPAD_A))) {
		if (m_bJumping)
			DoubleJump();
		else
			Jump();
	}

	// 이동 (키보드: A/D, 게임패드: 왼쪽 스틱)
	float moveSpeed = 0.01f;
	const SHORT deadZone = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;
	SHORT leftStickX = gamepadConnected ? state.Gamepad.sThumbLX : 0;

	if (GetAsyncKeyState('A') & 0x8000 || leftStickX < -deadZone) {
		Move(-moveSpeed, D_RIGHT);
	}
	if (GetAsyncKeyState('D') & 0x8000 || leftStickX > deadZone) {
		Move(moveSpeed, D_RIGHT);
	}
	if (gamepadConnected) {
		SHORT rightStickX = state.Gamepad.sThumbRX;
		SHORT rightStickY = state.Gamepad.sThumbRY;

		const SHORT deadZone = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;
		if (abs(rightStickX) > deadZone || abs(rightStickY) > deadZone) {
			float mouseSensitivity = 8.0f; // 마우스 이동 감도 조절

			INPUT input = { 0 };
			input.type = INPUT_MOUSE;
			input.mi.dwFlags = MOUSEEVENTF_MOVE;
			input.mi.dx = static_cast<int>((rightStickX / 32768.0f) * mouseSensitivity);
			input.mi.dy = static_cast<int>((-rightStickY / 32768.0f) * mouseSensitivity); // Y축 반전

			SendInput(1, &input, sizeof(INPUT));
		}
	}
	// Reflection (키보드: E, 게임패드: X 버튼)
	if ((GetAsyncKeyState('E') & 0x8000) || (gamepadConnected && (state.Gamepad.wButtons & XINPUT_GAMEPAD_X))) {
		Reflection();
	}

	// DragonBlade (키보드: Q, 게임패드: Y 버튼)
	if ((GetAsyncKeyState('Q') & 0x8000) || (gamepadConnected && (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y))) {
		DragonBlade();
	}

	// Dash (키보드: 우클릭, 게임패드: RB 버튼)
	if ((GetAsyncKeyState(VK_RBUTTON) & 0x0001) || (gamepadConnected && (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER))) {
		Dash();
	}

	// 공격 (키보드: 좌클릭, 게임패드: RT 트리거)
	if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) || (gamepadConnected && state.Gamepad.bRightTrigger > 100)) {
		Attack();
	}

	// 충돌 체크
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

        int randomIndex = rand() % 3;
        std::wstring soundFile = L"jump" + std::to_wstring(randomIndex) + L".mp3";

        SoundManager::GetInstance().PlayEffect(soundFile.c_str());
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
		int randomIndex = rand() % 3;
		std::wstring soundFile = L"jump" + std::to_wstring(randomIndex) + L".mp3";

		SoundManager::GetInstance().PlayEffect(soundFile.c_str());
	}

}

void UPlayer::AddDragonBladeGage(float _Add)
{
	if (m_DragonBladeGage < m_NeedGage)
	{
		m_DragonBladeGage += _Add;
		if (m_DragonBladeGage >= m_NeedGage)
		{
			m_DragonBladeGage = m_NeedGage;
		}
			
	}
}

void UPlayer::DashReset()
{
	m_DashTimer = 0.0f;
}

void UPlayer::TakeDamage(float _Damage)
{
	if (m_Hp <= 0.0f)
		return;
	m_Hp -= _Damage;
	if (m_Hp <= 0.f)
	{
		m_Dead = true;
		m_Hp = 0.0f;
		m_pMainGame->GetLeaderboard()->AddScore(m_pMainGame->GetGameMode()->score);
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
			SoundManager::GetInstance().PlayEffect(L"ReflectMetal.wav");
		}
	}
}