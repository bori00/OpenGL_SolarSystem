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
#include "ShaderWithUniformLocs.hpp"

namespace view_layer {
	class SolarSystem : public TimedDrawable
	{
	public:
		SolarSystem();

		void init(gps::ShaderWithUniformLocs* generic_shader_with_locs);

		void render(const glm::mat4* base_model, const glm::mat4* view, long long seconds);

	private:
		std::vector<PlanetView> planets_;
		gps::ShaderWithUniformLocs* generic_shader_with_locs_;

		static const int NO_SECONDS_IN_HOUR = 3600;

		void initPlanets();
	};
}

#endif /* SolarSystem_hpp */