#pragma once
#include "Define.h"
struct ObjectData {
	int objIndex;
	FVector3 position;
	FVector3 rotation;
	FVector3 scale;
	//TODO : ���� ���� ���� �߰� �� ���.
	//ObjectData* parentObject;
	//vector<ObjectData*> childObjects;
};


struct FVertexSimple {
	// �� ������ ��ġ ������ ���� ������ �Բ� ����.
	float x, y, z;			// Position(��ġ)
	float r, g, b, a;		// Color (����)
};