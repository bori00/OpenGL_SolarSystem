#ifndef PlanetView_hpp
#define PlanetView_hpp

#include <memory>
#include <string>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include "TimedDrawable.h"
#include "Shader.hpp"
#include "Model3D.hpp"
#include "ShaderWithUniformLocs.hpp"
#include "TimedSpaceObject.h"
#include "GeometryShader.h"

namespace view_layer {
	class SpaceObjectView : public TimedDrawable
	{
	public:
		SpaceObjectView(model_layer::TimedSpaceObject* space_object, std::string object_filename, gps::ShaderWithUniformLocs* shader_with_uniform_locs);

		SpaceObjectView(model_layer::TimedSpaceObject* space_object, gps::Model3D* object, gps::ShaderWithUniformLocs* shader_with_uniform_locs);

		void render(const glm::mat4* base_model, const glm::mat4* view, long long seconds);

		void renderWithDepthMapShader(const glm::mat4* base_model,
			const glm::mat4* view,
			long long current_seconds,
			gps::GeometryShader* geometryShader);

	private:
		model_layer::TimedSpaceObject* space_object_;
		gps::ShaderWithUniformLocs* shader_with_uniform_locs_;
		gps::Model3D* object_;
	};
}

#endif /* PlanetView_hpp */


