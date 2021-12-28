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

#include <iostream>

// window
gps::Window myWindow;

// matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;

// light parameters
glm::vec3 lightDir;
glm::vec3 lightColor;

// camera
gps::Camera myCamera(
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(2000.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));

GLfloat cameraSpeed = 1000.0f;

GLboolean pressedKeys[1024];

GLfloat angle;

// shaders
gps::ShaderWithUniformLocs myShaderWithLocs;
gps::ShaderWithUniformLocs skyboxShaderWithLocs;

// solar system
view_layer::SolarSystem solarSystem;

// timing
float deltaTimeSeconds = 0;
double lastTimeStamp = glfwGetTime();
double currentTimeStamp;

// skybox
std::vector<const GLchar*> skybox_faces;
gps::SkyBox mySkyBox;


const double REAL_SECOND_TO_ANIMATION_SECONDS = 3600 * 24 * 36.5; // 1s in real life corresponds to 3600s=1h in the animation
// (as a consequence, for example, it will take 1 seconds for the Earth to perform a full rotation, and 365 seconds to perform an orbital rotation)

void updateDelta() {
    lastTimeStamp = currentTimeStamp;
    currentTimeStamp = glfwGetTime();
    deltaTimeSeconds = currentTimeStamp - lastTimeStamp;
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

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);
	//TODO
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
        myCamera.move(gps::MOVE_FORWARD, cameraSpeed * deltaTimeSeconds);
        //update view matrix
        view = myCamera.getViewMatrix();
        myShaderWithLocs.sendViewUniform(view);
    }

    if (pressedKeys[GLFW_KEY_S]) {
        myCamera.move(gps::MOVE_BACKWARD, cameraSpeed * deltaTimeSeconds);
        //update view matrix
        view = myCamera.getViewMatrix();
        myShaderWithLocs.sendViewUniform(view);
    }

    if (pressedKeys[GLFW_KEY_A]) {
        myCamera.move(gps::MOVE_LEFT, cameraSpeed * deltaTimeSeconds);
        //update view matrix
        view = myCamera.getViewMatrix();
        myShaderWithLocs.sendViewUniform(view);
    }

    if (pressedKeys[GLFW_KEY_D]) {
        myCamera.move(gps::MOVE_RIGHT, cameraSpeed * deltaTimeSeconds);
        //update view matrix
        view = myCamera.getViewMatrix();
        myShaderWithLocs.sendViewUniform(view);
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
}

void initOpenGLWindow() {
    myWindow.Create(1024, 768, "OpenGL Project Core");
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
    skybox_faces.push_back("models/space_skybox/top.png");
    skybox_faces.push_back("models/space_skybox/bot.png");
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
    projection = glm::perspective(glm::radians(45.0f),
        (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
        0.1f, 1000000.0f);
    myShaderWithLocs.sendProjectionUniform(projection);
    skyboxShaderWithLocs.sendProjectionUniform(projection);

    //set the light direction (direction towards the light)
    lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
    myShaderWithLocs.sendLightDirUniform(lightDir);

    //set light color
    lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
    myShaderWithLocs.sendLightColorUniform(lightColor);
}

void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    solarSystem.render(&model, &view, currentTimeStamp * REAL_SECOND_TO_ANIMATION_SECONDS);

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
