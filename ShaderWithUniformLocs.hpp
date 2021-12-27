#ifndef ShaderWithUniformLocs_hpp
#define ShaderWithUniformLocs_hpp

#include "Shader.hpp"
#include <string>
#include <GL/glew.h>
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace gps {
	class ShaderWithUniformLocs
	{
	public:
		ShaderWithUniformLocs(std::string vertex_shader_path, std::string fragment_shader_path);

		Shader getShader();

		void sendModelUniform(glm::mat4 model);

		void sendViewUniform(glm::mat4 view);

		void sendProjectionUniform(glm::mat4 projection);

		void sendNormalMatrixUniform(glm::mat3 normal_matrix);

		void sendLightDirUniform(glm::vec3 light_dir);

		void sendLightColorUniform(glm::vec3 light_color);

	private:
		Shader shader_;
		GLint model_loc_;
		GLint view_loc_;
		GLint projection_loc_;
		GLint normal_matrix_loc_;
		GLint light_dir_loc_;
		GLint light_color_loc_;

		void initUniforms();
	};
}

#endif /* ShaderWithUniformLocs_hpp*/

