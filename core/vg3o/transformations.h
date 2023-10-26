#pragma once
#include "../ew/ewMath/ewMath.h"

namespace vg3o {
	inline float convertToRad(float deg) {
		return deg * (ew::PI / 180.0f);
	};

	// this function returns the sign of a number x
	// if x is 0 it will return 0, if x is negative it will return -1, and if x is positive it will return 1
	inline int sign(double x) { return x == 0 ? 0 : x / abs(x); };

	//Identity matrix
	inline ew::Mat4 Identity() {
		return ew::Mat4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		);
	};
	//Scale on x,y,z axes
	inline ew::Mat4 Scale(ew::Vec3 s) {
		return ew::Mat4(
			s.x, 0, 0, 0,
			0, s.y, 0, 0,
			0, 0, s.z, 0,
			0, 0, 0, 1
		);
	};
	//Rotation around X axis (pitch) in radians
	inline ew::Mat4 RotateX(float rad) {
		return ew::Mat4(
			1, 0, 0, 0,
			0, cos(rad), -sin(rad), 0,
			0, sin(rad), cos(rad), 0,
			0, 0, 0, 1
		);
	};
	//Rotation around Y axis (yaw) in radians
	inline ew::Mat4 RotateY(float rad) {
		return ew::Mat4(
			cos(rad), 0, sin(rad), 0,
			0, 1, 0, 0,
			-sin(rad), 0, cos(rad), 0,
			0, 0, 0, 1
		);
	};
	//Rotation around Z axis (roll) in radians
	inline ew::Mat4 RotateZ(float rad) {
		return ew::Mat4(
			cos(rad), -sin(rad), 0, 0,
			sin(rad), cos(rad), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		);
	};
	//Translate x,y,z
	inline ew::Mat4 Translate(ew::Vec3 t) {
		return ew::Mat4(
			1, 0, 0, t.x,
			0, 1, 0, t.y,
			0, 0, 1, t.z,
			0, 0, 0, 1
		);
	};

	struct Transform {
		ew::Vec3 position = ew::Vec3(0.0f, 0.0f, 0.0f);
		ew::Vec3 rotation = ew::Vec3(0.0f, 0.0f, 0.0f);
		ew::Vec3 scale = ew::Vec3(1.0f, 1.0f, 1.0f);
		ew::Mat4 getModelMatrix() const {
			return vg3o::Translate(position) * vg3o::RotateY(rotation.y) * vg3o::RotateX(rotation.x) * vg3o::RotateZ(rotation.z) * vg3o::Scale(scale);
		}
	};

	inline ew::Mat4 LookAt(ew::Vec3 eye, ew::Vec3 target, ew::Vec3 up)
	{
		// calculate basis vectors
		ew::Vec3 diff = eye - target;
		
		ew::Vec3 f = ew::Normalize(diff); 
		ew::Vec3 r = ew::Normalize(ew::Cross(up, f));
		ew::Vec3 u = ew::Normalize(ew::Cross(f, r));

		// view matrix
		return ew::Mat4(
			r.x, r.y, r.z, -ew::Dot(r, eye),
			u.x, u.y, u.z, -ew::Dot(u, eye),
			f.x, f.y, f.z, -ew::Dot(f, eye),
			0, 0, 0, 1
		);
	};

	inline ew::Mat4 Orthographic(float height, float aspect, float near, float far)
	{
		float width = aspect * height;

		float right = width / 2;
		float left = -right;
		float top = height / 2;
		float bottom = -top;

		return ew::Mat4(
			2 / (right - left), 0, 0, -((right + left) / (right - left)),
			0, 2 / (top - bottom), 0, -((top + bottom) / (top - bottom)),
			0, 0, -(2 / (far - near)), -((far + near) / (far - near)),
			0, 0, 0, 1
		);
	};

	inline ew::Mat4 Perspective(float fov, float aspect, float near, float far)
	{
		return ew::Mat4(
			(1 / (tan(fov / 2) * aspect)), 0, 0, 0,
			0, (1 / tan(fov / 2)), 0, 0,
			0, 0, (near + far) / (near - far), (2 * far * near) / (near - far),
			0, 0, -1, 0
		);
	};
}
