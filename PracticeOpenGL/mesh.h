#pragma once

#include <vector>
using namespace std;

#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "resources.h"
#include "material.h"
#include "util.h"

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
    
    void Render (Material *material) {
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

class MeshGenerator {
public:
    static Mesh GeneratePlane (int x = 1, int y = 1) {
        Mesh mesh;
        
        GLfloat deltaX = 1.f / x, deltaY = 1.f / y;
        for (int j = 0; j < y; j++) {
            for (int i = 0; i < x; i++) {
                Vertex vert;
                vert.position = glm::vec3(-.5f + deltaX * i, -.5f + deltaY * j, 0.f);
                vert.normal = glm::vec3(0.f, 0.f, 1.f);
                vert.texCoords = glm::vec2(deltaX * i, deltaY * j);
                vert.color = glm::vec3(1.f, 1.f, 1.f);
                mesh.vertices.push_back(vert);
                if (i == x - 1) {
                    vert.position = glm::vec3(.5f, -.5f + deltaY * j, 0.f);
                    vert.normal = glm::vec3(0.f, 0.f, 1.f);
                    vert.texCoords = glm::vec2(1.f, deltaY * j);
                    vert.color = glm::vec3(1.f, 1.f, 1.f);
                    mesh.vertices.push_back(vert);
                }
            }
            if (j == y - 1) {
                for (int i = 0; i < x; i++) {
                    Vertex vert;
                    vert.position = glm::vec3(-.5f + deltaX * i, .5f, 0.f);
                    vert.normal = glm::vec3(0.f, 0.f, 1.f);
                    vert.texCoords = glm::vec2(deltaX * i, 1.f);
                    vert.color = glm::vec3(1.f, 1.f, 1.f);
                    mesh.vertices.push_back(vert);
                    
                    if (i == x - 1) {
                        vert.position = glm::vec3(.5f, .5f, 0.f);
                        vert.normal = glm::vec3(0.f, 0.f, 1.f);
                        vert.texCoords = glm::vec2(1.f, 1.f);
                        vert.color = glm::vec3(1.f, 1.f, 1.f);
                        mesh.vertices.push_back(vert);
                    }
                }
            }
        }
        
        for (int j = 0; j < y; j++) {
            for (int i = 0; i < x; i++) {
                GLuint leftbot = i + j * (x + 1);
                mesh.triangles.push_back(leftbot);
                mesh.triangles.push_back(leftbot + x + 1);
                mesh.triangles.push_back(leftbot + 1);
                mesh.triangles.push_back(leftbot + 1);
                mesh.triangles.push_back(leftbot + x + 1);
                mesh.triangles.push_back(leftbot + x + 2);
            }
        }
        
        mesh.SetupBuffer();
        return mesh;
    }
    
    static Mesh GenerateSphere (int latitudeBands = 30, int longitudeBands = 30) {
        Mesh mesh;
        
        GLfloat deletaTheta = M_PI / latitudeBands;
        GLfloat deletaPhi = 2.f * M_PI / longitudeBands;
        for (int latNum = 0; latNum <= latitudeBands; latNum++) {
            GLfloat theta = deletaTheta * latNum;
            GLfloat sinTheta = sin(theta);
            GLfloat cosTheta = cos(theta);
            for (int longNum = 0; longNum <= longitudeBands; longNum++) {
                GLfloat phi = deletaPhi * longNum;
                GLfloat sinPhi = sin(phi);
                GLfloat cosPhi = cos(phi);
                
                Vertex vert;
                vert.normal = glm::normalize(glm::vec3(cosPhi * sinTheta, cosTheta, sinPhi * sinTheta));
                vert.position = vert.normal * .5f;
                vert.texCoords = glm::vec2(1.f - (GLfloat)longNum / longitudeBands, 1.f - (GLfloat)latNum / latitudeBands);
                vert.color = glm::vec3(1.f, 1.f, 1.f);
                mesh.vertices.push_back(vert);
            }
        }
        
        for (int latNum = 0; latNum < latitudeBands; latNum++) {
            for (int longNum = 0; longNum < longitudeBands; longNum++) {
                GLuint lefttop = longNum + latNum * (longitudeBands + 1);
                GLuint righttop = lefttop + longitudeBands + 1;
                
                mesh.triangles.push_back(lefttop);
                mesh.triangles.push_back(lefttop + 1);
                mesh.triangles.push_back(righttop);
                mesh.triangles.push_back(righttop);
                mesh.triangles.push_back(lefttop + 1);
                mesh.triangles.push_back(righttop + 1);
            }
        }
        
        mesh.SetupBuffer();
        return mesh;
    }
    
    static Mesh GenerateCube (int x = 1, int y = 1) {
        Mesh mesh;
        Mesh plane = MeshGenerator::GeneratePlane(x, y);
        
        glm::mat4 transform;
        transform = glm::translate(transform, glm::vec3(0.f, 0.f, 0.5f));
        mesh.Combine(&plane, transform);
        transform = glm::mat4();
        transform = glm::translate(transform, glm::vec3(0.f, 0.f, -0.5f));
        transform = glm::rotate(transform, 3.14f, glm::vec3(0.f, 1.f, 0.f));
        mesh.Combine(&plane, transform);
        
        transform = glm::mat4();
        transform = glm::translate(transform, glm::vec3(0.5f, 0.f, 0.f));
        transform = glm::rotate(transform, 1.571f, glm::vec3(0.f, 1.f, 0.f));
        mesh.Combine(&plane, transform);
        transform = glm::mat4();
        transform = glm::translate(transform, glm::vec3(-0.5f, 0.f, 0.f));
        transform = glm::rotate(transform, -1.571f, glm::vec3(0.f, 1.f, 0.f));
        mesh.Combine(&plane, transform);

        transform = glm::mat4();
        transform = glm::translate(transform, glm::vec3(0.f, 0.5f, 0.f));
        transform = glm::rotate(transform, -1.571f, glm::vec3(1.f, 0.f, 0.f));
        mesh.Combine(&plane, transform);
        transform = glm::mat4();
        transform = glm::translate(transform, glm::vec3(0.f, -0.5f, 0.f));
        transform = glm::rotate(transform, 1.571f, glm::vec3(1.f, 0.f, 0.f));
        mesh.Combine(&plane, transform);

        mesh.SetupBuffer();
        return mesh;
    }
};
