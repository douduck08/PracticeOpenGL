#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "util.h"

class RenderTexture {
    GLint width, height;
    GLint screenWidth, screenHeight;
    GLuint FBO, depthrenderbuffer;
    
public:
    GLuint textureName;
    
    RenderTexture (GLint width, GLint height, GLint screenWidth, GLint screenHeight) {
        this->width = width;
        this->height = height;
        this->screenWidth = screenWidth;
        this->screenHeight = screenHeight;
        
        glGenTextures(1, &textureName);
        glBindTexture(GL_TEXTURE_2D, textureName);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        
        glGenFramebuffers(1, &FBO);
        glGenRenderbuffers(1, &depthrenderbuffer);
        
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureName, 0);
        
        GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, DrawBuffers);
        
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            //Debug::LogError("RenderTexture error");
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    
    void Bind () {
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        glViewport(0, 0, width, height);
    }
    
    void Unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, screenWidth, screenHeight);
    }
};

class ShadowMap {
    GLuint program;
    GLuint DBO;
    
public:
    GLuint depthTexture;
    glm::mat4 depthMVP;
    glm::mat4 depthBiasMVP;
    
    ShadowMap (glm::vec3 directLightDir) {
        glGenTextures(1, &depthTexture);
        glBindTexture(GL_TEXTURE_2D, depthTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        glGenFramebuffers(1, &DBO);
        glBindFramebuffer(GL_FRAMEBUFFER, DBO);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);
        
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            //Debug::LogError("ShadowMap error");
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        CalcDepthMVP(directLightDir);
    }
    
    void Bind (GLuint program) {
        this->program = program;
        glUseProgram(program);
        glUniformMatrix4fv(glGetUniformLocation(this->program, "depthMVP"), 1, GL_FALSE, glm::value_ptr(depthMVP));
        glBindFramebuffer(GL_FRAMEBUFFER, DBO);
        glViewport(0, 0, 1024, 1024);
    }
    
    void Unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, 1600, 1200);
    }
    
private:
    void CalcDepthMVP (glm::vec3 directLightDir) {
        glm::mat4 depthProjectionMatrix = glm::ortho<float>(-10, 10, -10, 10, -10, 20);
        glm::mat4 depthViewMatrix = glm::lookAt(-directLightDir, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        glm::mat4 depthModelMatrix = glm::mat4(1.0);
        this->depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
        
        glm::mat4 biasMatrix;
        biasMatrix = glm::translate(biasMatrix, glm::vec3(.5f, .5f, .5f));
        biasMatrix = glm::scale(biasMatrix, glm::vec3(.5f, .5f, .5f));
        this->depthBiasMVP = biasMatrix * this->depthMVP;
    }
};
