#pragma once
class UObject
{
public:
	UObject();
	virtual ~UObject();

	virtual	void Update(float deltaTime); // ������Ʈ �Լ�
	virtual void BeginOverllaped(UObject* _pOther);
};

