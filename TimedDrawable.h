#ifndef TimedDrawable_hpp
#define TimedDrawable_hpp

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "Shader.hpp"

namespace view_layer {
	class TimedDrawable
	{
	public:
		virtual void render(const glm::mat4* base_model, 
							const glm::mat4* view, 
							long long current_seconds) = 0;
	};
}

#endif /* TimedDrawable_hpp*/

