#ifndef Lighting_hpp
#define Lighting_hpp

#include "glm/glm.hpp"

namespace view_layer {
	struct DirLight {
		glm::vec3 direction;

		glm::vec3 color;

		float ambientStrength;
		float specularStrength;
	};
}

#endif /* Lighting_hpp*/