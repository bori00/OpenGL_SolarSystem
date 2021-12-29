#ifndef TimedSpaceObject_hpp
#define TimedSpaceObject_hpp

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "Shader.hpp"

namespace model_layer {
	class TimedSpaceObject
	{
	public:
		virtual glm::mat4 getSpecificModel(long long current_seconds) = 0;

		virtual glm::vec3 getCurrentPosition(long long current_seconds) = 0;
	};
}

#endif /* TimedSPaceObject_hpp*/