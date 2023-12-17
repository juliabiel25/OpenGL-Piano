#define GLM_FORCE_RADIANS

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "shaderprogram.h"
#include "camera.h"
#include "mesh.h"
#include "model.h"


// Properties
int SCREEN_WIDTH = 1000, SCREEN_HEIGHT = 800;
float aspectRatio = 1000/800;

// Function prototypes
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void DoAction(Model* model, int* keyPointer);
void windowResizeCallback(GLFWwindow* window, int width, int height);
void error_callback(int error, const char* description) {
    fputs(description, stderr);
}



// Camera
Camera camera(glm::vec3(0.0f, 0.5f, 5.0f));

// Key press counters & controls
int keyPressCounter[1024] = { 0 };
bool pianoKeyPressed[89] = { false };
bool pianoKeyReleased[89] = { false };
GLfloat lastX = 400, lastY = 300;
bool firstMouseClick = true;

// Frame-to-frame time interval
GLfloat deltaTime = 0.0f;
GLfloat prevFrame = 0.0f;

// shader handle
ShaderProgram* sp;



int main()
{
    // --- INITIALIZATION --- // 

    // variables
    int keyPointer = 88; // for pressing piano keys with arrows

    // set error callback
    glfwSetErrorCallback(error_callback);

    // Initialize GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW.\n");
        exit(EXIT_FAILURE);
    }

    // Create a GLFW window
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL", NULL, NULL);
    if (!window) 
    {
        fprintf(stderr, "Failed to create a window object.\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window); // activate the window
    glfwSwapInterval(1);    // wait 1s before swapping buffers

    // Initialize GLFW
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize GLFW.\n");
        exit(EXIT_FAILURE);
    }

    // Init OpenGL Program
    glClearColor(1, 1, 1, 1);
    glEnable(GL_DEPTH_TEST);

    // Set the window-based callbacks
    glfwSetWindowSizeCallback(window, windowResizeCallback);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetScrollCallback(window, ScrollCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);

    // Setup and compile our shaders
    sp = new ShaderProgram("vertex_shader.glsl", NULL, "fragment_shader.glsl");
    

    // Load models
    vector<string> paths = {
        "models/-z_front/key_base_black.obj",           // 0 - base variant
        "models/-z_front/key_base_white01.obj",         // 1 - base variant
        "models/-z_front/key_base_white02.obj",         // 2 - base variant
        "models/-z_front/key_base_white03.obj",         // 3 - base variant
        "models/-z_front/key_hammer.obj",               // 4
        "models/-z_front/key_wippen.obj",               // 5
        "models/-z_front/key_repetition_lever.obj",     // 6
        "models/-z_front/key_jack.obj",                 // 7 
        "models/-z_front/key_top_bar.obj",              // 8 - immobile
        "models/-z_front/key_jack_cylinder.obj",        // 9 - immobile
        "models/-z_front/key_bottom_holder.obj",        // 10 - immobile
        "models/-z_front/piano_body_open.obj",          // 11-13 - PIANO - immobile
        "models/-z_front/piano_lid_open.obj",           // 14 - piano lid
        "models/-z_front/floor.obj",                    // 15 - floor
        "models/-z_front/cube.obj"                      // 16-17 - light source markers

    };
    Model model(paths);



    // ----- MAIN LOOP ----- //


    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set delta Time
        GLfloat currentFrame = glfwGetTime();
        deltaTime = 0.5f * (currentFrame - prevFrame);
        prevFrame = currentFrame;

        // Set view matrix
        glm::mat4 V = camera.getViewMatrix();

        // Set projection matrix
        glm::mat4 P = glm::perspective(camera.getZoom(), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);

        // Set model matrix (and adjust model position)
        glm::mat4 M = glm::mat4(1.0f);

        sp->use();

        // send parametrs to the shader program
        glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
        glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));
        glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));


        // draw the model: pass all the verticies, vertex colors and texture coordinates to the shader program
        model.Draw(sp);
 
        // call events
        glfwPollEvents();

        // execute animations, actions etc...
        DoAction(&model, &keyPointer);



        glfwSwapBuffers(window);
    }


    delete sp;
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}




// Moves/alters the camera positions based on user input
void DoAction(Model* model, int* keyPointer)
{
    // Camera controls
    if (keyPressCounter[GLFW_KEY_W] > 0 )
    {
        camera.move(FORWARD, deltaTime);
    }

    if (keyPressCounter[GLFW_KEY_S] > 0 )
    {
        camera.move(BACKWARD, deltaTime);
    }

    if (keyPressCounter[GLFW_KEY_A] > 0)
    {
        camera.move(LEFT, deltaTime);
    }

    if (keyPressCounter[GLFW_KEY_D] > 0)
    {
        camera.move(RIGHT, deltaTime);
    }

    // Piano keyboard controls - navigating with arrows
    /*
    if (*keyPointer > 0 && *keyPointer < 88)
    {
        if (!pianoKeyPressed[*keyPointer])
        {
            model->keyPressed(*keyPointer);
            pianoKeyPressed[*keyPointer] = true;
        }
    }
    
    if (keyPressCounter[GLFW_KEY_RIGHT] == 1)
    {
        keyPressCounter[GLFW_KEY_RIGHT]++;

        cout << "Going right! Key pointer " << *keyPointer << endl;
        if (*keyPointer <= 87)
        {
            *keyPointer += 1;
            if (*keyPointer > 0 && pianoKeyPressed[*keyPointer - 1])
            {
                model->keyReleased(*keyPointer - 1);
                pianoKeyPressed[*keyPointer - 1] = false;
            }
        }
    }
    
    if (keyPressCounter[GLFW_KEY_LEFT] == 1)
    {
        keyPressCounter[GLFW_KEY_LEFT]++;

        cout << "Going left! Key pointer " << *keyPointer << endl;
        if (*keyPointer >= 1)
        {
            *keyPointer -= 1;
            if (*keyPointer < 88 && pianoKeyPressed[*keyPointer + 1])
            {
                model->keyReleased(*keyPointer + 1);
                pianoKeyPressed[*keyPointer + 1] = false;
                
            }
        }
    }
    */
    
    // Piano keyboard controls - navigating with NUMPAD
    for (int i = 0; i <= 10; i++)
    {
        // if numpad number pressed and key is not up - press it
        if (keyPressCounter[320 + i] > 0 && !pianoKeyPressed[87 - i])
        {
            model->keyPressed(87 - i);
            pianoKeyPressed[87 - i] = true;
        }
        // if numpad number not pressed and key is up - release it
        if (keyPressCounter[320 + i] == 0 && pianoKeyPressed[87 - i])
        {
            model->keyReleased(87 - i);
            pianoKeyPressed[87 - i] = false;
        }
    }

    

    // Piano lid controls
    if (keyPressCounter[GLFW_KEY_O] == 1)
    {
        model->openLid();
        keyPressCounter[GLFW_KEY_O] = 0;
    }


    if (keyPressCounter[GLFW_KEY_C] == 1)
    {
        model->closeLid();
        keyPressCounter[GLFW_KEY_C] = 0;
    }

    
}

// called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }


    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
        {
           
            keyPressCounter[key] = 1;
        }
        else if (action == GLFW_RELEASE)
        {
            keyPressCounter[key] = 0;
        }
    }


}

// detecting scrolling for camera zoom
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    //cout << "xoffset: " << xoffset << " yoffset: " << yoffset << endl;
    if (yoffset == 1)
    {
        camera.move(FORWARD, 5 * deltaTime);
    }
    else if (yoffset == -1)
    {
        camera.move(BACKWARD, 5 * deltaTime);
    }
}

// detecting mouse clicks for operating the camera
void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{  
    if (firstMouseClick)
    {
        lastX = xPos;
        lastY = yPos;
        firstMouseClick = false;
    }

    GLfloat xOffset = xPos - lastX;
    GLfloat yOffset = lastY - yPos;  // Reversed

    glm::vec3 movement = glm::vec3(xOffset, yOffset, 0.0f);
    
    lastX = xPos;
    lastY = yPos;

    // an attempt at blender-style camera controls 
    // (only works in front view for now)
    if (keyPressCounter[GLFW_KEY_LEFT_SHIFT] > 0)
    {
        //cout << "detected SHIFT + SCROLL!\n";
        camera.changeCameraPosition(-1*xOffset, -1*yOffset);
    }
    else
    {
        //cout << "detected SCROLL!\n";
        camera.changeCameraAngle(xOffset, yOffset);
    }

}

// detecting scrollwheel clicks for camera operation
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    cout << button << " " << action << " " << mods << endl;
    
    // scroll wheel: press
    if (button == 2 && action == 1)
    {
        glfwSetCursorPosCallback(window, MouseCallback);
    }
    else if (button == 2 && action == 0)
    {
        glfwSetCursorPosCallback(window, NULL);
        firstMouseClick = true;
    }
}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
    if (height == 0) return;
    aspectRatio = (float)width / (float)height;
    glViewport(0, 0, width, height);
}