#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace dgb {
    class Shader {
        GLuint projLoc;
        GLuint viewLoc;
        GLuint modelLoc;
    public:
        GLuint program;
        
        Shader (const GLchar *vertexPath, const GLchar *fragmentPath) {
            this->program = glCreateProgram();
            
            string vertCode = LoadShaderFile(vertexPath);
            GLuint vertex = CompileShaderCode(vertCode.c_str(), GL_VERTEX_SHADER);
            glAttachShader(this->program, vertex);
            
            string fragCode = LoadShaderFile(fragmentPath);
            GLuint fragment = CompileShaderCode(fragCode.c_str(), GL_FRAGMENT_SHADER);
            glAttachShader(this->program, fragment);
            
            LinkProgram(this->program);
            glDeleteShader(vertex);
            glDeleteShader(fragment);
            
            this->projLoc = glGetUniformLocation(this->program, "projection");
            this->viewLoc = glGetUniformLocation(this->program, "view");
            this->modelLoc = glGetUniformLocation(this->program, "model");
        }
        
        void Use() {
            glUseProgram(this->program);
        }
        
        void SetMatrix (glm::mat4 projection, glm::mat4 view, glm::mat4 model) {
            glUniformMatrix4fv(this->projLoc, 1, GL_FALSE, glm::value_ptr(projection));
            glUniformMatrix4fv(this->viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            glUniformMatrix4fv(this->modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        }
        
    private:
        string LoadShaderFile (const GLchar *filePath) {
            ifstream shaderFile(filePath);
            stringstream shaderStream("");
            
            if(!shaderFile.is_open()) {
                Debug::LogError("<Shader> reading failed: %s", filePath);
                return "";
            }
            
            string content = "", line = "";
            while(!shaderFile.eof()) {
                getline(shaderFile, line);
                content.append(line + "\n");
            }
            shaderFile.close();
            return content;
        }
        
        GLint CompileShaderCode(const GLchar *code, const GLenum shaderType) {
            GLint success;
            GLchar infoLog[512];
            GLuint result = glCreateShader(shaderType);
            glShaderSource(result, 1, &code, NULL);
            glCompileShader(result);
            
            glGetShaderiv(result, GL_COMPILE_STATUS, &success );
            if (!success) {
                glGetShaderInfoLog(result, 512, NULL, infoLog);
                Debug::LogError("<Shader> compilation failed: %s", infoLog);
            }
            return result;
        }
        
        void LinkProgram(GLuint program) {
            GLint success;
            GLchar infoLog[512];
            glLinkProgram(program);
            glGetProgramiv(program, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(program, 512, NULL, infoLog);
                Debug::LogError("<Shader> linking failed: %s", infoLog);
            }
        }
    };
    
    class Texture {
    public:
        GLuint name;
        GLenum type;
        
        Texture(GLenum type, GLuint textureId) {
            this->name = textureId;
            this->type = type;
        }
        
        Texture(GLenum type, GLchar *path) {
            this->type = type;
            glGenTextures(1, &name);
            
            if (type == GL_TEXTURE_2D) {
                name = SOIL_load_OGL_texture(path, SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
            
                glBindTexture(GL_TEXTURE_2D, name);
                glGenerateMipmap(GL_TEXTURE_2D);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glBindTexture(GL_TEXTURE_2D, 0);
            } else {
                Debug::LogError("No implement texture type");
            }
        }
    };
    
    class Material {
    public:
        Shader shader;
        vector<Texture> textures;
        
        Material (Shader shader) : shader(shader) {
            
        }
        
        Material (Shader shader, vector<Texture> textures) : shader(shader) {
            this->textures = textures;
        }
        
        void BindTextures () {
            for (int i = 0; i < this->textures.size(); i++) {
                glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(this->textures[i].type, this->textures[i].name);
                string uniformName = "texture" + to_string(i);
                glUniform1i(glGetUniformLocation(this->shader.program, uniformName.c_str()), i);
            }
        }
        
        void UnbindTextures () {
            for (int i = 0; i < this->textures.size(); i++) {
                glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(this->textures[i].type, 0);
            }
        }
    };
    
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoords;
        glm::vec3 color;
    };
    
    class Mesh {
        GLuint VAO, VBO, EBO;
        
    public:
        vector<Vertex> vertices;
        vector<GLuint> triangles;
        
        Mesh () {
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);
        }
        
        Mesh (vector<Vertex> vertices, vector<GLuint> triangles) {
            this->vertices = vertices;
            this->triangles = triangles;
            
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);
            this->SetupBuffer();
        }
        
        ~Mesh () {
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
        }
        
        void Draw (Material *material, glm::mat4 projection, glm::mat4 view, glm::mat4 model) {
            material->shader.Use();
            material->shader.SetMatrix(projection, view, model);
            material->BindTextures();
            
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, this->triangles.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
            
            material->UnbindTextures();
        }
        
        void SetupBuffer () {
            glBindVertexArray(this->VAO);
            glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
            glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->triangles.size() * sizeof(GLuint), &this->triangles[0], GL_STATIC_DRAW);
            
            // Vertex Positions
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)0);
            // Vertex Normals
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)offsetof(Vertex, normal));
            // Vertex Texture Coords
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)offsetof(Vertex, texCoords));
            // Vertex Colors
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid *)offsetof(Vertex, color));
            
            glBindVertexArray(0);
        }
        
        void Combine (Mesh *source, glm::mat4 transform) {
            GLuint indexBase = this->vertices.size();
            for (int i = 0; i < source->vertices.size(); i++) {
                Vertex vert = source->vertices[i];
                vert.position = transform * glm::vec4(vert.position, 1.f);
                vert.normal = transform * glm::vec4(vert.normal, 0.f);
                this->vertices.push_back(vert);
            }
            for (int i = 0; i < source->triangles.size(); i++) {
                this->triangles.push_back(indexBase + source->triangles[i]);
            }
        }
    };
    
    class Transform {
        glm::vec3 position;
        glm::quat rotation;
        glm::vec3 scale;
        
        glm::mat4 matrix;
        
    public:
        static glm::vec3 worldUp;
        
        Transform(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f)) {
            this->position = position;
            this->scale = glm::vec3(1.f, 1.f, 1.f);
            UpdateMatrix();
        }
        
        glm::mat4 GetMatrix () {
            return this->matrix;
        }
        
        glm::vec3 GetPosition () {
            return this->position;
        }
        
        glm::quat GetRotation () {
            return this->rotation;
        }
        
        glm::vec3 GetEular () {
            return glm::eulerAngles(this->rotation) * 180.f / 3.14159f;
        }
        
        glm::vec3 GetScale () {
            return this->scale;
        }
        
        void SetPosition (glm::vec3 position) {
            this->position = position;
            UpdateMatrix();
        }
        
        void SetEular (glm::vec3 eular) {
            eular *= 3.14159f / 180.f;
            glm::quat xQuat = glm::angleAxis(eular.x, glm::vec3(1.f, 0, 0));
            glm::quat yQuat = glm::angleAxis(eular.y, glm::vec3(0, 1.f, 0));
            glm::quat zQuat = glm::angleAxis(eular.z, glm::vec3(0, 0, 1.f));
            this->rotation = xQuat * yQuat * zQuat;
            UpdateMatrix();
        }
        
        void SetScale (glm::vec3 scale) {
            this->scale = scale;
            UpdateMatrix();
        }
        
        void Translate (glm::vec3 movement, bool inWorldSpace = false) {
            if (inWorldSpace) {
                position += movement;
                return;
            }
            position += this->rotation * movement;
            UpdateMatrix();
        }
        
    private:
        void UpdateMatrix () {
            glm::mat4 rot = glm::mat4_cast(this->rotation);
            glm::mat4 tra = glm::translate(glm::mat4(1.0f), this->position);
            glm::mat4 sca = glm::scale(glm::mat4(1.0f), this->scale);
            this->matrix = tra * rot * sca;
        }
    };
    
    glm::vec3 Transform::worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
}
