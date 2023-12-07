/*
	Written by Brandon Salvietti

	cutscene.h
		Used to handle camera cutscenes.
*/

#include "../ew/ewMath/ewMath.h"
#include <vector>


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
			*valueReference = startValue + (diff * pow(alpha, style));
		};
	};

	class Cutscene {
	public:
		Cutscene(float duration) { mDuration = duration; std::cout << mTime; };
		void AddEvent(CutsceneEvent<float> event) { mValueEvents.push_back(event); };
		void AddEvent(CutsceneEvent<ew::Vec3> event) { mCameraEvents.push_back(event); };
		
		void Update(float deltaTime) {
			// (mTime >= mDuration) { return; }
			mTime += deltaTime;
			/*
			for (int i = 0; i < mCameraEvents.size(); i++) {
				if (mCameraEvents[i].startTime > mTime) { continue; }
				float alpha = (mTime - mCameraEvents[i].startTime) / mCameraEvents[i].duration;
				mCameraEvents[i].UpdateTweenValue(alpha);
			}*/
			for (int i = 0; i < mValueEvents.size(); i++) {
				if (mValueEvents[i].startTime > mTime) { continue; }
				float alpha = (mTime - mValueEvents[i].startTime) / mValueEvents[i].duration;
				mValueEvents[i].UpdateTweenValue(alpha);
			}
		};
		void PrintTime() { std::cout << "Time: " << mTime << std::endl; };
		void Reset() { mTime = 0.0f; };
	private:
		std::vector<CutsceneEvent<float>> mValueEvents;
		std::vector<CutsceneEvent<ew::Vec3>> mCameraEvents;
		float mTime = 0.0f;
		float mDuration;
	};
}