#pragma once

#include <GL/glew.h>

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
