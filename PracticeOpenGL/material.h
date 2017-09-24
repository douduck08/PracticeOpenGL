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

#include "resources.h"
#include "util.h"

class Shader {
public:
    static GLuint LoadShader(const GLchar *vertexPath, const GLchar *fragmentPath) {
        GLuint program = glCreateProgram();
        
        string vertCode = LoadShaderFile(vertexPath);
        GLuint vertex = CompileShaderCode(vertCode.c_str(), GL_VERTEX_SHADER);
        glAttachShader(program, vertex);
        
        string fragCode = LoadShaderFile(fragmentPath);
        GLuint fragment = CompileShaderCode(fragCode.c_str(), GL_FRAGMENT_SHADER);
        glAttachShader(program, fragment);
        
        LinkProgram(program);
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        return program;
    }
    
private:
    static string LoadShaderFile (const GLchar *filePath) {
        ifstream shaderFile(filePath);
        stringstream shaderStream("");
        
        if(!shaderFile.is_open()) {
            Debug::LogError("<Shader> reading failed: " + string(filePath));
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
    
    static GLint CompileShaderCode(const GLchar *code, const GLenum shaderType) {
        GLint success;
        GLchar infoLog[512];
        GLuint result = glCreateShader(shaderType);
        glShaderSource(result, 1, &code, NULL);
        glCompileShader(result);
        
        glGetShaderiv(result, GL_COMPILE_STATUS, &success );
        if (!success) {
            glGetShaderInfoLog(result, 512, NULL, infoLog);
            Debug::LogError("<Shader> compilation failed: " + string(infoLog));
        }
        return result;
    }
    
    static void LinkProgram(GLuint program) {
        GLint success;
        GLchar infoLog[512];
        glLinkProgram(program);
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(program, 512, NULL, infoLog);
            Debug::LogError("<Shader> linking failed: " + string(infoLog));
        }
    }
};

class Material {
    GLuint program;
    
public:
    vector<Texture> textures;

    Material (GLuint program) {
        this->program = program;
    }
    
    Material (GLuint program, vector<Texture> textures) {
        this->program = program;
        this->textures = textures;
    }
    
    void AddTexture(GLuint textureName, GLenum textureType) {
        Texture textureData;
        textureData.name = textureName;
        textureData.type = textureType;
        this->textures.push_back(textureData);
    }
    
    void UseProgram(GLuint program) {
        this->program = program;
        glUseProgram(this->program);
    }
    
    void SetProperty (glm::mat4 projection, glm::mat4 view, glm::mat4 model) {
        glUniformMatrix4fv(glGetUniformLocation(this->program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(this->program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(this->program, "model"), 1, GL_FALSE, glm::value_ptr(model));
    }
    
    void BindTextures () {
        for (int i = 0; i < this->textures.size(); i++) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(this->textures[i].type, this->textures[i].name);
            glUniform1i(glGetUniformLocation(this->program, "texture" + i ), i);
        }
    }
    
    void UnbindTextures () {
        for (int i = 0; i < this->textures.size(); i++) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(this->textures[i].type, 0);
        }
    }
};
