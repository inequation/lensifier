#ifndef UTILS_H
#define UTILS_H

namespace Lensifier
{

struct Vector2
{
	union {float V[2]; struct {float X, Y;};};
	Vector2(float InX, float InY) : X(InX), Y(InY) {}
	Vector2() : Vector2(0.f, 0.f) {}
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
	Vector3() : Vector3(0.f, 0.f, 0.f) {}
};
struct Vector4
{
	union {float V[4]; struct {float X, Y, Z, W;};};
	Vector4(float InX, float InY, float InZ, float InW) : X(InX), Y(InY), Z(InZ), W(InW) {}
	Vector4() : Vector4(0.f, 0.f, 0.f, 0.f) {}
};

}

#endif // UTILS_H
