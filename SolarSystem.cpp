#include "SolarSystem.hpp"

namespace view_layer {
	SolarSystem::SolarSystem() {
		generic_shader_with_locs_ = nullptr;
	}

	void SolarSystem::init(gps::ShaderWithUniformLocs* generic_shader_with_locs, gps::ShaderWithUniformLocs* sun_shader_with_locs, gps::ShaderWithUniformLocs* earth_shader_with_locs) {
		generic_shader_with_locs_ = generic_shader_with_locs;
		sun_shader_with_locs_ = sun_shader_with_locs;
		earth_shader_with_locs_ = earth_shader_with_locs;
		initPlanetsAndMoons();
		initSun();
	}

	void SolarSystem::render(const glm::mat4* base_model, const glm::mat4* view, long long seconds) {
		sun_view_->render(base_model, view, seconds);
		for (auto& planet : space_objects_) {
			planet.render(base_model, view, seconds);
		}
	}

	void SolarSystem::renderWithDepthMapShader(const glm::mat4* base_model,
		const glm::mat4* view,
		long long current_seconds,
		gps::GeometryShader* geometryShader) {
		for (auto& planet : space_objects_) {
			planet.renderWithDepthMapShader(base_model, view, current_seconds, geometryShader);
		}
	}

	void SolarSystem::initPlanetsAndMoons() {
		glm::vec3 mercuryPosition(57900 / DISTANCES_DIVIDER, 0, 0); // todo: multiply by 100
		glm::vec3 venusPosition(108200 / DISTANCES_DIVIDER, 0, 0);
		glm::vec3 earthPosition(149600 / DISTANCES_DIVIDER, 0, 0);
		glm::vec3 marsPosition(227900 / DISTANCES_DIVIDER, 0, 0);
		glm::vec3 jupiterPosition(778600 / DISTANCES_DIVIDER, 0, 0);
		glm::vec3 saturnPosition(1433500 / DISTANCES_DIVIDER, 0, 0);
		glm::vec3 uranusPosition(2872500 / DISTANCES_DIVIDER, 0, 0);
		glm::vec3 neptunePosition(4495100 / DISTANCES_DIVIDER, 0, 0);

		// TODO: resolve memory leak
		model_layer::Planet* mercury_planet = new model_layer::Planet(mercuryPosition, 1408 * NO_SECONDS_IN_HOUR, glm::vec3(0, 1, 0), 88 * NO_SECONDS_IN_DAY, glm::vec3(0, 1, 0), 2.44);
		model_layer::Planet* venus_planet = new model_layer::Planet(venusPosition, 5832 * NO_SECONDS_IN_HOUR, glm::vec3(0, 1, 0), 224.7 * NO_SECONDS_IN_DAY, glm::vec3(0, 1, 0), 6.052);
		model_layer::Planet* earth_planet = new model_layer::Planet(earthPosition, 24 * NO_SECONDS_IN_HOUR, glm::vec3(0, 1, 0), 365.2 * NO_SECONDS_IN_DAY, glm::vec3(0, 1, 0), 6.371);
		model_layer::Planet* mars_planet = new model_layer::Planet(marsPosition, 25 * NO_SECONDS_IN_HOUR, glm::vec3(0, 1, 0), 687.0 * NO_SECONDS_IN_DAY, glm::vec3(0, 1, 0), 3.390);
		model_layer::Planet* jupiter_planet = new model_layer::Planet(jupiterPosition, 10 * NO_SECONDS_IN_HOUR, glm::vec3(0, 1, 0), 4331.0 * NO_SECONDS_IN_DAY, glm::vec3(0, 1, 0), 69.911);
		model_layer::Planet* saturn_planet = new model_layer::Planet(saturnPosition, 11 * NO_SECONDS_IN_HOUR, glm::vec3(0, 1, 0), 10747.0 * NO_SECONDS_IN_DAY, glm::vec3(0, 1, 0), 58.232);
		model_layer::Planet* uranus_planet = new model_layer::Planet(uranusPosition, 17 * NO_SECONDS_IN_HOUR, glm::vec3(0, 1, 0), 30589.0 * NO_SECONDS_IN_DAY, glm::vec3(0, 1, 0), 25.362);
		model_layer::Planet* neptune_planet = new model_layer::Planet(neptunePosition, 16 * NO_SECONDS_IN_HOUR, glm::vec3(0, 1, 0), 59800.0 * NO_SECONDS_IN_DAY, glm::vec3(0, 1, 0), 24.622);

		view_layer::SpaceObjectView mercury_view(mercury_planet, "models/mercury/mercury.obj", generic_shader_with_locs_);
		view_layer::SpaceObjectView venus_view(venus_planet, "models/venus/venus.obj", generic_shader_with_locs_);
		view_layer::SpaceObjectView earth_view(earth_planet, "models/earth/earth.obj", earth_shader_with_locs_);
		view_layer::SpaceObjectView mars_view(mars_planet, "models/mars/mars.obj", generic_shader_with_locs_);
		view_layer::SpaceObjectView jupiter_view(jupiter_planet, "models/jupiter/jupiter.obj", generic_shader_with_locs_);
		view_layer::SpaceObjectView saturn_view(saturn_planet, "models/saturn/saturn.obj", generic_shader_with_locs_);
		view_layer::SpaceObjectView uranus_view(uranus_planet, "models/uranus/uranus.obj", generic_shader_with_locs_);
		view_layer::SpaceObjectView neptune_view(neptune_planet, "models/neptune/neptune.obj", generic_shader_with_locs_);

		space_objects_.push_back(mercury_view);
		space_objects_.push_back(venus_view);
		space_objects_.push_back(earth_view);
		space_objects_.push_back(mars_view);
		space_objects_.push_back(jupiter_view);
		space_objects_.push_back(saturn_view);
		space_objects_.push_back(uranus_view);
		space_objects_.push_back(neptune_view);

		// Moons
		// 1 moon for the Earth
		glm::vec3 earthMoonPosition(384 / DISTANCES_DIVIDER + EARTH_MOON_DIST_OFFSET, 0, 0);
		model_layer::Moon* earth_moon = new model_layer::Moon(earthMoonPosition, 27 * NO_SECONDS_IN_DAY, glm::vec3(0, 1, 0), 27.322 * NO_SECONDS_IN_DAY, glm::vec3(0, 1, 0), earth_planet, 1.737);
		view_layer::SpaceObjectView earth_moon_view(earth_moon, "models/earth_moon/earth_moon.obj", generic_shader_with_locs_);
		space_objects_.push_back(earth_moon_view);
		// 4 moons for Jupiter (at randomized positions, speed, etc.)
		gps::Model3D* jupiter_io_moon_model = new gps::Model3D();
		jupiter_io_moon_model->LoadModel("models/jupiter_io_moon/jupiter_io_moon.obj");
		for (int i = 0; i < NO_JUPITER_MOONS; i++) {
			double sign = i % 2 == 0 ? -1 : 1;
			glm::vec3 jupMoonPosition(sign * (JUPITER_MOON_DIST_OFFSET + i * 10), 0, 0);
			model_layer::Moon* jupiter_moon = new model_layer::Moon(jupMoonPosition, (1.7 + i * 5) * NO_SECONDS_IN_DAY, glm::vec3(0, 1, 0), 2 * (i + 1) * NO_SECONDS_IN_DAY, glm::vec3(glm::sin(i*10+10), glm::cos(i*10+10), glm::sin(i*10+10)), jupiter_planet, 3.643);
			view_layer::SpaceObjectView jupiter_moon_view(jupiter_moon, jupiter_io_moon_model, generic_shader_with_locs_);
			space_objects_.push_back(jupiter_moon_view);
		}
	}

	void SolarSystem::initSun() {
		glm::vec3 sunPosition(0, 0, 0);
		// TODO: solve memory leak
		model_layer::Sun* sun = new model_layer::Sun(sunPosition, 27 * NO_SECONDS_IN_DAY, glm::vec3(0, 1, 0), 174.085);

		view_layer::SpaceObjectView* sun_view = new SpaceObjectView(sun, "models/sun/sun.obj", sun_shader_with_locs_);

		sun_view_ = sun_view;
	}

	bool SolarSystem::landed_on_planet(long long current_seconds, glm::vec3 current_pos) {
		int i = 0;
		for (auto& planet : space_objects_) {
			if (length(planet.getCurrentPosition(current_seconds) - current_pos) < planet.getRadius()) {
				return true;
			}
			// printf("Distance from %d:  %f\n", length(planet.getCurrentPosition(current_seconds) - current_pos), i);
			i++;
		}
		return false;
	}
}