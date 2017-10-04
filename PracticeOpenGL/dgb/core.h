//
//  duck_game_box.h
//  Created by douduck08 on 2017/9/28.
//  Copyright © 2017年 douduck08. All rights reserved.
//

#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "assets.h"
#include "game_object.h"
#include "util.h"

namespace dgb {
    class Core {
        
    public:
        GLFWwindow *mainWindow;
        int frameBufferWidth, frameBufferHeight;
        
        Camera mainCamera;
        
        Core () {
            static bool isInitialized;
            if (!isInitialized) {
                Debug::Log("Initializing GLFW");
                if (!glfwInit()) {
                    Debug::LogError("Failed to initialize GLFW");
                }
                
                glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
                glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
                glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
                glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
                glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
            }
        }
        
        bool CreateWindow (const int windowWidth, const int windowHeight, const char* windowTitle) {
            this->mainWindow = glfwCreateWindow(windowWidth, windowHeight, windowTitle, nullptr, nullptr);
            if (nullptr == this->mainWindow ) {
                Debug::LogError("Failed to create GLFW window");
                glfwTerminate();
                return false;
            }
            
            glfwMakeContextCurrent(this->mainWindow);
            glewExperimental = GL_TRUE;
            if (GLEW_OK != glewInit()) {
                Debug::LogError("Failed to initialize GLEW");
                return false;
            }

            glfwGetFramebufferSize(this->mainWindow, &frameBufferWidth, &frameBufferHeight);
            this->mainCamera.SetViewport(0, 0, this->frameBufferWidth, this->frameBufferHeight);
            
            return true;
        }
    };
}
