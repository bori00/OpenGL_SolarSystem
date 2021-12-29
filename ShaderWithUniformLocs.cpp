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

	void ShaderWithUniformLocs::sendPointLightUniform(view_layer::PointLight pointLight, int lightIndex) {
		shader_.useShaderProgram();
		std::string lightIndexString = std::to_string(lightIndex);
		glUniform3fv(glGetUniformLocation(shader_.shaderProgram, ("pointLights[" + lightIndexString + "].position").c_str()), 1, glm::value_ptr(pointLight.position));

		glUniform1f(glGetUniformLocation(shader_.shaderProgram, ("pointLights[" + lightIndexString + "].constant").c_str()), pointLight.constant);
		glUniform1f(glGetUniformLocation(shader_.shaderProgram, ("pointLights[" + lightIndexString + "].linear").c_str()), pointLight.linear);
		glUniform1f(glGetUniformLocation(shader_.shaderProgram, ("pointLights[" + lightIndexString + "].quadratic").c_str()), pointLight.quadratic);

		glUniform3fv(glGetUniformLocation(shader_.shaderProgram, ("pointLights[" + lightIndexString + "].ambient").c_str()), 1, glm::value_ptr(pointLight.ambient));
		glUniform3fv(glGetUniformLocation(shader_.shaderProgram, ("pointLights[" + lightIndexString + "].diffuse").c_str()), 1, glm::value_ptr(pointLight.diffuse));
		glUniform3fv(glGetUniformLocation(shader_.shaderProgram, ("pointLights[" + lightIndexString + "].specular").c_str()), 1, glm::value_ptr(pointLight.specular));
	}

	void ShaderWithUniformLocs::initUniforms() {
		shader_.useShaderProgram();

		model_loc_ = glGetUniformLocation(shader_.shaderProgram, "model");
		view_loc_ = glGetUniformLocation(shader_.shaderProgram, "view");
		normal_matrix_loc_ = glGetUniformLocation(shader_.shaderProgram, "normalMatrix");
		projection_loc_ = glGetUniformLocation(shader_.shaderProgram, "projection");
	}
}