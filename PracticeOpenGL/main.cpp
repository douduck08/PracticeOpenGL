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

#include "resources.h"
#include "texture.h"
#include "util.h"

#include "dgb/core.h"
#include "dgb/assets.h"
#include "dgb/game_object.h"
#include "dgb/mesh_generator.h"
#include "dgb/util.h"

const char *TITLE = "Practice OpenGL";
const GLint WIDTH = 800, HEIGHT = 600;

const glm::vec3 FORWARD (0.0f, 0.0f, -1.0f);
const glm::vec3 UP (0.0f, 1.0f, 0.0f);
const glm::vec3 RIGHT (1.0f, 0.0f, 0.0f);
const GLfloat CAMERA_MOVE_SPEED = 6.0f;
const GLfloat CAMERA_ROTATION_SENSITIVTY = 0.5f;

bool keys[1024];
bool firstMouse = true;
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
float eularX, eularY;
void TriggerInput(GLfloat deltaTime);
void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow *window, double xPos, double yPos);

dgb::Core gameCore;

int main(int argc, const char * argv[]) {
    
    if (!gameCore.CreateWindow(800, 600, "Practice OpenGL")) {
        return EXIT_FAILURE;
    }
    
    glfwSetKeyCallback(gameCore.mainWindow, KeyCallback);
    glfwSetCursorPosCallback(gameCore.mainWindow, MouseCallback);
    glfwSetInputMode(gameCore.mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    //glEnable( GL_BLEND );
    //glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    dgb::Mesh planeMesh = dgb::MeshGenerator::GeneratePlane();
    dgb::Mesh spheremesh = dgb::MeshGenerator::GenerateSphere();
    dgb::Mesh cubeMesh = dgb::MeshGenerator::GenerateCube();
    
    dgb::Shader stdShader("resources/shaders/standard.vert", "resources/shaders/standard.frag");
    dgb::Material stdMaterial(stdShader);
    dgb::Texture whiteTex(GL_TEXTURE_2D, "resources/images/white.png");
    //stdMaterial.AddTexture(Resources::LoadTexture("resources/images/uv_diag.png"), GL_TEXTURE_2D);
    stdMaterial.textures.push_back(whiteTex);
    
    dgb::Shader depthShader("resources/shaders/depth.vert", "resources/shaders/depth.frag");
    dgb::Material depthMaterial(depthShader);
    ShadowMap shadowMap(glm::vec3(1.2f, -2.0f, -0.8f));
    stdMaterial.textures.push_back(dgb::Texture(GL_TEXTURE_2D, shadowMap.depthTexture));
    
    dgb::Shader baseShader("resources/shaders/base.vert", "resources/shaders/base.frag");
    dgb::Material baseMaterial(baseShader);
    RenderTexture renderTexture(WIDTH, HEIGHT, gameCore.frameBufferWidth, gameCore.frameBufferHeight);
    baseMaterial.textures.push_back(dgb::Texture(GL_TEXTURE_2D, renderTexture.textureName));
    //baseMaterial.AddTexture(shadowMap.depthTexture, GL_TEXTURE_2D);
    
    gameCore.mainCamera.Translate(glm::vec3(0.0f, 1.0f, 3.0f));
    glm::mat4 model[6], view, projection;
    projection = gameCore.mainCamera.GetProjectionMatrix(gameCore.frameBufferWidth, gameCore.frameBufferHeight);
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
    
    dgb::Transform trans(glm::vec3(0.f, 3.5f, -5.f));
    trans.SetScale(glm::vec3(4.f, 3.f, 4.f));
    trans.SetEular(glm::vec3(-30.f, 0.f, 0.f));
    
    dgb::Mesh combinedMesh;
    combinedMesh.Combine(&cubeMesh, model[0]);
    combinedMesh.Combine(&cubeMesh, model[1]);
    combinedMesh.Combine(&spheremesh, model[2]);
    combinedMesh.Combine(&spheremesh, model[3]);
    combinedMesh.Combine(&planeMesh, model[4]);
    combinedMesh.SetupBuffer();
    
    stdShader.Use();
    glUniform3f(glGetUniformLocation(stdShader.program, "dirLight.direction"), 1.2f, -2.0f, -0.8f);
    glUniform3f(glGetUniformLocation(stdShader.program, "dirLight.ambient"), 0.2f, 0.2f, 0.2f);
    glUniform3f(glGetUniformLocation(stdShader.program, "dirLight.diffuse"), 0.8f, 0.8f, 0.8f);
    glUniform3f(glGetUniformLocation(stdShader.program, "dirLight.specular"), 1.0f, 1.0f, 1.0f);
    glUniformMatrix4fv(glGetUniformLocation(stdShader.program, "depthBiasMVP"), 1, GL_FALSE, glm::value_ptr(shadowMap.depthBiasMVP));
    
    
    // Game Loop
    GLfloat deltaTime = 0.0f;
    GLfloat lastFrame = 0.0f;
    while (!glfwWindowShouldClose(gameCore.mainWindow)) {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glfwPollEvents();
        TriggerInput(deltaTime);
        
        // Render
        shadowMap.Bind(depthShader.program);
        {
            glClear(GL_DEPTH_BUFFER_BIT);
            //combinedMesh.Render(&depthMaterial);
        }
        shadowMap.Unbind();
        
        gameCore.mainCamera.BeginRender();
        stdShader.Use();
        glUniform3f(glGetUniformLocation(stdShader.program, "viewPos"), gameCore.mainCamera.GetPosition().x, gameCore.mainCamera.GetPosition().y, gameCore.mainCamera.GetPosition().z);
        view = gameCore.mainCamera.GetViewMatrix();
        combinedMesh.Draw(&stdMaterial, projection, view, model[0]);
        
        renderTexture.Bind();
        {
            glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            combinedMesh.Draw(&stdMaterial, projection, view, model[0]);
        }
        renderTexture.Unbind();
        
        //baseMaterial.SetProperty(projection, view, model[5]);
        planeMesh.Draw(&baseMaterial, projection, view, trans.GetMatrix());
        
        glfwSwapBuffers(gameCore.mainWindow);
    }
    
    glfwTerminate();
    return 0;
}

void TriggerInput(GLfloat deltaTime) {
    if (keys[GLFW_KEY_W] || keys[GLFW_KEY_UP]) {
        gameCore.mainCamera.Translate(FORWARD * CAMERA_MOVE_SPEED * deltaTime);
    }
    
    if (keys[GLFW_KEY_S] || keys[GLFW_KEY_DOWN]) {
        gameCore.mainCamera.Translate(-FORWARD * CAMERA_MOVE_SPEED * deltaTime);
    }
    
    if (keys[GLFW_KEY_A] || keys[GLFW_KEY_LEFT]) {
        gameCore.mainCamera.Translate(-RIGHT * CAMERA_MOVE_SPEED * deltaTime);
    }
    
    if (keys[GLFW_KEY_D] || keys[GLFW_KEY_RIGHT]) {
        gameCore.mainCamera.Translate(RIGHT * CAMERA_MOVE_SPEED * deltaTime);
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
    
    eularX += yOffset * CAMERA_ROTATION_SENSITIVTY;
    eularY += -xOffset * CAMERA_ROTATION_SENSITIVTY;
    if (eularX > 90) {
        eularX = 90;
    } else if (eularX < -90) {
        eularX = -90;
    }
    
    gameCore.mainCamera.transform.SetEular(glm::vec3(eularX, eularY, 0.f));
}
