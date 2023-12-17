#pragma once


#define GLEW_STATIC
#define GLM_SWIZZLE

#include <vector>
#include <iostream>
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>


// default camera variables
const GLfloat YAW = -90.0f;
const GLfloat PITCH = 0.0f;
const GLfloat SPEED = 6.0f;
const GLfloat SENSITIVTY = 0.25f;
const GLfloat ZOOM = 45.0f;

enum cameraDirection {
    FORWARD,
    BACKWARD,
    LEFT ,
    RIGHT
};

class Camera
{

private:
    // Camera Attributes
    glm::vec3 position;
    glm::vec3 front;    // controls movement forward (> 0) and backward (< 0)
    glm::vec3 up;
    glm::vec3 right;    // controls movement to the right (> 0) and to the left (< 0)
    glm::vec3 worldUp;

    GLfloat yaw;
    GLfloat pitch;

    GLfloat movementSpeed;          // controls the camera movement speed 
    GLfloat mouseSensitivity;       // controls the camera rotation speed
    GLfloat zoom;


    void updateVectors()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
        front.y = sin(glm::radians(this->pitch));
        front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
        this->front = glm::normalize(front);
        this->right = glm::normalize(glm::cross(this->front, this->worldUp));
        this->up = glm::normalize(glm::cross(this->right, this->front));
    }


public:
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW, GLfloat pitch = PITCH) : 
        front(glm::vec3(0.0f, 0.0f, -1.0f)), 
        movementSpeed(SPEED), 
        mouseSensitivity(SENSITIVTY), 
        zoom(ZOOM)
    {
        this->position = position;
        this->worldUp = up;
        this->yaw = yaw;
        this->pitch = pitch;
        this->updateVectors();
    }

    void checkPosition()
    {
        std::cout << "camera position  x: " << this->position.x << " y: " << this->position.y << " z: " << this->position.z << std::endl;
    }

    // moves the camera based on keyboard input
    void move(cameraDirection direction, GLfloat deltaTime)
    {
        GLfloat velocity = this->movementSpeed * deltaTime;

        if (direction == FORWARD)
        {
            this->position += this->front * velocity;
        }

        if (direction == BACKWARD)
        {
            this->position -= this->front * velocity;
        }

        if (direction == LEFT)
        {
            this->position -= this->right * velocity;
        }

        if (direction == RIGHT)
        {
            this->position += this->right * velocity;
        }
    }

    void changeCameraAngle(GLfloat xOffset, GLfloat yOffset, GLboolean constrainPitch = true)
    {
        xOffset *= this->mouseSensitivity;
        yOffset *= this->mouseSensitivity;
        
        this->yaw += xOffset;
        this->pitch += yOffset;

        if (constrainPitch)
        {
            if (this->pitch > 89.0f)
            {
                this->pitch = 89.0f;
            }

            if (this->pitch < -89.0f)
            {
                this->pitch = -89.0f;
            }
        }

        this->updateVectors();
    }

    void changeCameraPosition(GLfloat xOffset, GLfloat yOffset, GLboolean constrainPitch = true)
    {
        xOffset *= this->mouseSensitivity * 0.03f;
        yOffset *= this->mouseSensitivity * 0.03f;
       
        this->position += glm::vec3(xOffset, yOffset, 0.0f);
    }


    glm::mat4 getViewMatrix()
    {
        return glm::lookAt(this->position, this->position + this->front, this->up);
    }

    GLfloat getZoom()
    {
        return this->zoom;
    }

    glm::vec3 getPosition()
    {
        return this->position;
    }

    glm::vec3 getFront()
    {
        return this->front;
    }

};

