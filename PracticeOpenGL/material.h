#pragma once

#include <string>
#include <fstream>
#include <sstream>
using namespace std;

#include <GL/glew.h>
#include "util.h"

class Shader {
public:
    GLuint Program;
    
    Shader(const GLchar *vertexPath, const GLchar *fragmentPath) {
        this->Program = glCreateProgram();
        
        const GLchar *vertCode = LoadShaderFile(vertexPath);
        GLuint vertex = CompileShaderCode(vertCode, GL_VERTEX_SHADER);
        glAttachShader(this->Program, vertex);
        
        const GLchar *fragCode = LoadShaderFile(fragmentPath);
        GLuint fragment = CompileShaderCode(fragCode, GL_FRAGMENT_SHADER);
        glAttachShader(this->Program, fragment);
        
        LinkProgram();
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }
    
    void Use( ) {
        glUseProgram(this->Program);
    }
    
private:
    const GLchar * LoadShaderFile (const GLchar *filePath) {
        ifstream shaderFile(filePath);
        stringstream shaderStream("");
        
        if(!shaderFile.is_open()) {
            Debug::LogError("<Shader> reading failed: " + string(filePath));
            return "";
        }
        
        string code = "";
        string line = "";
        while(!shaderFile.eof()) {
            getline(shaderFile, line);
            code.append(line + "\n");
            //shaderStream << line << '\n';
        }
        shaderFile.close();
        return code.c_str();
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
            Debug::LogError("<Shader> compilation failed: " + string(infoLog));
        }
        return result;
    }
    
    void LinkProgram() {
        GLint success;
        GLchar infoLog[512];
        glLinkProgram(this->Program);
        glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog( this->Program, 512, NULL, infoLog );
            Debug::LogError("<Shader> linking failed: " + string(infoLog));
        }
    }
};
