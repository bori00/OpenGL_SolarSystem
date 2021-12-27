#include "ShaderWithUniformLocs.hpp"

namespace gps {
	ShaderWithUniformLocs::ShaderWithUniformLocs(std::string vertex_shader_path, std::string fragment_shader_path) {
		shader_.loadShader(
			vertex_shader_path,
			fragment_shader_path);
		initUniforms();
	}

	Shader ShaderWithUniformLocs::getShader() {
		return shader_;
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

	void ShaderWithUniformLocs::sendLightDirUniform(glm::vec3 light_dir) {
		shader_.useShaderProgram();
		glUniform3fv(light_dir_loc_, 1, glm::value_ptr(light_dir));
	}

	void ShaderWithUniformLocs::sendLightColorUniform(glm::vec3 light_color) {
		shader_.useShaderProgram();
		glUniform3fv(light_color_loc_, 1, glm::value_ptr(light_color));
	}

	void ShaderWithUniformLocs::initUniforms() {
		shader_.useShaderProgram();

		model_loc_ = glGetUniformLocation(shader_.shaderProgram, "model");
		view_loc_ = glGetUniformLocation(shader_.shaderProgram, "view");
		normal_matrix_loc_ = glGetUniformLocation(shader_.shaderProgram, "normalMatrix");
		projection_loc_ = glGetUniformLocation(shader_.shaderProgram, "projection");
		light_dir_loc_ = glGetUniformLocation(shader_.shaderProgram, "lightDir");
		light_color_loc_ = glGetUniformLocation(shader_.shaderProgram, "lightColor");
	}
}