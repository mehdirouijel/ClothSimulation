/* ==========================================================================================
 * Project Name  : ZelosEngine
 * File Name     : ZelosEngine.cpp
 *
 * Creation Date : 09/24/2017
 * Last Modified : 06/12/2017 - 08:18
 * ==========================================================================================
 * Description   : Largely based on the tutorials found here : https://learnopengl.com/
 *
 * Author        : Mehdi Rouijel
 * ========================================================================================== */

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "glm/gtc/matrix_transform.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <iostream>
#include "ShaderProgram.h"


// CONSTANTS AND GLOBALS
// ---------------------

const unsigned int SCREEN_WIDTH = 800;
const unsigned int SCREEN_HEIGHT = 600;

const float CAMERA_SPEED = 2.0f;


float lastTime = 0.0f;
float deltaTime = 0.0f;

glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, cameraUp));
float cameraSpeed = 0.0f;

float mixValue = 0.2f;


// PROTOTYPES
// ----------

void FramebufferSizeCallback(GLFWwindow *window, int width, int height);
void ProcessInput(GLFWwindow *window);


// ENTRY POINT
// -----------

int
main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Zelos Engine", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    //
    // CPU DATA
    // --------

    GLfloat vertices[] = {
        //  positions    //  // tex  //
       -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
       -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
       -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

       -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
       -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
       -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

       -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
       -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
       -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
       -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
       -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
       -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

       -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
       -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
       -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

       -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
       -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
       -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;


    //
    // GPU DATA
    // --------
    //   Set up shaders
    //   --------------

    ShaderProgram shader("../Sources/Shaders/Basic.vert", "../Sources/Shaders/Basic.frag");


    //   Set up buffers
    //   --------------

    GLuint vao;
    GLuint vbo;
    //GLuint ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    //glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position: layout(location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // color: layout(location = 1)
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void *)(3*sizeof(float)));
    //glEnableVertexAttribArray(1);

    // texture coordinates: layout(location = 2)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void *)(3*sizeof(float)));
    glEnableVertexAttribArray(2);

    // The VAO stores the EBO unbind call, so don't unbind the latter
    // before the former, or else.
    // Also, unbinding the VBO and EBO isn't necessary at all in
    // the first place...
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    //
    //glBindVertexArray(0);
    //
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glEnable(GL_DEPTH_TEST);


    //   Textures
    //   --------

    GLuint texture1;
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width;
    int height;
    int channelCount;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = (unsigned char *)stbi_load("../Assets/container.jpg", &width, &height, &channelCount, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture 1" << std::endl;
    }
    stbi_image_free(data);

    GLuint texture2;
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    data = (unsigned char *)stbi_load("../Assets/awesomeface.png", &width, &height, &channelCount, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture 2" << std::endl;
    }
    stbi_image_free(data);


    //   Uniforms
    //   --------

    shader.Use(); // Activate shader to set uniforms

    shader.SetInt("Tex1", 0); // Set uniforms
    shader.SetInt("Tex2", 1); // Set uniforms


    //
    // RENDER LOOP
    // -----------

    while (!glfwWindowShouldClose(window))
    {
        // Timing.
        float currentTime = (float)glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        cameraSpeed = CAMERA_SPEED * deltaTime;

        // Process input.
        ProcessInput(window);

        // Render.
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);


        projection = glm::perspective(glm::radians(75.0f), (float)(SCREEN_WIDTH/SCREEN_HEIGHT), 0.1f, 100.0f);
        //view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));

        view = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);


        shader.Use();
        shader.SetMat4("View", view);
        shader.SetMat4("Projection", projection);
        shader.SetFloat("MixValue", mixValue);


        glBindVertexArray(vao);
        for (unsigned int i = 0; i < 10; ++i)
        {
            model = glm::translate(glm::mat4(1.0f), cubePositions[i]);
            float angle = 36.0f * i;

            if (i % 2 == 0)
            {
                model = glm::rotate(model, (float)(glfwGetTime()*glm::radians(0.5f*angle)), glm::vec3(1.0f, 0.3f, 0.6f));
            }
            else
            {
                model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.6f));
            }

            shader.SetMat4("Model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // Update events and swap buffers.
        glfwPollEvents();
        glfwSwapBuffers(window);
    }


    // Not really necessary if we quit the program right then
    // and there, but I like to be thorough and explicit.
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    //glDeleteBuffers(1, &ebo);

    glfwTerminate();
    return 0;
}


// FUNCTIONS
// ---------

void
ProcessInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
    else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        cameraPosition += cameraFront * cameraSpeed;
    }
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        cameraPosition -= cameraFront * cameraSpeed;
    }
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        cameraPosition -= cameraRight * cameraSpeed;
    }
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        cameraPosition += cameraRight * cameraSpeed;
    }
    else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        mixValue += 0.005f;
        if (mixValue > 1.0f)
        {
            mixValue = 1.0f;
        }
    }
    else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        mixValue -= 0.005f;
        if (mixValue < 0.0f)
        {
            mixValue = 0.0f;
        }
    }
}


// CALLBACKS
// ---------

void
FramebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}
