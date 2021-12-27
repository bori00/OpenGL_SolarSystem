#include "PlanetView.hpp"

namespace view {
	PlanetView::PlanetView(model::Planet planet, std::string object_filename, gps::Shader* shader) 
			: planet_(planet), 
			  shader_(shader) {
		this->object_.LoadModel(object_filename);
	}

	void PlanetView::render(const glm::mat4* base_model, const glm::mat4* view, long long current_seconds, GLint model_loc, GLint normal_matrix_loc) {
		glm::mat4 model = planet_.getPlanetSpecificModel(current_seconds) * (*base_model);

		shader_->useShaderProgram();

		glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));

		glm::mat3 normalMatrix = glm::mat3(glm::inverseTranspose(*view * model));

		glUniformMatrix3fv(normal_matrix_loc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

		object_.Draw(*shader_);
	}
}