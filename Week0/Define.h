#pragma once
#include <cmath>
#include <algorithm>

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

	// 벡터 내적
	float Dot(const FVector3& other) const {
		return x * other.x + y * other.y + z * other.z;
	}

	// 벡터 크기
	float Magnitude() const {
		return sqrt(x * x + y * y + z * z);
	}

	// 벡터 정규화
	FVector3 Normalize() const {
		float mag = Magnitude();
		return (mag > 0) ? FVector3(x / mag, y / mag, z / mag) : FVector3(0, 0, 0);
	}

	// 스칼라 곱셈
	FVector3 operator*(float scalar) const {
		return FVector3(x * scalar, y * scalar, z * scalar);
	}

	bool operator==(const FVector3& other) const {
		return (x == other.x && y == other.y && z == other.z);
	}

	float Distance(const FVector3& other) const {
		// 두 벡터의 차 벡터의 크기를 계산
		return ((*this - other).Magnitude());
	}
};

enum Direction
{
	D_UP,
	D_DOWN,
	D_RIGHT,
	D_LEFT
};
class UBall {
public:
	// 클래스 이름과, 아래 두개의 변수 이름은 변경 X
	FVector3 Location;
	FVector3 Velocity;
	float Radius;
	float Mass;

	// 자유 변수
	float Index;
	int NumHits;
	UBall* NextBall; // 다음 볼 객체 가리키는 포인터.
	FVector3 offset;
	FVector3 velocity;
	// 각속도 미완
	//FVector3 AngularVelocity;

	/*void Render(URenderer* renderer) {

	}

	void DoRender(URenderer& renderer) {

	}

	void Move() {

	}

	void Update() {

	}*/
};

