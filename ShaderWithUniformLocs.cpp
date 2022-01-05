#include "ShaderWithUniformLocs.hpp"

namespace gps {

	void ShaderWithUniformLocs::init(std::string vertex_shader_path, std::string fragment_shader_path) {
		shader_.loadShader(
			vertex_shader_path,
			fragment_shader_path);
		this->initUniforms();
	}

	Shader* ShaderWithUniformLocs::getShader() {
		return &shader_;
	}

	void ShaderWithUniformLocs::sendModelUniform(glm::mat4 model) {
		shader_.useShaderProgram();
		glUniformMatrix4fv(model_loc_, 1, GL_FALSE, glm::value_ptr(model));
	}

	void ShaderWithUniformLocs::sendViewUniform(glm::mat4 view) {
		shader_.useShaderProgram();
		glUniformMatrix4fv(view_loc_, 1, GL_FALSE, glm::value_ptr(view));
	}

	void ShaderWithUniformLocs::sendProjectionUniform(glm::mat4 projection) {
		shader_.useShaderProgram();
		glUniformMatrix4fv(projection_loc_, 1, GL_FALSE, glm::value_ptr(projection));
	}

	void ShaderWithUniformLocs::sendNormalMatrixUniform(glm::mat3 normal_matrix) {
		shader_.useShaderProgram();
		glUniformMatrix3fv(normal_matrix_loc_, 1, GL_FALSE, glm::value_ptr(normal_matrix));
	}

	void ShaderWithUniformLocs::sendDirectionalLightUniform(view_layer::DirLight dirLight) {
		shader_.useShaderProgram();
		glUniform3fv(glGetUniformLocation(shader_.shaderProgram, "dirLight.direction"), 1, glm::value_ptr(dirLight.direction));
		glUniform3fv(glGetUniformLocation(shader_.shaderProgram, "dirLight.color"), 1, glm::value_ptr(dirLight.color));
		glUniform1f(glGetUniformLocation(shader_.shaderProgram, "dirLight.ambientStrength"), dirLight.ambientStrength);
		glUniform1f(glGetUniformLocation(shader_.shaderProgram, "dirLight.diffuseStrength"), dirLight.diffuseStrength);
		glUniform1f(glGetUniformLocation(shader_.shaderProgram, "dirLight.specularStrength"), dirLight.specularStrength);
	}

	void ShaderWithUniformLocs::sendSunLightUniform(view_layer::PointLight sunLight) {
		shader_.useShaderProgram();
		glUniform3fv(glGetUniformLocation(shader_.shaderProgram, "sunLight.position"), 1, glm::value_ptr(sunLight.position));

		glUniform1f(glGetUniformLocation(shader_.shaderProgram, "sunLight.constant"), sunLight.constant);
		glUniform1f(glGetUniformLocation(shader_.shaderProgram, "sunLight.linear"), sunLight.linear);
		glUniform1f(glGetUniformLocation(shader_.shaderProgram, "sunLight.quadratic"), sunLight.quadratic);

		glUniform3fv(glGetUniformLocation(shader_.shaderProgram, "sunLight.ambient"), 1, glm::value_ptr(sunLight.ambient));
		glUniform3fv(glGetUniformLocation(shader_.shaderProgram, "sunLight.diffuse"), 1, glm::value_ptr(sunLight.diffuse));
		glUniform3fv(glGetUniformLocation(shader_.shaderProgram, "sunLight.specular"), 1, glm::value_ptr(sunLight.specular));
	}

	void ShaderWithUniformLocs::initUniforms() {
		shader_.useShaderProgram();

		model_loc_ = glGetUniformLocation(shader_.shaderProgram, "model");
		view_loc_ = glGetUniformLocation(shader_.shaderProgram, "view");
		normal_matrix_loc_ = glGetUniformLocation(shader_.shaderProgram, "normalMatrix");
		projection_loc_ = glGetUniformLocation(shader_.shaderProgram, "projection");
	}
}