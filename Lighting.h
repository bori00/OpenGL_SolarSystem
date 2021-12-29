#ifndef Lighting_hpp
#define Lighting_hpp

#include "glm/glm.hpp"

namespace view_layer {
	struct DirLight {
		glm::vec3 direction;

		glm::vec3 color;

		float ambientStrength;
        float diffuseStrength;
		float specularStrength;
	};

    struct PointLight {
        glm::vec3 position;

        float constant;
        float linear;
        float quadratic;

        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
    };
}

#endif /* Lighting_hpp*/