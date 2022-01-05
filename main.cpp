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
#include "GeometryShader.h"

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

GLfloat cameraMoveSpeed = 300.0f;
GLfloat cameraRotationSpeed = 10.0f;

// event handling
GLboolean pressedKeys[1024];

// angle of rotation, for the model
GLfloat angle;

// shaders
gps::ShaderWithUniformLocs myShaderWithLocs;
gps::ShaderWithUniformLocs skyboxShaderWithLocs;
gps::GeometryShader sunDepthMapShader;
gps::ShaderWithUniformLocs sunShaderWithLocs;
gps::ShaderWithUniformLocs earthShaderWithLocs;

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
                                /*.ambientStrength =*/ 0.4, 
                                /*.diffuseStrength =*/ 0.0,
                                /*.specularStrength =*/ 0.0 };

// yellow-ish sun light (positional)
view_layer::PointLight sunLight = {
    /* .position= */ glm::vec3(0, 0, 0),
    /* .constant= */ 1.0,
    /* .linear= */   0.000014,
    /* .quadratic=*/ 0.000000007,
    /* .ambient = */ glm::vec3(0.0, 0.0, 0.0),
    /* .diffuse = */ glm::vec3(1.0, 0.952, 0.741),
    /* .specular= */ glm::vec3(1.0, 0.952, 0.741),
};

// speed
const double REAL_SECOND_TO_ANIMATION_SECONDS = 3600 * 24 * 3.65; // 1s in real life corresponds to 3600s=1h in the animation
// (as a consequence, for example, it will take 1 seconds for the Earth to perform a full rotation, and 365 seconds to perform an orbital rotation)

// shadows
unsigned int depthCubemap; // for the sunlight
const unsigned int SHADOW_WIDTH = 10240, SHADOW_HEIGHT = 10240;
unsigned int depthMapFBO;
std::vector<glm::mat4> shadowTransforms;
const float SUN_DEPTH_MAP_FAR_PLANE = 20000;

// additional textures
GLuint earth_night_texture;

// mouse control
float lastMouseX = 400, lastMouseY = 300;
const float mouseSensitivity = 0.1f;
bool mouse_control_enabled = false;
bool mouse_button_pressed = false;

// wireframe vs solid mode
bool wireframe_mode_on = false;
bool wireframe_button_pressed = false;

void setupShadowTransforms() {
    float aspect = (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT;
    float near = 1.0f;
    float far = SUN_DEPTH_MAP_FAR_PLANE;
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near, far);

    shadowTransforms.clear();
    shadowTransforms.push_back(shadowProj *
        glm::lookAt(sunLight.position, sunLight.position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
    shadowTransforms.push_back(shadowProj *
        glm::lookAt(sunLight.position, sunLight.position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
    shadowTransforms.push_back(shadowProj *
        glm::lookAt(sunLight.position, sunLight.position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
    shadowTransforms.push_back(shadowProj *
        glm::lookAt(sunLight.position, sunLight.position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
    shadowTransforms.push_back(shadowProj *
        glm::lookAt(sunLight.position, sunLight.position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
    shadowTransforms.push_back(shadowProj *
        glm::lookAt(sunLight.position, sunLight.position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));
}

void setupShadows() {
    glGenFramebuffers(1, &depthMapFBO);

    // create depth cubemap texture
    glGenTextures(1, &depthCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
    for (unsigned int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
            SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    setupShadowTransforms();
}

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
        0.1f, SUN_DEPTH_MAP_FAR_PLANE);
    myShaderWithLocs.sendProjectionUniform(projection);
    skyboxShaderWithLocs.sendProjectionUniform(projection);
    sunShaderWithLocs.sendProjectionUniform(projection);
    earthShaderWithLocs.sendProjectionUniform(projection);
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
    float xoffset = xpos - lastMouseX;
    float yoffset = ypos - lastMouseY;
    lastMouseX = xpos;
    lastMouseY = ypos;

    if (mouse_control_enabled) {
        xoffset *= mouseSensitivity;
        yoffset *= mouseSensitivity;

        myCamera.rotate(cameraRotationSpeed * yoffset * deltaTimeSeconds, cameraRotationSpeed * xoffset * deltaTimeSeconds);
    }
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
    } else {
        prev_enter_pressed = false;
    }
    if (pressedKeys[GLFW_KEY_M]) {
        if (!mouse_button_pressed) {
            mouse_control_enabled = !mouse_control_enabled;
            mouse_button_pressed = true;
        }
    } else {
        mouse_button_pressed = false;
    }
    if (pressedKeys[GLFW_KEY_Z]) {
        if (!wireframe_button_pressed) {
            wireframe_mode_on = !wireframe_mode_on;
            if (wireframe_mode_on) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }
            else {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
            wireframe_button_pressed = true;
        }
    }
    else {
        wireframe_button_pressed = false;
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
    // TODO: glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

GLuint ReadTextureFromFile(const char* file_name) {
    int x, y, n;
    int force_channels = 4;
    unsigned char* image_data = stbi_load(file_name, &x, &y, &n, force_channels);
    if (!image_data) {
        fprintf(stderr, "ERROR: could not load %s\n", file_name);
        return false;
    }
    // NPOT check
    if ((x & (x - 1)) != 0 || (y & (y - 1)) != 0) {
        fprintf(
            stderr, "WARNING: texture %s is not power-of-2 dimensions\n", file_name
        );
    }

    int width_in_bytes = x * 4;
    unsigned char* top = NULL;
    unsigned char* bottom = NULL;
    unsigned char temp = 0;
    int half_height = y / 2;

    for (int row = 0; row < half_height; row++) {
        top = image_data + row * width_in_bytes;
        bottom = image_data + (y - row - 1) * width_in_bytes;
        for (int col = 0; col < width_in_bytes; col++) {
            temp = *top;
            *top = *bottom;
            *bottom = temp;
            top++;
            bottom++;
        }
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_SRGB, //GL_SRGB,//GL_RGBA,
        x,
        y,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        image_data
    );
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    return textureID;
}

void initModels() {
   solarSystem.init(&myShaderWithLocs, &sunShaderWithLocs, &earthShaderWithLocs);
   earth_night_texture = ReadTextureFromFile("models/earth/earth_night_texture.jpg");
}

void initShaders() {
    myShaderWithLocs.init("shaders/basic.vert", "shaders/basic.frag");
    skyboxShaderWithLocs.init("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
    // the SunDepthMapShader has a gemoetry shader as well
    sunDepthMapShader.LoadShader("shaders/depthMapForSun.vert", "shaders/depthMapForSun.frag", "shaders/depthMapForSun.geo");
    sunShaderWithLocs.init("shaders/sunShader.vert", "shaders/sunShader.frag");
    earthShaderWithLocs.init("shaders/earthShader.vert", "shaders/earthShader.frag");
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
    sunShaderWithLocs.sendViewUniform(view);
    earthShaderWithLocs.sendViewUniform(view);

    // compute normal matrix
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

    // create projection matrix
    updateProjection();

    //set the directional light
    myShaderWithLocs.sendDirectionalLightUniform(dirLight);
    earthShaderWithLocs.sendDirectionalLightUniform(dirLight);

    // set the sun light
    myShaderWithLocs.sendSunLightUniform(sunLight);
    earthShaderWithLocs.sendSunLightUniform(sunLight);

    // for shadows
    sunDepthMapShader.setFloat("far_plane", SUN_DEPTH_MAP_FAR_PLANE);
    for (unsigned int i = 0; i < 6; ++i)
        sunDepthMapShader.setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
    sunDepthMapShader.setVec3("lightPos", sunLight.position);
    glUseProgram(myShaderWithLocs.getShader()->shaderProgram);
    glUniform1f(glGetUniformLocation(myShaderWithLocs.getShader()->shaderProgram, "far_plane"), SUN_DEPTH_MAP_FAR_PLANE);
    glUseProgram(earthShaderWithLocs.getShader()->shaderProgram);
    glUniform1f(glGetUniformLocation(earthShaderWithLocs.getShader()->shaderProgram, "far_plane"), SUN_DEPTH_MAP_FAR_PLANE);
}

void updateView() {
    //update view matrix
    view = myCamera.getViewMatrix();

    myShaderWithLocs.sendViewUniform(view);
    sunShaderWithLocs.sendViewUniform(view);
    earthShaderWithLocs.sendViewUniform(view);
}

void renderScene() {
    setupShadowTransforms();

    updateView();

    // 1. first render to depth cubemap
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    solarSystem.renderWithDepthMapShader(&model, &view, simulationTimeStamp * REAL_SECOND_TO_ANIMATION_SECONDS, &sunDepthMapShader);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 2. then render scene as normal with shadow mapping (using depth cubemap)
    glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    myShaderWithLocs.getShader()->useShaderProgram();
    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
    glUniform1i(glGetUniformLocation(myShaderWithLocs.getShader()->shaderProgram, "depthMap"), 8);
    earthShaderWithLocs.getShader()->useShaderProgram();
    glUniform1i(glGetUniformLocation(earthShaderWithLocs.getShader()->shaderProgram, "depthMap"), 8);

    glActiveTexture(GL_TEXTURE5);
    earthShaderWithLocs.getShader()->useShaderProgram();
    glUniform1i(glGetUniformLocation(earthShaderWithLocs.getShader()->shaderProgram, "nightDiffuseTexture"), 5);
    glBindTexture(GL_TEXTURE_2D, earth_night_texture);

    solarSystem.render(&model, &view, simulationTimeStamp * REAL_SECOND_TO_ANIMATION_SECONDS);

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, 0);

    mySkyBox.Draw(*skyboxShaderWithLocs.getShader(), view, projection);
    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void cleanup() {
    myWindow.Delete();
    //cleanup code for your own data
}

void detectLandings() {
    glm::vec3 cameraPos = myCamera.getCameraPosition();

    bool landed = solarSystem.landed_on_planet(currentTimeStamp, cameraPos);

    if (landed) {
        printf("Landed on a planet!");
    }
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
    setupShadows();
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

        detectLandings();
	}

	cleanup();

    return EXIT_SUCCESS;
}
