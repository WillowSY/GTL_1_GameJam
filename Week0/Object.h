#pragma once
#include "Define.h"
class UObject
{
public:
	UObject();
	UObject(OBJECTLIST _OL);
	UObject(FVector3 _Loc, FVector3 _Vel);
	UObject(FVector3 _Loc, FVector3 _Scale, FVector3 _Rot, FVector3 _Vel);
	UObject(FVector3 _Loc, FVector3 _Scale, FVector3 _Rot, FVector3 _Vel, OBJECTLIST _Type);

	virtual ~UObject();

	virtual	void Update(float deltaTime); // 업데이트 함수
	virtual void BeginOverllaped(UObject* _pOther);

protected:
	int			m_Index;
	FVector3	m_Loc;
	FVector3	m_Velocity;
	FVector3	m_Scale;
	FVector3	m_Rot;
	OBJECTLIST	m_Type;
public:
	int			GetIndex() { return m_Index; }
	FVector3	GetVelocity() { return m_Velocity; }
	FVector3	GetLoc() { return m_Loc; }
	FVector3	GetScale() { return m_Scale; }
	FVector3	GetRot() { return m_Rot; }
	OBJECTLIST	GetType() { return m_Type; }
	void		SetLoc(FVector3 _Loc) { m_Loc = _Loc; }
	void		SetVel(FVector3 _Vec); // { m_Velocity = _Vec; }
	void		SetType(OBJECTLIST _Type) { m_Type = _Type; }
	void		SetScale(FVector3 _Scale) { m_Scale = _Scale; }
	void		SetRot(FVector3 _Rotation) { m_Rot = _Rotation; }
	void		SetIndex(int _Index) { m_Index = _Index; }

};

