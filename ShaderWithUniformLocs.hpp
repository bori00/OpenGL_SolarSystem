#define GLEW_STATIC
#ifndef ShaderWithUniformLocs_hpp
#define ShaderWithUniformLocs_hpp

#include <string>
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types
#include "glm/glm.hpp"
#include "Shader.hpp"
#include "Lighting.h"

namespace gps {
	class ShaderWithUniformLocs
	{
	public:
		void init(std::string vertex_shader_path, std::string fragment_shader_path);

		Shader* getShader();

		void sendModelUniform(glm::mat4 model);

		void sendViewUniform(glm::mat4 view);

		void sendProjectionUniform(glm::mat4 projection);

		void sendNormalMatrixUniform(glm::mat3 normal_matrix);

		void sendDirectionalLightUniform(view_layer::DirLight dirLight);

		void sendPointLightUniform(view_layer::PointLight pointLight, int lightIndex);

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

