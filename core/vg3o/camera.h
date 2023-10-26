#pragma once
#include "../ew/ewMath/ewMath.h"
#include "../vg3o/transformations.h"

namespace vg3o{ 
	struct Camera 
	{
		ew::Vec3 position;
		ew::Vec3 target;
		float fov;
		float aspectRatio;
		float nearPlane;
		float farPlane;
		bool orthographic;
		float orthoSize;
		ew::Mat4 ViewMatrix() 
		{
			return vg3o::LookAt(position, target, ew::Vec3(0, 1, 0));
		};
		ew::Mat4 ProjectionMatrix()
		{
			if (orthographic)
			{
				return vg3o::Orthographic(orthoSize, aspectRatio, nearPlane, farPlane);
			}
			else
			{
				return vg3o::Perspective(ew::Radians(fov), aspectRatio, nearPlane, farPlane);
			}
		};
	};

	struct CameraControls
	{
		double previousX, previousY;
		float yaw = -90, pitch = 0; // this is in degrees not radians
		bool firstMouse = true;
		float speed = 5.0f;
		float sensitivity = 0.05f;
	};
}