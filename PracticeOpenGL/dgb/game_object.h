#pragma once

namespace dgb {
    class Camera {
        int viewport[4];
        float clearColor[4];
        GLfloat zoom;
        
    public:
        Transform transform;
        
        Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f)) : zoom(45.0f) {
            this->transform.SetPosition(position);
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
            glm::vec3 forward = this->transform.GetRotation() * glm::vec3(0.0f, 0.0f, -1.0f);
            glm::vec3 up = this->transform.GetRotation() * glm::vec3(0.0f, 1.0f, 0.0f);
            return glm::lookAt(this->transform.GetPosition(), this->transform.GetPosition() + forward, up);
        }

        
        glm::vec3 GetPosition () {
            return this->transform.GetPosition();
        }
        
        void Translate (glm::vec3 movement, bool inWorldSpace = false) {
            this->transform.Translate(movement, inWorldSpace);
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
