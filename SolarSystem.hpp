#ifndef SolarSystem_hpp
#define SolarSystem_hpp

#include <memory>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>
#include "SpaceObjectView.hpp"
#include "Planet.hpp"
#include "TimedDrawable.h"
#include "Shader.hpp"
#include "Model3D.hpp"
#include "ShaderWithUniformLocs.hpp"
#include "Sun.h"
#include "Moon.h"
#include "GeometryShader.h"

namespace view_layer {
	class SolarSystem : public TimedDrawable
	{
	public:
		SolarSystem();

		void init(gps::ShaderWithUniformLocs* generic_shader_with_locs, gps::ShaderWithUniformLocs* sun_shader_with_locs, gps::ShaderWithUniformLocs* earth_shader_with_locs);

		void render(const glm::mat4* base_model, const glm::mat4* view, long long seconds);

		void renderWithDepthMapShader(const glm::mat4* base_model,
			const glm::mat4* view,
			long long current_seconds,
			gps::GeometryShader* geometryShader);

		bool hasLandedOnPlanet(long long current_seconds, glm::vec3 current_pos);

	private:
		std::vector<SpaceObjectView> space_objects_;
		SpaceObjectView* sun_view_;
		gps::ShaderWithUniformLocs* generic_shader_with_locs_;
		gps::ShaderWithUniformLocs* sun_shader_with_locs_;
		gps::ShaderWithUniformLocs* earth_shader_with_locs_;

		static const int NO_SECONDS_IN_HOUR = 3600;
		// the factor by which the distances between objects are smaller than the size of the objects, compared to the real distance/size ratio
		static const int DISTANCES_DIVIDER = 250; 
		static const int NO_SECONDS_IN_DAY = NO_SECONDS_IN_HOUR * 24;
		static const int EARTH_MOON_DIST_OFFSET = 20;
		static const int JUPITER_MOON_DIST_OFFSET = 90;
		static const int NO_JUPITER_MOONS = 8;

		void initPlanetsAndMoons();

		void initSun();
	};
}

#endif /* SolarSystem_hpp */