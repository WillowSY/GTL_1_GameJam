#pragma once
#include "Define.h"
#include "Object.h"

class UBall;
class SharkShark;
class UPlayer : public UObject
{
public:
	UPlayer();
	~UPlayer();

	virtual void	Update(float deltaTime);
	virtual void	BeginOverllaped(UObject* _pOther);
public:
	void	Initialize();
	void	Release();
	void	SetMainGame(SharkShark* _MainGame);
	void	Reposition();
	void	AddDragonBladeGage(float _Add);
	void	DashReset();
	void	TakeDamage(float _Damage);
private:
	void	Move();
	void	Move(float _Scale, Direction _Dir);
	void	SideCheck();
	void	Jump();
	void	Attack();
	void	Dash();
	void	Dumbling();
	void	Reflection();
	void	FinishReflection();
	void	DragonBlade();
	void	FinishDragonBlade();
private:
	float		m_Scale = 0.05f;
	
	FVector3	m_DashTarget;
	bool		m_Dead = false;
	float		m_MaxHp;
	float		m_Hp;
	bool		m_bJumping = false;
	//질풍참
	bool		m_Dashing = false;
	float		m_DashCDT = 10.0f;
	float		m_DashTimer = 10.0f;
	// 수리검
	float		m_AttackCDT = 0.5f;
	float		m_AttackTimer = 0.0f;
	// 튕겨내기
	float		m_ReflectionCDT = 8.0f;
	float		m_ReflectionTimer = 8.0f;
	float		m_Reflectionlasting = 2.0f;
	bool		m_bReflecting = false;
	// 용검
	float		m_NeedGage = 10.0f;
	float		m_DragonBladeGage = 0.0f;
	float		m_DragonBladeLasting = 5.0f;
	bool		m_bDragonBlading = false;
private:
	SharkShark* m_pMainGame;

public:

	float		GetScale() { return m_Scale; }
	FVector3	GetRot() { return m_Rot; }
	float		GetHp() { return m_Hp; }
	float		GetMaxHp() { return m_MaxHp; }
	float		GetDragonBladeGage() { return m_DragonBladeGage; }
	float		GetDragonBladeNeedGage() { return m_NeedGage; }

	bool		IsDead() { return m_Dead; }
	bool		IsDash() { return m_Dashing; }
	float		GetDashTimer() { return m_DashTimer; }
	bool		IsDragonBlading() { return m_bDragonBlading; }
};

