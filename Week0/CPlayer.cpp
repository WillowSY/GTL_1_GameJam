#include "CPlayer.h"
#include "Define.h"
CPlayer::CPlayer()
{
}

CPlayer::~CPlayer()
{
}

void CPlayer::Initialize()
{
}

void CPlayer::Update(float deltaTime)
{
	m_Velocity.y -= 0.0005f; // gravity
	Move();
}

void CPlayer::Release()
{
}

void CPlayer::Move()
{
	m_Loc = m_Loc + m_Velocity;
	if (m_Velocity.x > 0)
	{
		m_Velocity.x -= 0.00001f;
		if (m_Velocity.x < 0)
			m_Velocity.x = 0.f;
	}
	else if (m_Velocity.x < 0)
	{
		m_Velocity.x += 0.00001f;
		if (m_Velocity.x > 0)
			m_Velocity.x = 0.f;
	}
	SideCheck();
}

void CPlayer::Move(float _Scale, Direction _Dir)
{
	//if (_Dir == D_UP)
	//	m_Loc.y += _Scale;
	//if (_Dir == D_RIGHT)
	//	m_Loc.x += _Scale;

	if (_Dir == D_RIGHT)
		m_Velocity.x = _Scale;
	SideCheck();
}



void CPlayer::SideCheck()
{
	if (m_Loc.x - m_Scale * 0.5f < -1.0f)
		m_Loc.x = -1.0f + m_Scale * 0.5f;
	if (m_Loc.x + m_Scale * 0.5f> 1.0f)
		m_Loc.x = 1.0f - m_Scale * 0.5f;
	if (m_Loc.y - m_Scale * 0.5f< -1.0f)
		m_Loc.y = -1.0f + m_Scale * 0.5f;
	if (m_Loc.y + m_Scale * 0.5f > 1.0f)
		m_Loc.y = 1.0f - m_Scale * 0.5f;
}

void CPlayer::Jump()
{
	m_Velocity.y = 0.02f;
}
