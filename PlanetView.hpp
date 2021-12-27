#ifndef PlanetView_hpp
#define PlanetView_hpp

#include <memory>
#include <string>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include "Planet.hpp"
#include "TimedDrawable.h"
#include "Shader.hpp"
#include "Model3D.hpp"

namespace view {
	class PlanetView : public TimedDrawable
	{
	public:
		PlanetView(model::Planet planet, std::string object_filename, gps::Shader* shader);

		void render(const glm::mat4* base_model, const glm::mat4* view, long long seconds, GLint model_loc, GLint normal_matrix_loc);

	private:
		model::Planet planet_;
		gps::Shader* shader_;
		gps::Model3D object_;
	};
}

#endif /* PlanetView_hpp */


