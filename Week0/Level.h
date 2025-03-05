#pragma once
#include "Object.h"
#include "Define.h"

class SharkShark;
class ULevel : public UObject {
public:
	ULevel();
	virtual ~ULevel();

	virtual void BeginOverllaped(UObject* _pOther) override;

private:
	void CollisionHandle(UObject* _pOther);
	
};