/*
	Written by Brandon Salvietti

	tween.h
		Used for smoothening and animations.
*/

#include "../ew/ewMath/ewMath.h"

namespace vg3o {
	
	enum InterpolationType {
		LINEAR = 1,
		QUADRATIC = 2,
		CUBIC = 3,
		QUARTIC = 4,
		QUINTIC = 5
	};


	/// <summary>
	/// Gets a tween value using specified interpolation style.
	/// </summary>
	/// <param name="start">The starting value.</param>
	/// <param name="goal">The target value.</param>
	/// <param name="alpha">A value between 0 and 1 that determines where on the line the value returned lies.</param>
	/// <param name="style">InterpolationType enum that specifies what type of interpolation function to use.</param>
	/// <param name="reverses">Whether or not to reverse the calculation. Is defaulted to false.</param>
	/// <returns>A float containing the interpolated value.</returns>
	inline float GetTweenValue(float start, float goal, float alpha, InterpolationType style, bool reverses = false) {
		float diff = goal - start;
		if (reverses) { alpha = 1.f - alpha; }
		return start + (diff * pow(alpha, style));
	};
}