//
// Created by 李梦凡 on 2019/10/15.
//

//3D模型制作工具
//Blender：http://www.blender.org/
//3DS Max：http://www.autodesk.nl/products/3ds-max/overview
//Maya：http://www.autodesk.com/products/autodesk-maya/overview

//模型加载库
//assimp：https://github.com/assimp/assimp/releases/tag/v3.3.1

#ifndef MESH_H
#define MESH_H

#include <glad.h> // holds all OpenGL type declarations

#include <../glm/glm.hpp>
#include <../glm/gtc/matrix_transform.hpp>

#include "shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

using namespace std;

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
//    切线和副切线：http://www.opengl-tutorial.org/cn/intermediate-tutorials/tutorial-13-normal-mapping/
    // tangent
    glm::vec3 Tangent; // 切线
    // bitangent
    glm::vec3 Bitangent; // 副切线
};

struct Texture {
    unsigned int id;
    string type; // 反射贴图或者是镜面光贴图
    string path;
};

class Mesh {
public:
    /*  Mesh Data  */
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;
    unsigned int VAO; // 让我们能够访问它的顶点数组对象（实例化数组：小行星带中使用（4_10_2））

    /*  Functions  */
    // constructor
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures) {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        setupMesh();
    }

    // render the mesh
//    绘制网格
    void Draw(Shader shader) {
        // bind appropriate textures
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr = 1;
        unsigned int heightNr = 1;
        for (unsigned int i = 0; i < textures.size(); i++) {
            glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
            // retrieve texture number (the N in diffuse_textureN)
            string number;
            string name = textures[i].type;
            if (name == "texture_diffuse") {
                number = std::to_string(diffuseNr++);
            } else if (name == "texture_specular") {
                number = std::to_string(specularNr++); // transfer unsigned int to stream
            } else if (name == "texture_normal") {
//                法线贴图：https://www.cnblogs.com/wangchengfeng/p/3470310.html
//                人眼对物体的凹凸感觉，很大程度上取决于表面的光照明暗变化，如果能通过一张贴图在一个平面上表现出由凹凸造成的明暗变化，则可以让人眼感觉这个平面是凹凸不平的（虽然这个平面还是平的）。
//                法线贴图的制作过程：通过对比高模和低模的差异，生成法线贴图，在运行时使用法线贴图+低模即可表现出接近高模的效果，但是大大降低了显卡的负担。计算法线贴图的算法在3ds Max中已实现，美术人员只需针对同一个模型制作一个高模和一个低模，烘焙出一张法线贴图，在游戏中使用低模+法线贴图即可。
//                切线空间：https://blog.csdn.net/bonchoix/article/details/8619624
                number = std::to_string(normalNr++); // transfer unsigned int to stream
            } else if (name == "texture_height") {
                number = std::to_string(heightNr++); // transfer unsigned int to stream
            }
//            查看已有的贴图
//            cout << (name + number).c_str() << endl;

            // now set the sampler to the correct texture unit
//            纹理命名标准
            glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
            // and finally bind the texture
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }

        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
    }

private:
    /*  Render data  */
    unsigned int VBO, EBO;

    /*  Functions    */
    // initializes all the buffer objects/arrays
//    初始化缓冲
    void setupMesh() {
        // create buffers/arrays
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        // load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // set the vertex attribute pointers
        // vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) 0);
        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, TexCoords));
        // vertex tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Tangent));
        // vertex bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Bitangent));

        glBindVertexArray(0);
    }
};

#endif