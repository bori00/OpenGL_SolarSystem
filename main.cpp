#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp> //core glm functionality
#include <glm/gtc/matrix_transform.hpp> //glm extension for generating common transformation matrices
#include <glm/gtc/matrix_inverse.hpp> //glm extension for computing inverse matrices
#include <glm/gtc/type_ptr.hpp> //glm extension for accessing the internal data structure of glm types

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "ShaderWithUniformLocs.hpp"
#include "SolarSystem.hpp"
#include "SkyBox.hpp"
#include "Lighting.h"

#include <iostream>

// window
gps::Window myWindow;

// matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;

// camera
gps::Camera myCamera(
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(2000.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));

GLfloat cameraMoveSpeed = 1000.0f;
GLfloat cameraRotationSpeed = 10.0f;

// event handling
GLboolean pressedKeys[1024];

// angle of rotation, for the model
GLfloat angle;

// shaders
gps::ShaderWithUniformLocs myShaderWithLocs;
gps::ShaderWithUniformLocs skyboxShaderWithLocs;

// solar system
view_layer::SolarSystem solarSystem;

// timing
float deltaTimeSeconds = 0;
double lastTimeStamp = glfwGetTime();
double currentTimeStamp = 0;
double simulationTimeStamp = 0;
bool stopped = false;
bool prev_enter_pressed = false;

// skybox
std::vector<const GLchar*> skybox_faces;
gps::SkyBox mySkyBox;

// lighting
// white directional light
view_layer::DirLight dirLight = {/*direction*/ glm::vec3(-1.0f, -1.0f, 0.0f), 
                                /*.color= */ glm::vec3(1.0f, 1.0f, 1.0f), 
                                /*.ambientStrength =*/ 0.2, 
                                /*.specularStrength =*/ 0.0 };
// yellow-ish sun light (positional)
view_layer::PointLight sunLight = {
    /* .position= */ glm::vec3(0, 0, 0),
    /* .constant= */ 1.0,
    /* .linear= */   0.0014,
    /* .quadratic=*/ 0.0000007,
    /* .ambient = */ glm::vec3(1.0, 0.952, 0.741),
    /* .diffuse = */ glm::vec3(1.0, 0.952, 0.741),
    /* .specular= */ glm::vec3(1.0, 0.952, 0.741),
};

// speed
const double REAL_SECOND_TO_ANIMATION_SECONDS = 3600 * 24 * 3.65; // 1s in real life corresponds to 3600s=1h in the animation
// (as a consequence, for example, it will take 1 seconds for the Earth to perform a full rotation, and 365 seconds to perform an orbital rotation)

void updateDelta() {
    lastTimeStamp = currentTimeStamp;
    currentTimeStamp = glfwGetTime();
    deltaTimeSeconds = currentTimeStamp - lastTimeStamp;
    if (!stopped) {
        simulationTimeStamp += deltaTimeSeconds;
    }
}

GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR) {
		std::string error;
		switch (errorCode) {
            case GL_INVALID_ENUM:
                error = "INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                error = "INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                error = "INVALID_OPERATION";
                break;
            case GL_STACK_OVERFLOW:
                error = "STACK_OVERFLOW";
                break;
            case GL_STACK_UNDERFLOW:
                error = "STACK_UNDERFLOW";
                break;
            case GL_OUT_OF_MEMORY:
                error = "OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error = "INVALID_FRAMEBUFFER_OPERATION";
                break;
        }
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void updateProjection() {
    projection = glm::perspective(glm::radians(55.0f),
        (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
        0.1f, 1000000.0f);
    fprintf(stdout, "Window resized! New width: %d , and height: %d\n", myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    myShaderWithLocs.sendProjectionUniform(projection);
    skyboxShaderWithLocs.sendProjectionUniform(projection);
}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);
    myWindow.setWindowDimensions({width, height});
    glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    updateProjection();
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

	if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            pressedKeys[key] = true;
        } else if (action == GLFW_RELEASE) {
            pressedKeys[key] = false;
        }
    }
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    //TODO
}

void processMovement() {
    if (pressedKeys[GLFW_KEY_W]) {
        myCamera.move(gps::MOVE_FORWARD, cameraMoveSpeed * deltaTimeSeconds);
    }

    if (pressedKeys[GLFW_KEY_S]) {
        myCamera.move(gps::MOVE_BACKWARD, cameraMoveSpeed * deltaTimeSeconds);
    }

    if (pressedKeys[GLFW_KEY_A]) {
        myCamera.move(gps::MOVE_LEFT, cameraMoveSpeed * deltaTimeSeconds);
    }

    if (pressedKeys[GLFW_KEY_D]) {
        myCamera.move(gps::MOVE_RIGHT, cameraMoveSpeed * deltaTimeSeconds);
    }

    if (pressedKeys[GLFW_KEY_Q]) {
        angle -= 1.0f * deltaTimeSeconds;
        // update model matrix
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
    }

    if (pressedKeys[GLFW_KEY_E]) {
        angle += 1.0f * deltaTimeSeconds;
        // update model matrix
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
    }

    if (pressedKeys[GLFW_KEY_Y]) {
        myCamera.rotate(0, -cameraRotationSpeed * deltaTimeSeconds);
    }
    if (pressedKeys[GLFW_KEY_T]) {
        myCamera.rotate(0, cameraRotationSpeed * deltaTimeSeconds);
    }

    if (pressedKeys[GLFW_KEY_P]) {
        myCamera.rotate(cameraRotationSpeed * deltaTimeSeconds, 0);
    }
    if (pressedKeys[GLFW_KEY_L]) {
        myCamera.rotate(-cameraRotationSpeed * deltaTimeSeconds, 0);
    }
    if (pressedKeys[GLFW_KEY_ENTER]) {
        if (!prev_enter_pressed) {
            stopped = !stopped;
            prev_enter_pressed = true;
        }
    }
    else {
        prev_enter_pressed = false;
    }
}

void initOpenGLWindow() {
    myWindow.Create(1024, 768, "Solar System");
}

void setWindowCallbacks() {
	glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
    glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
    glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
}

void initOpenGLState() {
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initModels() {
   solarSystem.init(&myShaderWithLocs);
}

void initShaders() {
    myShaderWithLocs.init("shaders/basic.vert", "shaders/basic.frag");
    skyboxShaderWithLocs.init("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
}

void initSkyBox() {
    skybox_faces.push_back("models/space_skybox/right.png");
    skybox_faces.push_back("models/space_skybox/left.png");
    skybox_faces.push_back("models/space_skybox/bot.png");
    skybox_faces.push_back("models/space_skybox/top.png");
    skybox_faces.push_back("models/space_skybox/back.png");
    skybox_faces.push_back("models/space_skybox/front.png");
    mySkyBox.Load(skybox_faces);
}

void initUniforms() {
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));

    // get view matrix for current camera
    view = myCamera.getViewMatrix();
    myShaderWithLocs.sendViewUniform(view);
    skyboxShaderWithLocs.sendViewUniform(view);

    // compute normal matrix
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

    // create projection matrix
    updateProjection();

    //set the directional light
    myShaderWithLocs.sendDirectionalLightUniform(dirLight);

    // set the sun light
    myShaderWithLocs.sendPointLightUniform(sunLight, 0);
}

void updateView() {
    //update view matrix
    view = myCamera.getViewMatrix();

    myShaderWithLocs.sendViewUniform(view);
}

void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    updateView();

    solarSystem.render(&model, &view, simulationTimeStamp * REAL_SECOND_TO_ANIMATION_SECONDS);

    mySkyBox.Draw(*skyboxShaderWithLocs.getShader(), view, projection);
}

void cleanup() {
    myWindow.Delete();
    //cleanup code for your own data
}

int main(int argc, const char * argv[]) {

    try {
        initOpenGLWindow();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    initOpenGLState();
	initModels();
	initShaders();
    initSkyBox();
	initUniforms();
    setWindowCallbacks();

	glCheckError();
	// application loop
	while (!glfwWindowShouldClose(myWindow.getWindow())) {
        updateDelta();
        processMovement();
	    renderScene();

		glfwPollEvents();
		glfwSwapBuffers(myWindow.getWindow());

		glCheckError();
	}

	cleanup();

    return EXIT_SUCCESS;
}
