#pragma once
#include <cmath>
#include <algorithm>
#include <list>
#include <vector>
using namespace std;
struct FVector3
{
	float x, y, z;
	FVector3(float _x = 0, float _y = 0, float _z = 0) : x(_x), y(_y), z(_z) {}

	FVector3 operator-(const FVector3& other) const {
		return FVector3(x - other.x, y - other.y, z - other.z);
	}
	FVector3 operator+(const FVector3& other) const {
		return FVector3(x + other.x, y + other.y, z + other.z);
	}

	// ���� ����
	float Dot(const FVector3& other) const {
		return x * other.x + y * other.y + z * other.z;
	}

	// ���� ũ��
	float Magnitude() const {
		return sqrt(x * x + y * y + z * z);
	}

	// ���� ����ȭ
	FVector3 Normalize() const {
		float mag = Magnitude();
		return (mag > 0) ? FVector3(x / mag, y / mag, z / mag) : FVector3(0, 0, 0);
	}

	// ��Į�� ����
	FVector3 operator*(float scalar) const {
		return FVector3(x * scalar, y * scalar, z * scalar);
	}

	bool operator==(const FVector3& other) const {
		return (x == other.x && y == other.y && z == other.z);
	}

	float Distance(const FVector3& other) const {
		// �� ������ �� ������ ũ�⸦ ���
		return ((*this - other).Magnitude());
	}
};

enum Direction
{
	D_UP,
	D_DOWN,
	D_RIGHT,
	D_LEFT,
	D_END
};

enum OBJECTLIST
{
	OL_PLAYER,
	OL_BALL,
	OL_END
};

