#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Debug {
public:
    static void Log (string msg) {
        printf("[Log] %s\n", msg.c_str());
    }
    
    static void LogWaring (string msg) {
        printf("[Warning] %s\n", msg.c_str());
    }
    
    static void LogError (string msg) {
        printf("[Error] %s\n", msg.c_str());
    }
};

class Util {
public:
    static void InitGLFW () {
        if (!glfwInit()) {
            Debug::LogError("Failed to initialize GLFW");
        }
        
        glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
        glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
        glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
        glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
        glfwWindowHint( GLFW_RESIZABLE, GL_FALSE );
    }
};
