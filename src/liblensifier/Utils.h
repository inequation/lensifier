#ifndef UTILS_H
#define UTILS_H

namespace Lensifier
{

struct Vector2
{
	union {float V[2]; struct {float X, Y;};};
	Vector2(float InX, float InY) : X(InX), Y(InY) {}
	Vector2(float InXY) : X(InXY), Y(InXY) {}
	Vector2() : X(0.f), Y(0.f) {}
	inline bool operator==(const Vector2& Other) {
		return X == Other.X && Y == Other.Y;
	}
	inline bool operator!=(const Vector2& Other) {
		return X != Other.X && Y != Other.Y;
	}
};
struct Vector3
{
	union {float V[3]; struct {float X, Y, Z;};};
	Vector3(float InX, float InY, float InZ) : X(InX), Y(InY), Z(InZ) {}
	Vector3(float InXYZ) : X(InXYZ), Y(InXYZ), Z(InXYZ) {}
	Vector3() : X(0.f), Y(0.f), Z(0.f) {}
};
struct Vector4
{
	union {float V[4]; struct {float X, Y, Z, W;};};
	Vector4(float InX, float InY, float InZ, float InW) : X(InX), Y(InY), Z(InZ), W(InW) {}
	Vector4(float InXYZW) : X(InXYZW), Y(InXYZW), Z(InXYZW), W(InXYZW) {}
	Vector4() : X(0.f), Y(0.f), Z(0.f), W(0.f) {}
};

}

#endif // UTILS_H
