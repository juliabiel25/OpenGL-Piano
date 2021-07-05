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
#include "shader.h"
#include "shaderprogram.h"
#include "camera.h"
#include "mesh.h"
#include "model.h"
#include "lodepng.h"


// Properties
const GLuint WIDTH = 1000, HEIGHT = 800;
int SCREEN_WIDTH, SCREEN_HEIGHT;
float aspectRatio = WIDTH / HEIGHT;

// Function prototypes
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void MouseCallback(GLFWwindow* window, double xPos, double yPos);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void DoAction(Model* model, int* keyPointer);
GLuint readTexture(const char* filename);

void error_callback(int error, const char* description) {
    fputs(description, stderr);
}

void windowResizeCallback(GLFWwindow* window, int width, int height) {
    if (height == 0) return;
    aspectRatio = (float)width / (float)height;
    glViewport(0, 0, width, height);
}

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
int keyPressCounter[1024] = { 0 };
bool pianoKeyPressed[89] = { false };
bool pianoKeyReleased[89] = { false };
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat prevFrame = 0.0f;

// shader handle
ShaderProgram* sp;



int main()
{
    // -- INITIALIZATION -- 

    // variables
    int keyPointer = 0;

    // set error callback
    glfwSetErrorCallback(error_callback);

    // Initialize GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW.\n");
        exit(EXIT_FAILURE);
    }

    // Initialize a GLFW window
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL", NULL, NULL);  //Utwórz okno 500x500 o tytule "OpenGL" i kontekst OpenGL.
    if (!window) 
    {
        fprintf(stderr, "Failed to create a window object.\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window); // activate the window
    glfwSwapInterval(1);    // wait 1s before swapping buffers
    glfwGetFramebufferSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT); //????????????

    // Initialize GLFW
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize GLFW.\n");
        exit(EXIT_FAILURE);
    }

    // Init OpenGL Program
    glClearColor(0, 0, 0, 1);
    glEnable(GL_DEPTH_TEST);

    // Set the window-based callbacks
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetScrollCallback(window, ScrollCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetWindowSizeCallback(window, windowResizeCallback);

    // Setup and compile our shaders
        //Shader shader("shaders/modelLoading.vs", "shaders/modelLoading.frag");
    sp = new ShaderProgram("v_simplest.glsl", NULL, "f_simplest.glsl");
    
    // Load textures
    vector<GLuint>textures = {
        readTexture("textures/metal.png")
    };

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
        "models/-z_front/key_bottom_holder.obj"         // 10 - immobile
    };
    Model model(paths);

    //glm::mat4 projection = glm::perspective(camera.GetZoom(), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);


    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set delta Time
        GLfloat currentFrame = glfwGetTime();
        deltaTime = 0.5f * (currentFrame - prevFrame);
        prevFrame = currentFrame;

        // Set view matrix
        glm::mat4 V = camera.GetViewMatrix();
        //glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        //glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));

        // Set projection matrix
        glm::mat4 P = glm::perspective(camera.GetZoom(), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);

        // Set model matrix (and adjust model position)
        glm::mat4 M = glm::mat4(1.0f);
        M = glm::translate(M, glm::vec3(0.0f, -1.75f, 0.0f));
        M = glm::scale(M, glm::vec3(0.2f, 0.2f, 0.2f));	

        sp->use();
        //shader.Use();

        // send parametrs to the shader program
        glUniformMatrix4fv(sp->u("P"), 1, false, glm::value_ptr(P));
        glUniformMatrix4fv(sp->u("V"), 1, false, glm::value_ptr(V));
        glUniformMatrix4fv(sp->u("M"), 1, false, glm::value_ptr(M));


       // glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(M));

        // draw the model: pass all the verticies, vertex colors and texture coordinates to the shader program
        model.Draw(sp);
 
        // call events
        glfwPollEvents();

        // execute animations, actions etc...
        DoAction(&model, &keyPointer);

        // Swap the buffers
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
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }

    if (keyPressCounter[GLFW_KEY_S] > 0 )
    {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }

    if (keyPressCounter[GLFW_KEY_A] > 0)
    {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }

    if (keyPressCounter[GLFW_KEY_D] > 0)
    {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }

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

        cout << "W prawo! Key pointer " << *keyPointer << endl;
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

        cout << "w lewo! Key pointer " << *keyPointer << endl;
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

    
}

// Is called whenever a key is pressed/released via GLFW
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (GLFW_KEY_ESCAPE == key && GLFW_PRESS == action)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
        {
           
            keyPressCounter[key] = 1;
            //keyReleaseCounter[key] = 0;
        }
        else if (action == GLFW_RELEASE)
        {
            keyPressCounter[key] = 0;
            // CHANGED THIS FOR THE PURPUSE OF PICKING KEYS WITH ARROWS
            // 
            //keyReleaseCounter[key]++; // after each release keyReleaseCounter[key] == 1 
            //keyPressCounter[key] = 0;
        }
    }
}


void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    cout << "xoffset: " << xoffset << " yoffset: " << yoffset << endl;
    if (yoffset == 1)
    {
        camera.ProcessKeyboard(FORWARD, 5 * deltaTime);
    }
    else if (yoffset == -1)
    {
        camera.ProcessKeyboard(BACKWARD, 5 * deltaTime);
    }
}

void MouseCallback(GLFWwindow* window, double xPos, double yPos)
{  
    if (firstMouse)
    {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    GLfloat xOffset = xPos - lastX;
    GLfloat yOffset = lastY - yPos;  // Reversed since y-coordinates go from bottom to left

    glm::vec3 movement = glm::vec3(xOffset, yOffset, 0.0f);
    
    lastX = xPos;
    lastY = yPos;

    if (keyPressCounter[GLFW_KEY_LEFT_SHIFT] > 0)
    {
        cout << "wykryto SHIFT + SCROLL!\n";
        camera.ChangeCameraPosition(-1*xOffset, -1*yOffset);
    }
    else
    {
        cout << "wykryto SCROLL!\n";
        camera.ChangeCameraAngle(xOffset, yOffset);
    }

}

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
        firstMouse = true;
    }
}

GLuint readTexture(const char* filename) {
    GLuint tex;
    glActiveTexture(GL_TEXTURE0);

    //Wczytanie do pamiêci komputera
    std::vector<unsigned char> image;   //Alokuj wektor do wczytania obrazka
    unsigned width, height;   //Zmienne do których wczytamy wymiary obrazka
    //Wczytaj obrazek
    unsigned error = lodepng::decode(image, width, height, filename);

    //Import do pamiêci karty graficznej
    glGenTextures(1, &tex); //Zainicjuj jeden uchwyt
    glBindTexture(GL_TEXTURE_2D, tex); //Uaktywnij uchwyt
    //Wczytaj obrazek do pamiêci KG skojarzonej z uchwytem
    glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return tex;
}