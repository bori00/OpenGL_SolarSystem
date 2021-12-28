#include "SolarSystem.hpp"

namespace view_layer {
	SolarSystem::SolarSystem() {
		generic_shader_with_locs_ = nullptr;
	}

	void SolarSystem::init(gps::ShaderWithUniformLocs* generic_shader_with_locs) {
		generic_shader_with_locs_ = generic_shader_with_locs;
		initPlanets();
	}

	void SolarSystem::render(const glm::mat4* base_model, const glm::mat4* view, long long seconds) {
		for (auto& planet : planets_) {
			planet.render(base_model, view, seconds);
		}
	}

	void SolarSystem::initPlanets() {
		glm::vec3 mercuryPosition(57900 / DISTANCES_DIVIDER, 0, 0); // todo: multiply by 100
		glm::vec3 venusPosition(108200 / DISTANCES_DIVIDER, 0, 0);
		glm::vec3 earthPosition(149600 / DISTANCES_DIVIDER, 0, 0);
		glm::vec3 marsPosition(227900 / DISTANCES_DIVIDER, 0, 0);
		glm::vec3 jupiterPosition(778600 / DISTANCES_DIVIDER, 0, 0);
		glm::vec3 saturnPosition(1433500 / DISTANCES_DIVIDER, 0, 0);
		glm::vec3 uranusPosition(2872500 / DISTANCES_DIVIDER, 0, 0);
		glm::vec3 neptunePosition(4495100 / DISTANCES_DIVIDER, 0, 0);

		model_layer::Planet mercury_planet(mercuryPosition, 1408 * NO_SECONDS_IN_HOUR, glm::vec3(0, 1, 0), 88.0 * NO_SECONDS_IN_DAY, glm::vec3(0, 1, 0));
		model_layer::Planet venus_planet(venusPosition, 5832 * NO_SECONDS_IN_HOUR, glm::vec3(0, 1, 0), 224.7 * NO_SECONDS_IN_DAY, glm::vec3(0, 1, 0));
		model_layer::Planet earth_planet(earthPosition, 24 * NO_SECONDS_IN_HOUR, glm::vec3(0, 1, 0), 365.2 * NO_SECONDS_IN_DAY, glm::vec3(0, 1, 0));
		model_layer::Planet mars_planet(marsPosition, 25 * NO_SECONDS_IN_HOUR, glm::vec3(0, 1, 0), 687.0 * NO_SECONDS_IN_DAY, glm::vec3(0, 1, 0));
		model_layer::Planet jupiter_planet(jupiterPosition, 10 * NO_SECONDS_IN_HOUR, glm::vec3(0, 1, 0), 4331.0 * NO_SECONDS_IN_DAY, glm::vec3(0, 1, 0));
		model_layer::Planet saturn_planet(saturnPosition, 11 * NO_SECONDS_IN_HOUR, glm::vec3(0, 1, 0), 10747.0 * NO_SECONDS_IN_DAY, glm::vec3(0, 1, 0));
		model_layer::Planet uranus_planet(uranusPosition, 17 * NO_SECONDS_IN_HOUR, glm::vec3(0, 1, 0), 30589.0 * NO_SECONDS_IN_DAY, glm::vec3(0, 1, 0));
		model_layer::Planet neptune_planet(neptunePosition, 16 * NO_SECONDS_IN_HOUR, glm::vec3(0, 1, 0), 59800 * NO_SECONDS_IN_DAY, glm::vec3(0, 1, 0));

		view_layer::PlanetView mercury_view(mercury_planet, "models/mercury/mercury.obj", generic_shader_with_locs_);
		view_layer::PlanetView venus_view(venus_planet, "models/venus/venus.obj", generic_shader_with_locs_);
		view_layer::PlanetView earth_view(earth_planet, "models/earth/earth.obj", generic_shader_with_locs_);
		view_layer::PlanetView mars_view(mars_planet, "models/mars/mars.obj", generic_shader_with_locs_);
		view_layer::PlanetView jupiter_view(jupiter_planet, "models/jupiter/jupiter.obj", generic_shader_with_locs_);
		view_layer::PlanetView saturn_view(saturn_planet, "models/saturn/saturn.obj", generic_shader_with_locs_);
		view_layer::PlanetView uranus_view(uranus_planet, "models/uranus/uranus.obj", generic_shader_with_locs_);
		view_layer::PlanetView neptune_view(neptune_planet, "models/neptune/neptune.obj", generic_shader_with_locs_);

		planets_.push_back(mercury_view);
		planets_.push_back(venus_view);
		planets_.push_back(earth_view);
		planets_.push_back(mars_view);
		planets_.push_back(jupiter_view);
		planets_.push_back(saturn_view);
		planets_.push_back(uranus_view);
		planets_.push_back(neptune_view);
	}
}