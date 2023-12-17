#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include "shaderprogram.h"
#include <glm/gtc/type_ptr.hpp>

using namespace std;

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Texture
{
    GLuint id;
    string type;
    aiString path;
};

class Mesh
{

private:
  
    GLuint VAO, VBO, EBO;

    string name;
    vector<Vertex> vertices;
    vector<GLuint> indices;
    vector<Texture> textures;

    GLfloat rotationLimit;

    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    Mesh* parent;

    glm::mat4 M; // model matrix


    // Initializes all the buffer objects/arrays
    void SetupMesh()
    {
        
        // Setup VAO
        glGenVertexArrays(1, &this->VAO);
        glBindVertexArray(this->VAO);

        // Setup VBO
        glGenBuffers(1, &this->VBO);
        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

        // Setup EBO
        glGenBuffers(1, &this->EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);

        // Load data into vertex buffers
        glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

        // Set the vertex attribute pointers and enable
        // Vertex Positions
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
        glEnableVertexAttribArray(0);

        // Vertex Normals
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
        glEnableVertexAttribArray(1);

        // Vertex Texture Coords
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0); // reset vertex buffer
        

    }


    // send current M matrix to the shading program
    void updateUniformM(ShaderProgram* shader)
    {
        shader->use();
        glUniformMatrix4fv(shader->u("M"), 1, false, glm::value_ptr(this->M));

    }

    void updateMeshMatrix()
    {
        this->M = glm::mat4(1.0f);
        
        //if mesh has a parent, rotate it around the parent's origin with the rotation = parent's rotation       
        if (this->parent != nullptr)
        {

            // move to parent's origin
            this->M = glm::translate(this->M, glm::vec3(this->parent->getPosition()));

            // rotate
            this->M = glm::rotate(this->M, glm::radians(this->parent->getRotation().x), glm::vec3(1.f, 0.f, 0.f));

            // move back to original position
            this->M = glm::translate(this->M, glm::vec3(-1.0f * this->parent->getPosition()));

        }
        
        // move to updated position and rotate on x and z
        // (nothing rotates on y) 
        this->M = glm::translate(this->M, this->position);
        this->M = glm::rotate(this->M, glm::radians(this->rotation.x), glm::vec3(1.f, 0.f, 0.f));
        this->M = glm::rotate(this->M, glm::radians(this->rotation.z), glm::vec3(0.f, 0.f, 1.f));
    }
    
    void updateAnimationPositions()
    {
        GLfloat risingSpeed = 0.2f * this->getRotationLimit(); // speed set to a positive or a negative number depending on the rotation limit (rotation direction)
        GLfloat fallingSpeed = 0.175f * this->getRotationLimit();

        // MESH MOVEMENT
        if (this->isRising)
        {
            // rotating the lid (on z axis)
            if (this->name == "lid")
            {
                risingSpeed = 0.05f * this->getRotationLimit();
                if ((this->rotation.z < this->rotationLimit && this->rotationLimit > 0) || (this->rotation.z > this->rotationLimit && this->rotationLimit < 0))
                {
                    this->rotate(glm::vec3(0.f, 0.f, risingSpeed));
                }
                else
                {
                    this->isRising = false;
                }

            }
            // rotating the keys (on x axis)
            else
            {
                if ((this->rotation.x < this->rotationLimit && this->rotationLimit > 0) || (this->rotation.x > this->rotationLimit && this->rotationLimit < 0))
                {
                    this->rotate(glm::vec3(risingSpeed, 0.f, 0.f));
                }
                else
                {
                    this->isRising = false;
                }
            }
        }

        else if (this->isFalling)
        {
            // rotating the lid (on z axis)
            if (this->name == "lid")
            {
                fallingSpeed = 0.05f * this->getRotationLimit();
                if ((this->rotation.z > 0 && this->rotationLimit > 0) || (this->rotation.z < 0 && this->rotationLimit < 0))
                {
                    this->rotate(glm::vec3(0.f, 0.f, -1.0f * fallingSpeed));
                }
                else if (this->rotation.z <= 0)
                {
                    //if rotation reached min value (0) -> stop falling
                    this->isFalling = false;

                    // make sure that rotation doesnt go below 0:
                    this->setRotation(glm::vec3(0.0f));
                }

            }
            // rotating the keys (on x axis)
            else
            {
                if ((this->rotation.x > 0 && this->rotationLimit > 0) || (this->rotation.x < 0 && this->rotationLimit < 0))
                {
                    this->rotate(glm::vec3(-1.0f * fallingSpeed, 0.f, 0.f));
                }
                else if (this->rotation.x <= 0)
                {
                    //if rotation reached min value (0) -> stop falling
                    this->isFalling = false;

                    // make sure that rotation doesnt go below 0:
                    this->setRotation(glm::vec3(0.0f));
                }
            }
        }
    }
    

public:

    bool isRising;
    bool isFalling;

    Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures)
    {
        this->name = "unknown";
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        this->position = glm::vec3(0.0f);
        this->scale = glm::vec3(1.0f);
        this->rotation = glm::vec3(0.0f);

        // variables controlling the up & down animations on objects
        this->isRising = false;
        this->isFalling = false;

        // for perent-relative transformations
        this->parent = nullptr;

        this->SetupMesh();
        this->updateMeshMatrix();
    }


    // Render the mesh
    void Draw(ShaderProgram* shader)
    {
        for (GLuint i = 0; i < this->textures.size(); i++)
        {
            string name = this->textures[i].type;

            // send tecture to the shader
            glUniform1i(shader->u(name.c_str()), this->textures[i].id );

            // activate texture
            glActiveTexture(GL_TEXTURE0 + this->textures[i].id);

            // bind the texture
            glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
        }

        updateUniformM(shader);  // re-sends the M matrix to the shader program
        updateAnimationPositions(); // sets the right rotation attributes depending on whether the mesh is currently in motion (isFalling, isRising)
        updateMeshMatrix();     // applies animation transformations to the M matrix

        // Draw mesh
        glBindVertexArray(this->VAO);
        glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

        
    void setPosition(const glm::vec3 position)
    {
        this->position = position;
    }

    void setRotation(const glm::vec3 rotation)
    {
        this->rotation = rotation;
    }

    void setScale(const glm::vec3 setScale)
    {
        this->scale = scale;
    }

    void setRotationLimit(GLfloat limit)
    {
        this->rotationLimit = limit;
    }

    void setName(string name)
    {
        this->name = name;
    }

    void setParent(Mesh* parent)
    {
        this->parent = parent;
    }


    glm::vec3 getPosition()
    {
        return this->position;
    }

    glm::vec3 getRotation()
    {
        return this->rotation;
    }

    glm::vec3 getScale()
    {
        return this->scale;
    }

    GLfloat getRotationLimit()
    {
        return this->rotationLimit;
    }

    string getName()
    {
        return this->name;
    }

    Mesh* getParent()
    {
        return this->parent;
    }

    void printTexturesInfo()
    {
        std::cout << "\tNumber of textures: " << this->textures.size() << std::endl;
        for (int i = 0; i < this->textures.size(); i++)
        {
            std::cout << "\t" << i << ") Texture ID: " << this->textures[i].id << ";\tType: " << this->textures[i].type << ";\t\tPath: " << this->textures[i].path.C_Str() << std::endl;

        }
    }

   
    void move(const glm::vec3 translation)
    {
        this->position += translation;
    }

    void rotate(const glm::vec3 rotation)
    {
        this->rotation += rotation;
    }


    
};

