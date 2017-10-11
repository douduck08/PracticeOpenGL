//
//  Created by douduck08 on 2017/9/22.
//  Copyright © 2017 Leo Lu. All rights reserved.
//

#ifndef mesh_generator_h
#define mesh_generator_h

namespace dgb {
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
                    mesh.triangles.push_back(leftbot + 1);
                    mesh.triangles.push_back(leftbot + x + 1);
                    mesh.triangles.push_back(leftbot + 1);
                    mesh.triangles.push_back(leftbot + x + 2);
                    mesh.triangles.push_back(leftbot + x + 1);
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
}

#endif /* mesh_generator_h */
