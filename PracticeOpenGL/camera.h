#pragma once

#define GLEW_STATIC
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "util.h"

class Camera {
    glm::vec3 position;
    glm::vec3 forward;
    glm::vec3 up;
    glm::vec3 right;
    
    glm::vec3 worldUp;
    GLfloat zoom;

public:
    Camera(
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f)
    ) : zoom(45.0f), worldUp(glm::vec3(0.0f, 1.0f, 0.0f)) {
        this->position = position;
        this->forward = glm::normalize(forward);
        this->up = glm::normalize(up);
        
        this->right = glm::normalize(glm::cross(this->forward, this->up));
        this->up = glm::normalize(glm::cross(this->right, this->forward));
    }
    
    glm::mat4 GetProjectionMatrix (GLfloat screenWidth, GLfloat screenHeight) {
        return glm::perspective(zoom, screenWidth / screenHeight, 0.1f, 1000.0f);
    }
    
    glm::mat4 GetViewMatrix () {
        return glm::lookAt(this->position, this->position + this->forward, this->up);
    }
    
    glm::vec3 GetPosition () {
        return this->position;
    }
    
    void Translate (glm::vec3 movement, bool inWorldSpace = false) {
        if (inWorldSpace) {
            position += movement;
            return;
        }
        position += movement.x * right;
        position += movement.y * up;
        position += movement.z * (-forward);
    }
    
    void Rotation (GLfloat x, GLfloat y) {
        // TODO: has some problem
        glm::vec3 proxy = position + forward;
        proxy += x * right + y * up;
        
        this->forward = glm::normalize(proxy - position);
        this->right = glm::normalize(glm::cross(this->forward, this->worldUp));
        this->up = glm::normalize(glm::cross(this->right, this->forward));
    }
};
