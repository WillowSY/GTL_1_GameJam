#pragma once
#include "Define.h"

class UBall;
class UPlayer
{
public:
	UPlayer();
	~UPlayer();

public:
	void	Initialize();
	void	Update(float deltaTime);
	void	Release();

	void	Move();
	void	Move(float _Scale, Direction _Dir);
	void	SideCheck();
	void	Jump();
	void	Dash();
	
	void	BeginOverlapped(UBall* _Ball);
private:
	FVector3	m_Loc;
	float		m_Scale = 0.1f;
	FVector3	m_Rot;
	FVector3	m_Velocity;
	FVector3	m_DashTarget;
	bool		m_Dead = false;
	float		m_Hp;
	bool		m_Dashing = false;


public:
	FVector3	GetLoc() { return m_Loc; }
	float		GetScale() { return m_Scale; }
	FVector3	GetRot() { return m_Rot; }
	FVector3	GetVelocity() { return m_Velocity; }
	bool		IsDead() { return m_Dead; }
	bool		IsDash() { return m_Dashing; }
};

