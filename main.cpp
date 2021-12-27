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

// shader uniform locations
GLint modelLoc;
GLint viewLoc;
GLint projectionLoc;
GLint normalMatrixLoc;
GLint lightDirLoc;
GLint lightColorLoc;

// camera
gps::Camera myCamera(
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(10.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));

GLfloat cameraSpeed = 10.0f;

GLboolean pressedKeys[1024];

// models
gps::Model3D mercury;
gps::Model3D venus;
gps::Model3D earth;
gps::Model3D mars;
gps::Model3D jupiter;
gps::Model3D saturn;
gps::Model3D uranus;
gps::Model3D neptune;

glm::vec3 mercuryPosition(579, 0, 0); // todo: multiply by 100
glm::vec3 venusPosition(1082, 0, 0);
glm::vec3 earthPosition(1496, 0, 0);
glm::vec3 marsPosition(2279, 0, 0);
glm::vec3 jupiterPosition(7786, 0, 0);
glm::vec3 saturnPosition(14335, 0, 0);
glm::vec3 uranusPosition(28725, 0, 0);
glm::vec3 neptunePosition(44951, 0, 0);

GLfloat angle;

// shaders
gps::Shader myBasicShader;

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
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
		//update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	}

    if (pressedKeys[GLFW_KEY_Q]) {
        angle -= 1.0f;
        // update model matrix
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
    }

    if (pressedKeys[GLFW_KEY_E]) {
        angle += 1.0f;
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
    mercury.LoadModel("models/mercury/mercury.obj");
    venus.LoadModel("models/venus/venus.obj");
    earth.LoadModel("models/earth/earth.obj");
    mars.LoadModel("models/mars/mars.obj");
    jupiter.LoadModel("models/jupiter/jupiter.obj");
    saturn.LoadModel("models/saturn/saturn.obj");
    uranus.LoadModel("models/uranus/uranus.obj");
    neptune.LoadModel("models/neptune/neptune.obj");
}

void initShaders() {
	myBasicShader.loadShader(
        "shaders/basic.vert",
        "shaders/basic.frag");
}

void initUniforms() {
	myBasicShader.useShaderProgram();

    // create model matrix
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");

	// get view matrix for current camera
	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
	// send view matrix to shader
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // compute normal matrix
    normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");

	// create projection matrix
	projection = glm::perspective(glm::radians(45.0f),
                               (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
                               0.1f, 100000000.0f);
	projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
	// send projection matrix to shader
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));	

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
	lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
	// send light dir to shader
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

	//set light color
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");
	// send light color to shader
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
}

void renderMercury(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    //send mercury model matrix data to shader
    glm::mat4 mercury_model = glm::translate(model, mercuryPosition);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mercury_model));

    //send mercury normal matrix data to shader
    normalMatrix = glm::mat3(glm::inverseTranspose(view * mercury_model));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    // draw mercury
    mercury.Draw(shader);
}

void renderVenus(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    //send venus model matrix data to shader
    glm::mat4 venus_model = glm::translate(model, venusPosition);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(venus_model));

    //send venus normal matrix data to shader
    normalMatrix = glm::mat3(glm::inverseTranspose(view * venus_model));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    // draw venus
    venus.Draw(shader);
}

void renderEarth(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    //send venus model matrix data to shader
    glm::mat4 earth_model = glm::translate(model, earthPosition);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(earth_model));

    //send venus normal matrix data to shader
    normalMatrix = glm::mat3(glm::inverseTranspose(view * earth_model));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    // draw venus
    earth.Draw(shader);
}

void renderMars(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    //send venus model matrix data to shader
    glm::mat4 mars_model = glm::translate(model, marsPosition);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(mars_model));

    //send venus normal matrix data to shader
    normalMatrix = glm::mat3(glm::inverseTranspose(view * mars_model));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    // draw venus
    mars.Draw(shader);
}

void renderJupiter(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    //send venus model matrix data to shader
    glm::mat4 jupiter_model = glm::translate(model, jupiterPosition);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(jupiter_model));

    //send venus normal matrix data to shader
    normalMatrix = glm::mat3(glm::inverseTranspose(view * jupiter_model));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    // draw venus
    jupiter.Draw(shader);
}

void renderSaturn(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    //send venus model matrix data to shader
    glm::mat4 saturn_model = glm::translate(model, saturnPosition);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(saturn_model));

    //send venus normal matrix data to shader
    normalMatrix = glm::mat3(glm::inverseTranspose(view * saturn_model));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    // draw venus
    saturn.Draw(shader);
}

void renderUranus(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    //send venus model matrix data to shader
    glm::mat4 uranus_model = glm::translate(model, uranusPosition);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(uranus_model));

    //send venus normal matrix data to shader
    normalMatrix = glm::mat3(glm::inverseTranspose(view * uranus_model));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    // draw venus
    uranus.Draw(shader);
}

void renderNeptune(gps::Shader shader) {
    // select active shader program
    shader.useShaderProgram();

    //send venus model matrix data to shader
    glm::mat4 neptune_model = glm::translate(model, neptunePosition);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(neptune_model));

    //send venus normal matrix data to shader
    normalMatrix = glm::mat3(glm::inverseTranspose(view * neptune_model));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

    // draw venus
    neptune.Draw(shader);
}

void renderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // render the planets
    renderMercury(myBasicShader);
    renderVenus(myBasicShader);
    renderEarth(myBasicShader);
    renderMars(myBasicShader);
    renderJupiter(myBasicShader);
    renderSaturn(myBasicShader);
    renderUranus(myBasicShader);
    renderNeptune(myBasicShader);
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
	initUniforms();
    setWindowCallbacks();

	glCheckError();
	// application loop
	while (!glfwWindowShouldClose(myWindow.getWindow())) {
        processMovement();
	    renderScene();

		glfwPollEvents();
		glfwSwapBuffers(myWindow.getWindow());

		glCheckError();
	}

	cleanup();

    return EXIT_SUCCESS;
}
