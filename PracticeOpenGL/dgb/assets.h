#pragma once

namespace dgb {
    class Shader {
        
    };
    
    class Texture {
        
    };
    
    class Material {
        
    };
    
    class Mesh {
        
    };
    
    class Transform {
        glm::vec3 position;
        glm::quat rotation;
        
        glm::vec3 forward;
        glm::vec3 up;
        glm::vec3 right;
        
        glm::vec3 worldUp;
        
    public:
        glm::mat4 GetMatrix () {
            return;
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
}
