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
    
    };
