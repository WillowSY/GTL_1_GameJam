#pragma once
#include "Define.h"
class UObject
{
public:
	UObject();
	UObject(OBJECTLIST _OL);
	UObject(FVector3 _Loc, FVector3 _Vel);
	virtual ~UObject();

	virtual	void Update(float deltaTime); // 업데이트 함수
	virtual void BeginOverllaped(UObject* _pOther);

protected:
	FVector3	m_Loc;
	FVector3	m_Velocity;
	OBJECTLIST	m_Type;
public:
	FVector3	GetVelocity() { return m_Velocity; }
	FVector3	GetLoc() { return m_Loc; }
	OBJECTLIST	GetType() { return m_Type; }
	void		SetLoc(FVector3 _Loc) { m_Loc = _Loc; }
	void		SetVel(FVector3 _Vec) { m_Velocity = _Vec; }
	void		SetType(OBJECTLIST _Type) { m_Type = _Type; }

};

