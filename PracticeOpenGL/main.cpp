//
//  Created by douduck08 on 2017/9/22.
//  Copyright © 2017 Leo Lu. All rights reserved.
//

#include <iostream>
#include <vector>
using namespace std;

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "resources.h"
#include "render_texture.h"
#include "material.h"
#include "mesh.h"
#include "util.h"

const char *TITLE = "Practice OpenGL";
const GLint WIDTH = 800, HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 1.0f, 3.0f));
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
    glEnable(GL_CULL_FACE);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    Mesh planeMesh = MeshGenerator::GeneratePlane();
    Mesh spheremesh = MeshGenerator::GenerateSphere();
    Mesh cubeMesh = MeshGenerator::GenerateCube();
    
    GLuint stdShader = Shader::LoadShader("resources/shaders/standard.vert", "resources/shaders/standard.frag");
    Material stdMaterial(stdShader);
    //stdMaterial.AddTexture(Resources::LoadTexture("resources/images/uv_diag.png"), GL_TEXTURE_2D);
    stdMaterial.AddTexture(Resources::LoadTexture("resources/images/white.png"), GL_TEXTURE_2D);
    
    RenderTexture renderTexture(WIDTH, HEIGHT, screenWidth, screenHeight);
    GLuint baseShader = Shader::LoadShader("resources/shaders/base.vert", "resources/shaders/base.frag");
    Material baseMaterial(baseShader);
    baseMaterial.AddTexture(renderTexture.textureName, GL_TEXTURE_2D);
    
    glm::mat4 model[6], view, projection;
    projection = camera.GetProjectionMatrix(screenWidth, screenHeight);
    
    model[0] = glm::mat4();
    model[1] = glm::mat4();
    model[1] = glm::translate(model[1], glm::vec3(-1.2f, 0.f, -1.f));
    model[1] = glm::rotate(model[1], (GLfloat)M_PI * .25f, glm::vec3(0.f, 1.f, 0.f));
    model[2] = glm::mat4();
    model[2] = glm::translate(model[2], glm::vec3(0.f, 1.f, 0.f));
    model[3] = glm::mat4();
    model[3] = glm::translate(model[3], glm::vec3(1.2f, 0.f, 0.f));
    model[4] = glm::mat4();
    model[4] = glm::translate(model[4], glm::vec3(0.f, -.5f, 0.f));
    model[4] = glm::rotate(model[4], (GLfloat)M_PI * -.5f, glm::vec3(1.f, 0.f, 0.f));
    model[4] = glm::scale(model[4], glm::vec3(5.f, 5.f, 5.f));
    model[5] = glm::mat4();
    model[5] = glm::translate(model[5], glm::vec3(0.f, 3.5f, -5.f));
    model[5] = glm::scale(model[5], glm::vec3(4.f, 3.f, 4.f));
    
    // Game Loop
    GLfloat deltaTime = 0.0f;
    GLfloat lastFrame = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glfwPollEvents();
        TriggerInput(deltaTime);
        
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Render
        stdMaterial.UseProgram(stdShader);
        glUniform3f(glGetUniformLocation(stdShader, "dirLight.direction"), 1.2f, -2.0f, -0.8f);
        glUniform3f(glGetUniformLocation(stdShader, "dirLight.ambient"), 0.2f, 0.2f, 0.2f);
        glUniform3f(glGetUniformLocation(stdShader, "dirLight.diffuse"), 0.8f, 0.8f, 0.8f);
        glUniform3f(glGetUniformLocation(stdShader, "dirLight.specular"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(stdShader, "viewPos"), camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
        view = camera.GetViewMatrix();
        
        stdMaterial.SetProperty(projection, view, model[0]);
        cubeMesh.Render(&stdMaterial);
        stdMaterial.SetProperty(projection, view, model[1]);
        cubeMesh.Render(&stdMaterial);
        stdMaterial.SetProperty(projection, view, model[2]);
        spheremesh.Render(&stdMaterial);
        stdMaterial.SetProperty(projection, view, model[3]);
        spheremesh.Render(&stdMaterial);
        stdMaterial.SetProperty(projection, view, model[4]);
        planeMesh.Render(&stdMaterial);
        
        renderTexture.Bind();
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        stdMaterial.SetProperty(projection, view, model[0]);
        cubeMesh.Render(&stdMaterial);
        stdMaterial.SetProperty(projection, view, model[1]);
        cubeMesh.Render(&stdMaterial);
        stdMaterial.SetProperty(projection, view, model[2]);
        spheremesh.Render(&stdMaterial);
        stdMaterial.SetProperty(projection, view, model[3]);
        spheremesh.Render(&stdMaterial);
        stdMaterial.SetProperty(projection, view, model[4]);
        planeMesh.Render(&stdMaterial);
        renderTexture.Unbind();
        
        baseMaterial.UseProgram(baseShader);
        baseMaterial.SetProperty(projection, view, model[5]);
        planeMesh.Render(&baseMaterial);
        
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
