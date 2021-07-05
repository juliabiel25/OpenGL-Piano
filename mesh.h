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
#include "shader.h"
#include "shaderprogram.h"
#include <glm/gtc/type_ptr.hpp>

using namespace std;

struct Vertex
{
    // Position
    glm::vec3 Position;
    // Normal
    glm::vec3 Normal;
    // TexCoords
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
    glm::vec3 parentOrigin;
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

        glBindVertexArray(0);
        

    }


    // send current M matrix to the shading program
    void updateUniforms(ShaderProgram* shader)
    {
        shader->use();
        glUniformMatrix4fv(shader->u("M"), 1, false, glm::value_ptr(this->M));
        //glUniformMatrix4fv(glGetUniformLocation(shader->Program, "model"), 1, GL_FALSE, glm::value_ptr(this->M));
    }

    void updateMeshMatrix()
    {
        this->M = glm::mat4(1.0f);
        
        //if mesh has a parent, rotate it around the parent's origin with the rotation = parent's rotation       
        if (this->parent != nullptr)
        {
            /*
            this->meshMatrix = glm::translate(this->meshMatrix, glm::vec3(-1.0f * this->parent->getPosition()));
            //cout << this->name << ": move by (" << -1.0f * this->parent->getPosition().x << ", " << -1.0f * this->parent->getPosition().y << ", " << -1.0f * this->parent->getPosition().z << ")\n";

            this->meshMatrix = glm::rotate(this->meshMatrix, glm::radians(this->parent->getRotation().x), glm::vec3(1.0f, 0.0f, 0.0f));
            //cout << this->name << ": rotate by " << this->parent->getRotation().x << " degrees\n";

            this->meshMatrix = glm::translate(this->meshMatrix, glm::vec3(this->parent->getPosition()));
            //cout << this->name << ": move by (" << this->parent->getPosition().x << ", " << this->parent->getPosition().y << ", " << this->parent->getPosition().z << ")\n";
            */

            // move to parent's origin
            this->M = glm::translate(this->M, glm::vec3(this->parent->getPosition()));

            // rotate
            this->M = glm::rotate(this->M, glm::radians(this->parent->getRotation().x), glm::vec3(1.f, 0.f, 0.f));

            // move back to original position
            this->M = glm::translate(this->M, glm::vec3(-1.0f * this->parent->getPosition()));

        }
        
        // Next, rotate around its origin and move to position
        // this->meshMatrix = glm::rotate(this->meshMatrix, glm::radians(this->rotation.x), glm::vec3(1.f, 0.f, 0.f));
        // this->meshMatrix = glm::translate(this->meshMatrix, this->position);
        
        // move to updated position and rotate on x and z 
        this->M = glm::translate(this->M, this->position);
        this->M = glm::rotate(this->M, glm::radians(this->rotation.x), glm::vec3(1.f, 0.f, 0.f));
        this->M = glm::rotate(this->M, glm::radians(this->rotation.z), glm::vec3(0.f, 0.f, 1.f));
       


        //===
        // doesn't seem to do anything:
        //===

        //this->meshMatrix = glm::translate(this->meshMatrix, glm::vec3(0.0f)); // position - origin (origin == position)
        
              
        
        //this->meshMatrix = glm::scale(this->meshMatrix, this->scale);



        //cout << "Mesh parentOrigin: (" << this->parentOrigin.x << ", " << this->parentOrigin.y << ", " << this->parentOrigin.z << ")\n";
        //cout << "Mesh position: (" << this->position.x << ", " << this->position.y << ", " << this->position.z << ")\n";
        
        // SAFETY COPY
        /*
        this->meshMatrix = glm::mat4(1.f);
        this->meshMatrix = glm::translate(this->meshMatrix, this->parentOrigin);
        this->meshMatrix = glm::rotate(this->meshMatrix, glm::radians(this->rotation.x), glm::vec3(1.f, 0.f, 0.f));
        this->meshMatrix = glm::rotate(this->meshMatrix, glm::radians(this->rotation.y), glm::vec3(0.f, 1.f, 0.f));
        this->meshMatrix = glm::rotate(this->meshMatrix, glm::radians(this->rotation.z), glm::vec3(0.f, 0.f, 1.f));
        this->meshMatrix = glm::translate(this->meshMatrix, this->position - this->parentOrigin);
        this->meshMatrix = glm::scale(this->meshMatrix, this->scale);
        */
        /*
        this->meshMatrix = glm::mat4(1.f);
        this->meshMatrix = glm::translate(this->meshMatrix, -1.0f*this->position);
        this->meshMatrix = glm::rotate(this->meshMatrix, glm::radians(this->rotation.x), glm::vec3(1.f, 0.f, 0.f));
        this->meshMatrix = glm::rotate(this->meshMatrix, glm::radians(this->rotation.y), glm::vec3(0.f, 1.f, 0.f));
        this->meshMatrix = glm::rotate(this->meshMatrix, glm::radians(this->rotation.z), glm::vec3(0.f, 0.f, 1.f));
        this->meshMatrix = glm::translate(this->meshMatrix, this->position);
        this->meshMatrix = glm::scale(this->meshMatrix, this->scale);*/
    }
    
    void updateAnimationPositions()
    {
        GLfloat risingSpeed = 0.1f * this->getRotationLimit(); // speed set to a positive or a negative number depending on the rotation limit (rotation direction)
        GLfloat fallingSpeed = 0.075f * this->getRotationLimit();

        /*
        // PARENT MOVEMENT
        if (this->parent != nullptr && this->parent->isRising)
        {
            // rotate at the same rotation speed as parent
            this->rotate(glm::vec3(0.1f * this->parent->getRotationLimit(), 0.0f, 0.0f));

            // move to the current origin 
        }
        else if (this->parent != nullptr && this->parent->isFalling)
        {
            // rotate at the same rotation speed as parent
            this->rotate(glm::vec3(-0.1f * this->parent->getRotationLimit(), 0.0f, 0.0f));
        }
        */

        // MESH MOVEMENT
        if (this->isRising)
        {

            if ((this->rotation.x < this->rotationLimit && this->rotationLimit > 0) || (this->rotation.x > this->rotationLimit && this->rotationLimit < 0))
            {
                this->rotate(glm::vec3(risingSpeed, 0.f, 0.f));

                // rotate around own origin
               // this->meshMatrix = glm::translate(this->meshMatrix, this->position);
               // this->meshMatrix = glm::rotate(this->meshMatrix, glm::radians(this->rotation.x), glm::vec3(1.f, 0.f, 0.f));
               // this->meshMatrix = glm::translate(this->meshMatrix, glm::vec3(0.f, 0.f, 0.f));
            }
            else
            {
                this->isRising = false;
            }
        }

        else if (this->isFalling)
        {
            if ((this->rotation.x > 0  && this->rotationLimit > 0) || (this->rotation.x < 0 && this->rotationLimit < 0))
            {
                this->rotate(glm::vec3(-1.0f * fallingSpeed, 0.f, 0.f));

                // rotate around own origin
               // this->meshMatrix = glm::translate(this->meshMatrix, this->position);
              // this->meshMatrix = glm::rotate(this->meshMatrix, glm::radians(this->rotation.x), glm::vec3(1.f, 0.f, 0.f));
               // this->meshMatrix = glm::translate(this->meshMatrix, glm::vec3(0.f, 0.f, 0.f));
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
    

public:

    bool isRising;
    bool isFalling;

    Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        this->parentOrigin = glm::vec3(0.0f);
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

    //void Draw(Shader shader)
    void Draw(ShaderProgram* shader)
    {
        // Bind appropriate textures
        GLuint diffuseNr = 1;
        GLuint specularNr = 1;

        for (GLuint i = 0; i < this->textures.size(); i++)
        {
           
            // Retrieve texture number (the N in diffuse_textureN)
            stringstream ss;
            string number;
            string name = this->textures[i].type;

            if (name == "texture_diffuse")
            {
                ss << diffuseNr++; // Transfer GLuint to stream
            }
            else if (name == "texture_specular")
            {
                ss << specularNr++; // Transfer GLuint to stream
            }

            number = ss.str();

            // Now set the sampler to the correct texture unit
            //glUniform1i(glGetUniformLocation(shader.Program, (name + number).c_str()), i);
            glUniform1i(shader->u((name + number).c_str()), i);

            // activate texture
            glActiveTexture(GL_TEXTURE0 + i);

            // And finally bind the texture
            glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
        }

        //// Also set each mesh's shininess property to a default value (if you want you could extend this to another mesh property and possibly change this value)
        //// glUniform1f(glGetUniformLocation(shader.Program, "material.shininess"), 16.0f);


        //updateUniforms(&shader);
        updateUniforms(shader);
        updateAnimationPositions(); // sets the right rotation depending on whether the mesh is currently in motion (isFalling, isRising)
        updateMeshMatrix();

        // Draw mesh
        glBindVertexArray(this->VAO);
        glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Always good practice to set everything back to defaults once configured.
        ////for (GLuint i = 0; i < this->textures.size(); i++)
        ////{
        ////    glActiveTexture(GL_TEXTURE0 + i);
        ////    glBindTexture(GL_TEXTURE_2D, 0);
        ////}
    }


    
    // Setters
    void setPosition(const glm::vec3 position)
    {
        this->position = position;
    }

    void setParentOrigin(const glm::vec3 parentOrigin)
    {
        this->parentOrigin = parentOrigin;
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


    // Getters
    glm::vec3 getPosition()
    {
        return this->position;
    }

    glm::vec3 getParentOrigin()
    {
        return this->parentOrigin;
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

    // Modifiers
    void move(const glm::vec3 translation)
    {
        this->position += translation;
        //this->parentOrigin += translation;
    }

    void rotate(const glm::vec3 rotation)
    {
        this->rotation += rotation;
    }


    
};

