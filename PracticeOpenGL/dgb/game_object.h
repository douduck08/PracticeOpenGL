#pragma once

namespace dgb {
    class Camera {
        int viewport[4];
        float clearColor[4];
        
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
        
        void SetViewport (int viewport[]) {
            for (int i = 0; i < 4; i++) {
                this->viewport[i] = viewport[i];
            }
        }
        
        void SetViewport (int x, int y, int width, int height) {
            viewport[0] = x;
            viewport[1] = y;
            viewport[2] = width;
            viewport[3] = height;
        }
        
        void SetClearColor(float clearColor[]) {
            for (int i = 0; i < 4; i++) {
                this->clearColor[i] = clearColor[i];
            }
        }
        
        void SetClearColor(float r, float g, float b, float a = 1.f) {
            clearColor[0] = r;
            clearColor[1] = g;
            clearColor[2] = b;
            clearColor[3] = a;
        }
        
        void BeginRender () {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
            glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
    
    class Model {
        vector<Mesh> meshes;
        vector<Material> materials;
        
    public:
        void AddMesh () {
            
        }
        
        void Draw () {
            
        }
    };
    
    class Skybox {
        
    };
}
