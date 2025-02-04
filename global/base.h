#pragma once

#include<iostream>
#include<vector>
#include<map>
#include<cmath>
#include<assert.h>
#include "../math/math.h"

#define PI					3.14159265358979323
#define DEG2RAD(theta)		(0.01745329251994329 * (theta))
#define FRACTION(v)			((v) - (int)(v))

using byte = unsigned char;

struct RGBA {
	byte mB;
	byte mG;
	byte mR;
	byte mA;

	RGBA(
		byte r = 255,
		byte g = 255,
		byte b = 255,
		byte a = 255)
	{
		mR = r;
		mG = g;
		mB = b;
		mA = a;
	}
};

struct Point {
	int32_t x;
	int32_t y;
	RGBA color;
	math::vec2f uv;
};