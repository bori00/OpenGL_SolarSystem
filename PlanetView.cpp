#include "PlanetView.hpp"

#include "Shader.hpp"

namespace view_layer {
	PlanetView::PlanetView(model_layer::Planet planet, std::string object_filename, gps::ShaderWithUniformLocs* shader_with_uniform_locs) 
			: planet_(planet), 
			  shader_with_uniform_locs_(shader_with_uniform_locs) {
		this->object_ = new gps::Model3D();
		this->object_->LoadModel(object_filename);
	}

	void PlanetView::render(const glm::mat4* base_model, const glm::mat4* view, long long current_seconds) {
		glm::mat4 model = planet_.getPlanetSpecificModel(current_seconds) * (*base_model);

		shader_with_uniform_locs_->sendModelUniform(model);

		glm::mat3 normalMatrix = glm::mat3(glm::inverseTranspose(*view * model));

		shader_with_uniform_locs_->sendNormalMatrixUniform(normalMatrix);

		object_->Draw(*shader_with_uniform_locs_->getShader());
	}
}