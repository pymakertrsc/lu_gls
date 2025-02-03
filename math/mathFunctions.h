#pragma once

#include "vector.h"

namespace math {

	/*
	* 标量与向量
	*-s* v 
	*/
	template<typename T, typename S>
	inline Vector2<T> operator * (S s, const Vector2<T>& v) {
		return v * s;
	}

	template<typename T, typename S>
	inline Vector3<T> operator * (S s, const Vector3<T>& v) {
		return v * s;
	}

	template<typename T, typename S>
	inline Vector4<T> operator * (S s, const Vector4<T>& v) {
		return v * s;
	}

	/*
	* 向量与向量
	*-v = v0 * v1
	*/
	template<typename T>
	inline Vector2<T> operator * (const Vector2<T>& v0, const Vector2<T>& v1) {
		return Vector2<T>(v0.x * v1.x , v0.y * v1.y);
	}

	template<typename T>
	inline Vector3<T> operator * (const Vector3<T>& v0, const Vector3<T>& v1) {
		return Vector3<T>(v0.x * v1.x, v0.y * v1.y, v0.z * v1.z);
	}

	template<typename T>
	inline Vector4<T> operator * (const Vector4<T>& v0, const Vector4<T>& v1) {
		return Vector4<T>(v0.x * v1.x, v0.y * v1.y, v0.z * v1.z, v0.w * v1.w);
	}
	
	/*
	* 绝对值
	*/
	template<typename T>
	inline Vector2<T> abs(const Vector2<T>& v) {
		return Vector2<T>(std::abs(v.x), std::abs(v.y));
	}

	template<typename T>
	inline Vector3<T> abs(const Vector3<T>& v) {
		return Vector3<T>(std::abs(v.x), std::abs(v.y), std::abs(v.z));
	}

	template<typename T>
	inline Vector4<T> abs(const Vector4<T>& v) {
		return Vector4<T>(std::abs(v.x), std::abs(v.y), std::abs(v.z), std::abs(v.w));
	}

	/*
	* 点乘函数
	*/
	template<typename T>
	inline T dot(const Vector2<T>& v1, const Vector2<T>& v2) {
		return v1.x * v2.x + v1.y * v2.y;
	}

	template<typename T>
	inline T dot(const Vector3<T>& v1, const Vector3<T>& v2) {
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	}

	template<typename T>
	inline T dot(const Vector4<T>& v1, const Vector4<T>& v2) {
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
	}

	/*
	* 叉乘函数
	*/
	template<typename T>
	inline T cross(const Vector2<T>& v1, const Vector2<T>& v2) {
		return v1.x * v2.y - v1.y * v2.x;
	}

	template<typename T>
	inline Vector3<T> cross(const Vector3<T>& v1, const Vector3<T>& v2) {
		double v1x = v1.x, v1y = v1.y, v1z = v1.z;
		double v2x = v2.x, v2y = v2.y, v2z = v2.z;

		return Vector3<T>(
			v1y * v2z - v1z * v2y,
			v1z * v2x - v1x * v2z,
			v1x * v2y - v1y * v2x
		);
	}

	/*
	* 求长度平方
	*/
	template<typename T>
	inline float lengthSquared(const Vector2<T>& v) {
		return v.x * v.x + v.y * v.y;
	}

	template<typename T>
	inline float lengthSquared(const Vector3<T>& v) {
		return v.x * v.x + v.y * v.y + v.z * v.z;
	}

	template<typename T>
	inline float lengthSquared(const Vector4<T>& v) {
		return v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w;
	}

	/*
	* 求长度
	*/
	template<typename T>
	inline T length(const Vector2<T>& v) {
		return std::sqrt(lengthSquared(v));
	}

	template<typename T>
	inline T length(const Vector3<T>& v) {
		return std::sqrt(lengthSquared(v));
	}

	template<typename T>
	inline T length(const Vector4<T>& v) {
		return std::sqrt(lengthSquared(v));
	}

	/*
	* 归一化
	*/
	template<typename T>
	inline Vector2<T> normalize(const Vector2<T>& v) {
		return v / length(v);
	}

	template<typename T>
	inline Vector3<T> normalize(const Vector3<T>& v) {
		return v / length(v);
	}

	template<typename T>
	inline Vector4<T> normalize(const Vector4<T>& v) {
		return v / length(v);
	}

}