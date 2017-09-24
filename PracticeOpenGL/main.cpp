//
//  Created by douduck08 on 2017/9/22.
//  Copyright © 2017 Leo Lu. All rights reserved.
//

#include <iostream>
using namespace std;

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "resources.h"
#include "material.h"
#include "mesh.h"
#include "util.h"

const char *TITLE = "Practice OpenGL";
const GLint WIDTH = 800, HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
const glm::vec3 FORWARD (0.0f, 0.0f, -1.0f);
const glm::vec3 UP (0.0f, 1.0f, 0.0f);
const glm::vec3 RIGHT (1.0f, 0.0f, 0.0f);
const GLfloat CAMERA_MOVE_SPEED = 6.0f;
const GLfloat CAMERA_ROTATION_SENSITIVTY = 0.01f;

bool keys[1024];
bool firstMouse = true;
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
void TriggerInput(GLfloat deltaTime);
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow *window, double xPos, double yPos);

int main(int argc, const char * argv[]) {
    Util::InitGLFW();
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, nullptr, nullptr);
    if (nullptr == window ) {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate( );
        return EXIT_FAILURE;
    }
    
    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCursorPosCallback(window, MouseCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (GLEW_OK != glewInit()) {
        cout << "Failed to initialize GLEW" << endl;
        return EXIT_FAILURE;
    }
    
    glViewport(0, 0, screenWidth, screenHeight);
    //glEnable( GL_BLEND );
    //glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glEnable(GL_DEPTH_TEST);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    Mesh planeMesh = MeshGenerator::GeneratePlane();
    Mesh spheremesh = MeshGenerator::GenerateSphere();
    Mesh cubeMesh = MeshGenerator::GenerateCube();
    GLuint textureShader = Shader::LoadShader("resources/shaders/texture.vert", "resources/shaders/texture.frag");
    Material uvMaterial(textureShader);
    uvMaterial.AddTexture(Resources::LoadTexture("resources/images/uv_diag.png"), GL_TEXTURE_2D);
    Material grayMaterial(textureShader);
    grayMaterial.AddTexture(Resources::LoadTexture("resources/images/border.png"), GL_TEXTURE_2D);

    // Game Loop
    GLfloat deltaTime = 0.0f;
    GLfloat lastFrame = 0.0f;
    glm::mat4 model, view, projection;
    projection = camera.GetProjectionMatrix(screenWidth, screenHeight);
    
    while (!glfwWindowShouldClose(window)) {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glfwPollEvents();
        TriggerInput(deltaTime);
        
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Render
        uvMaterial.UseProgram(textureShader);
        view = camera.GetViewMatrix();
        model = glm::mat4();
        uvMaterial.SetProperty(projection, view, model);
        planeMesh.Render(&uvMaterial);
        
        model = glm::mat4();
        model = glm::translate(model, glm::vec3(-1.2f, 0.f, 0.f));
        uvMaterial.SetProperty(projection, view, model);
        spheremesh.Render(&uvMaterial);
        
        model = glm::mat4();
        model = glm::translate(model, glm::vec3(1.2f, 0.f, 0.f));
        uvMaterial.SetProperty(projection, view, model);
        cubeMesh.Render(&uvMaterial);
        
        glfwSwapBuffers(window);
    }
    
    glfwTerminate( );
    return 0;
}

void TriggerInput(GLfloat deltaTime) {
    if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP]) {
        camera.Translate(FORWARD * CAMERA_MOVE_SPEED * deltaTime);
    }
    
    if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN]) {
        camera.Translate(-FORWARD * CAMERA_MOVE_SPEED * deltaTime);
    }
    
    if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT]) {
        camera.Translate(-RIGHT * CAMERA_MOVE_SPEED * deltaTime);
    }
    
    if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT]) {
        camera.Translate(RIGHT * CAMERA_MOVE_SPEED * deltaTime);
    }
}

void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            keys[key] = true;
        } else if (action == GLFW_RELEASE) {
            keys[key] = false;
        }
    }
}

void MouseCallback(GLFWwindow *window, double xPos, double yPos) {
    if(firstMouse) {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }
    
    GLfloat xOffset = xPos - lastX;
    GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left
    lastX = xPos;
    lastY = yPos;
    
    camera.Rotation(xOffset * CAMERA_ROTATION_SENSITIVTY, yOffset * CAMERA_ROTATION_SENSITIVTY);
}
