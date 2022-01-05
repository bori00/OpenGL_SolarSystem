#include "SpaceObjectView.hpp"

#include "Shader.hpp"

namespace view_layer {
	SpaceObjectView::SpaceObjectView(model_layer::TimedSpaceObject* space_object, std::string object_filename, gps::ShaderWithUniformLocs* shader_with_uniform_locs) 
			: space_object_(space_object), 
			  shader_with_uniform_locs_(shader_with_uniform_locs) {
		this->object_ = new gps::Model3D();
		this->object_->LoadModel(object_filename);
	}

	SpaceObjectView::SpaceObjectView(model_layer::TimedSpaceObject* space_object, gps::Model3D* object, gps::ShaderWithUniformLocs* shader_with_uniform_locs) 
			: space_object_(space_object),
			  object_(object),
			  shader_with_uniform_locs_(shader_with_uniform_locs) {}

	void SpaceObjectView::render(const glm::mat4* base_model, const glm::mat4* view, long long current_seconds) {
		glm::mat4 model =  (*base_model) * space_object_->getSpecificModel(current_seconds);

		shader_with_uniform_locs_->sendModelUniform(model);

		glm::mat3 normalMatrix = glm::mat3(glm::inverseTranspose(*view * model));

		shader_with_uniform_locs_->sendNormalMatrixUniform(normalMatrix);

		object_->Draw(*shader_with_uniform_locs_->getShader());
	}

	void SpaceObjectView::renderWithDepthMapShader(const glm::mat4* base_model,
		const glm::mat4* view,
		long long current_seconds,
		gps::GeometryShader* geometryShader) {
		glm::mat4 model = (*base_model) * space_object_->getSpecificModel(current_seconds);

		geometryShader->setMat4("model", model);

		object_->Draw(*geometryShader);
	}
}