/*
	Written by Brandon Salvietti

	cutscene.h
		Used to handle camera cutscenes.
*/

#include "../ew/ewMath/ewMath.h"
#include <vector>
#include <variant>


namespace vg3o {
	
	enum InterpolationType {
		LINEAR = 1,
		QUADRATIC = 2,
		CUBIC = 3,
		QUARTIC = 4,
		QUINTIC = 5
	};

	template<typename T>
	struct CutsceneEvent {
		T targetValue;
		T startValue;
		T* valueReference;
		
		float startTime = 0.0f;
		float duration = 1.0f;
		InterpolationType style = LINEAR;

		void UpdateTweenValue(float alpha) {
			T diff = targetValue - startValue;
			alpha = ew::Clamp(alpha, 0.0f, 1.0f);
			valueReference = startValue + (diff * pow(alpha, style));
		};
	};

	class Cutscene {
	public:
		Cutscene(float duration) { mDuration = duration; };
		void AddEvent(CutsceneEvent<float> event) { mValueEvents.push_back(event); };
		void AddEvent(CutsceneEvent<ew::Vec3> event) { mCameraEvents.push_back(event); };
		
		void Update(float deltaTime) {
			mTime += deltaTime;

			for (int i = 0; i < mCameraEvents.size(); i++) {

			}
			for (int i = 0; i < mValueEvents.size(); i++) {

			}
		};
	private:
		std::vector<CutsceneEvent<float>> mValueEvents;
		std::vector<CutsceneEvent<ew::Vec3>> mCameraEvents;
		float mTime = 0.0f;
		float mDuration;
	};
}