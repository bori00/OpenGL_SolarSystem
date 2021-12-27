#ifndef SolarSystem_hpp
#define SolarSystem_hpp

#include <memory>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>
#include "PlanetView.hpp"
#include "Planet.hpp"
#include "TimedDrawable.h"
#include "Shader.hpp"
#include "Model3D.hpp"

namespace view {
	class SolarSystem : public TimedDrawable
	{
	public:
		SolarSystem();

		void render(const glm::mat4* base_model, const glm::mat4* view, long long seconds, GLint model_loc, GLint normal_matrix_loc);

	private:
		std::vector<PlanetView> planets_;
		gps::Shader generic_shader_;

		void initPlanets();
	};
}

#endif /* SolarSystem_hpp */