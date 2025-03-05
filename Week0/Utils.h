#pragma once
#include "Define.h"
struct ObjectData {
	int objIndex;
	FVector3 position;
	FVector3 rotation;
	FVector3 scale;
	//TODO : 추후 계층 구조 추가 시 사용.
	//ObjectData* parentObject;
	//vector<ObjectData*> childObjects;
};


struct FVertexSimple {
	// 각 정점에 위치 정보와 색상 정보가 함께 저장.
	float x, y, z;			// Position(위치)
	float r, g, b, a;		// Color (색상)
};