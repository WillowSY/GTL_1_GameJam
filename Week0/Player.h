#pragma once
#include "Define.h"
#include "Object.h"

class UBall;
class UPlayer : public UObject
{
public:
	UPlayer();
	~UPlayer();

public:
	virtual void	Update(float deltaTime);
	void	Initialize();
	void	Release();

	void	Move();
	void	Move(float _Scale, Direction _Dir);
	void	SideCheck();
	void	Jump();
	void	Dash();
	
	virtual void BeginOverllaped(UObject* _pOther);

private:
	FVector3	m_Loc;
	float		m_Scale = 0.1f;
	FVector3	m_Rot;
	FVector3	m_Velocity;
	FVector3	m_DashTarget;
	bool		m_Dead = false;
	float		m_Hp;
	bool		m_Dashing = false;
	float		m_DashCDT = 10.0f;
	float		m_DashTimer = 10.0f;
	bool		m_bJumping = false;
public:
	FVector3	GetLoc() { return m_Loc; }
	float		GetScale() { return m_Scale; }
	FVector3	GetRot() { return m_Rot; }
	FVector3	GetVelocity() { return m_Velocity; }
	bool		IsDead() { return m_Dead; }
	bool		IsDash() { return m_Dashing; }
	float		GetDashTimer() { return m_DashTimer; }
};

