#pragma once

// Implementa��o da biblioteca STB para carregar imagens
#define STB_IMAGE_IMPLEMENTATION
#include "Libs/stb_image.h"

// Bibliotecas OpenGL e GLM
#include <glad/glad.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/glm.hpp>
// Bibliotecas do projeto
#include "transformController.h"
#include "shader.h"
#include "gameObject.h"
#include "main.h"

// Bibliotecas padr�o C++
#include <stdexcept>
#include <iostream>
#include <string>
#include <vector>


void generateTexture(unsigned int& texture, const char* path)
{
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (!data)
    {
        std::cout << "Could not load texture: " << path << "\n";
        return;
    }
    std::cout << path << "\n";

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Adjust texture format based on the number of channels
    GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

GameObject::GameObject(const glm::vec3& inicialPos, const glm::vec3& inicialRot, const float* vertices, const int& numVertices, Shader* shaderProgramRef, const bool& isStatic) : TransformController(inicialPos, inicialRot)
{
    //Make EBO OPTMIZATION
    isStaticObj = isStatic;
    
    enablePhysicalRepresentation(vertices, numVertices, shaderProgramRef);
}
void GameObject::enablePhysicalRepresentation(const float* vertices, const int& numVertices, Shader* shaderProgramRef)
{
    shaderProgram = shaderProgramRef;
    verticesNum = numVertices;
    sizeOfVerticesObj = sizeof(vertices[0]) * verticesNum;


    verticesObj = new float[verticesNum];

    std::copy(vertices, vertices + verticesNum, verticesObj);

    // Resize vertex normals based on verticesNum
    vertexNormals.resize(3 * verticesNum, 0.0f);

    updateNormals();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //Could be sizeOfVertices + sizeOfVertices because is the same size but doing vertexNormals.size() * sizeof(float) to be explicit
    size_t sizeOfNormals = vertexNormals.size() * sizeof(float);

    glBufferData(GL_ARRAY_BUFFER, sizeOfVerticesObj + sizeOfNormals, nullptr, isStaticObj ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeOfVerticesObj, verticesObj);


    glBufferSubData(GL_ARRAY_BUFFER, sizeOfVerticesObj, sizeOfNormals, &vertexNormals[0]);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(sizeOfVerticesObj));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

GameObject::~GameObject()
{
    for (int textureId : texturesIds)
    {
        glDeleteTextures(1, (GLuint *)&textureId);
    }
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    if (verticesObj)
    {
        delete[] verticesObj;
        verticesObj = nullptr;
        verticesNum = -1;
    }
    
}
void GameObject::setTextures(const float* textureCoords, const int& sizeOftextureCoords, const char** paths, const int& numberOfTextures)
{
    this->mustHaveRenderAtribb("setTextures");

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    size_t sizeOfNormals = vertexNormals.size() * sizeof(float);

    glBufferData(GL_ARRAY_BUFFER, sizeOfVerticesObj + sizeOfNormals + sizeOftextureCoords, nullptr, isStaticObj ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);

    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeOfVerticesObj, verticesObj);

    glBufferSubData(GL_ARRAY_BUFFER, sizeOfVerticesObj, sizeOfNormals, &vertexNormals[0]);

    glBufferSubData(GL_ARRAY_BUFFER, sizeOfVerticesObj + sizeOfNormals, sizeOftextureCoords, textureCoords);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(sizeOfVerticesObj));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(sizeOfVerticesObj + sizeOfNormals));
    glEnableVertexAttribArray(2);
   
   

    

    texturesIds.reserve(numberOfTextures);

    

    // Vetor para testes de blending
    std::vector<float> blend;
    blend.reserve(numberOfTextures);

    std::vector<int> textureUnits;
    textureUnits.reserve(numberOfTextures);

    for (int i = 0; i < numberOfTextures; i++)
    {
        unsigned int texturesId;
        generateTexture(texturesId, paths[i]);
     
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, texturesId);


        texturesIds.push_back(texturesId);
        textureUnits.push_back(i);

        // Valores de blend de teste
        blend.push_back(0.5f);
    }

   
    glBindVertexArray(0);
}
void GameObject::prepareRender()
{
    this->mustHaveRenderAtribb("prepareRender");

    shaderProgram->use();

    glm::mat4 model = this->getModelMatrix();
    this->shaderProgram->setMat4("model", model);

    this->shaderProgram->setMat3("model3", glm::mat3(model));

    shaderProgram->setVec4("color",material->color);

    shaderProgram->setFloat("roughness", material->roughness);

    shaderProgram->setFloat("amountOfSpecular", material->amountOfSpecular);

    for (int i = 0; i < texturesIds.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, texturesIds[i]);
    }
    glBindVertexArray(VAO);
}
void GameObject::disableTextures()
{
    for (int i = 0; i < texturesIds.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0); // Desvinculando a textura
    }

}
void GameObject::setUpdateFunc(std::function<void(GameObject*)> func)
{
    updateFunc = func;
}
void GameObject::Update()
{
    if (updateFunc != nullptr)
    {
        updateFunc(this);
    }
    
}
bool containsNaN(const glm::vec3& vec) 
{
    return std::isnan(vec.x) || std::isnan(vec.y) || std::isnan(vec.z);
}


void GameObject::updateNormals()
{
    this->mustHaveRenderAtribb("updateNormals");
    std::fill(vertexNormals.begin(), vertexNormals.end(), 0.0f); // Reset normals

    // Interior point for the object (assuming the object is centered at the origin)
    glm::vec3 interiorPoint = glm::vec3(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < verticesNum; i += 9)
    {
        glm::vec3 v1 = glm::vec3(verticesObj[i], verticesObj[i + 1], verticesObj[i + 2]);
        glm::vec3 v2 = glm::vec3(verticesObj[i + 3], verticesObj[i + 4], verticesObj[i + 5]);
        glm::vec3 v3 = glm::vec3(verticesObj[i + 6], verticesObj[i + 7], verticesObj[i + 8]);

        glm::vec3 normal = glm::cross(v2 - v1, v3 - v1);

        if (containsNaN(normal) || glm::length(normal) < 1e-6f)
        {
            normal = glm::vec3(0.0f, 0.0f, 1.0f);
        }
        else
        {
            normal = glm::normalize(normal);

            glm::vec3 vectorToFace = v1 - interiorPoint;  
            if (glm::dot(vectorToFace, normal) < 0.0f)
            {
                normal = -normal;
            }
        }

        vertexNormals[i] = normal.x;
        vertexNormals[i + 1] = normal.y;
        vertexNormals[i + 2] = normal.z;

        vertexNormals[i + 3] = normal.x;
        vertexNormals[i + 4] = normal.y;
        vertexNormals[i + 5] = normal.z;

        vertexNormals[i + 6] = normal.x;
        vertexNormals[i + 7] = normal.y;
        vertexNormals[i + 8] = normal.z;
    }


    for (int i = 0; i < vertexNormals.size(); i += 3)
    {
        glm::vec3 accumulatedNormal = glm::vec3(vertexNormals[i], vertexNormals[i + 1], vertexNormals[i + 2]);
        glm::vec3 normalizedNormal = glm::normalize(accumulatedNormal);

        vertexNormals[i] = normalizedNormal.x;
        vertexNormals[i + 1] = normalizedNormal.y;
        vertexNormals[i + 2] = normalizedNormal.z;
    }


    // Atualizar dados do buffer
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, sizeOfVerticesObj, vertexNormals.size() * sizeof(float), &vertexNormals[0]);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

bool GameObject::hasVertices()
{
    if (verticesNum == -1)
        return false;

    return true;
}

void GameObject::mustHaveRenderAtribb(std::string methodName)
{
    if (!hasVertices())
    {
        throw std::runtime_error("[ERROR] GameObject must have vertices when executing the method: " + methodName);
    }

    if (shaderProgram == nullptr)
    {
        throw std::runtime_error("[ERROR] GameObject must have shaderProgram when executing the method: " + methodName);
    }
}

int GameObject::getVerticesNum()
{
    return verticesNum;
}

Shader* GameObject::getShaderPointer() const
{
    return shaderProgram;
}

unsigned int GameObject::getVAO()
{
    return VAO;
}
unsigned int GameObject::getVBO()
{
    return VBO;
}
std::vector<unsigned int>* GameObject::getTextureIds()
{
    return &texturesIds;
}
void GameObject::setMaterial(Material* newMaterial)
{
    material = newMaterial;
}