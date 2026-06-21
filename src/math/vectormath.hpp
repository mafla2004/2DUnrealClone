#ifndef __C_VEC_MATH_HPP__
#define __C_VEC_MATH_HPP__

#include <raylib.h>

// So fun fact, I need functions defined in raymath.h for the dot product, but since it redefines structs defined in raylib.h
// it causes all sorts of errors, and I need raylib.h in my header and raymath.h in my cpp, and since I'm extremely fucking pissed right now,
// talking about straight up boiling, completely irascible and mildly genocidal too, I am not going to break my head and balls looking for a proper
// solution and I'm making what I need myself - Because that's what I fucking do, I do the thing myself and you die.

//-------------------------------------------------------------------------------------------
// VECTOR2
//-------------------------------------------------------------------------------------------

inline Vector2 operator+(const Vector2& v1, const Vector2& v2) { return {v1.x + v2.x, v1.y + v2.y}; }
inline Vector2 operator-(const Vector2& v1, const Vector2& v2) { return {v1.x - v2.x, v1.y - v2.y}; }
inline Vector2 operator*(const Vector2& v1, float n) { return {v1.x * n, v1.y * n}; }
inline Vector2 operator/(const Vector2& v1, float n) { return {v1.x / n, v1.y / n}; }
inline Vector2 operator*(float n, const Vector2& v1) { return {v1.x * n, v1.y * n}; }

// Dot Product
inline float operator*(const Vector2& v1, const Vector2& v2) { return v1.x * v2.x + v1.y * v2.y; }

#endif