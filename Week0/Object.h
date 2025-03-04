#pragma once
class UObject
{
public:
	UObject();
	virtual ~UObject();

	virtual	void Update(float deltaTime); // 업데이트 함수
	virtual void BeginOverllaped(UObject* _pOther);
};

