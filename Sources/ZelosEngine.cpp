/* ==========================================================================================
 * Project Name  : ZelosEngine
 * File Name     : ZelosEngine.cpp
 *
 * Creation Date : 09/24/2017
 * Last Modified : 10/12/2017 - 19:10
 * ==========================================================================================
 * Description   : Largely based on the tutorials found here : https://learnopengl.com/
 *
 * Author        : Mehdi Rouijel
 * ========================================================================================== */

#include <iostream>
#include <vector>
#include <string>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "glm/gtc/matrix_transform.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Shader.h"
#include "Camera.h"
#include "Model.h"


// CONSTANTS AND GLOBALS
// ---------------------

const unsigned int SCREEN_WIDTH = 1280;
const unsigned int SCREEN_HEIGHT = 720;

const glm::vec3 WORLD_UP = glm::vec3(0.0f, 1.0f, 0.0f);

const unsigned int SOLVER_ITERATIONS = 30;


float LastTime = 0.0f;
float DeltaTime = 0.0f;

Camera camera(glm::vec3(0.0f, 5.0f, 10.0f));
float LastCameraX = 0.5f * SCREEN_WIDTH;
float LastCameraY = 0.5f * SCREEN_HEIGHT;
bool FirstMouseInput = true;
bool CursorEnabled = true;
bool TabWasPressed = false;
bool TabIsPressed = false;


// PROTOTYPES
// ----------

void ProcessInput(GLFWwindow *window);

void FramebufferSizeCallback(GLFWwindow *window, int width, int height);
void MouseCallback(GLFWwindow *window, double xPosition, double yPosition);


struct Light
{
    glm::vec3 Position;
    glm::vec3 Direction;
    glm::vec3 Color;

    float CutOff;
    float OuterCutOff;

    glm::vec3 AmbientIntensity;
    glm::vec3 DiffuseIntensity;
    glm::vec3 SpecularIntensity;

    float ConstantAtt;
    float LinearAtt;
    float QuadraticAtt;
};


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
    glfwSetCursorPosCallback(window, MouseCallback);


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    //
    // CPU DATA
    // --------

    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
    glm::mat4 normalMatrix;

    Light light;
    light.Position = glm::vec3(10.0f, 12.0f, 10.0f);
    light.Direction = glm::normalize(-light.Position);
    light.Color = glm::vec3(0.9f, 0.9f, 0.8f);
    light.CutOff = glm::cos(glm::radians(18.0f));
    light.OuterCutOff = glm::cos(glm::radians(23.0f));
    light.AmbientIntensity = glm::vec3(0.2f, 0.2f, 0.2f);
    light.DiffuseIntensity = glm::vec3(0.8f, 0.8f, 0.8f);
    light.SpecularIntensity = glm::vec3(1.0f, 1.0f, 1.0f);
    // Attenuation values from: http://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation
    light.ConstantAtt = 1.0f;
    light.LinearAtt = 0.027f;
    light.QuadraticAtt = 0.0028f;


    //   Set up shaders
    //   --------------

    Shader SHDR_basic("../Sources/Shaders/Basic.vert", "../Sources/Shaders/Basic.frag");
    Shader SHDR_ground("../Sources/Shaders/Basic.vert", "../Sources/Shaders/Ground.frag");


    //   Load models
    //   -----------

    Model ground("../Assets/groundPlane.obj", glm::vec3(0.75f, 0.75f, 0.8f));
    //Model pole("../Assets/pole.obj", glm::vec3(0.8f, 0.8f, 0.8f));
    Model cloth("../Assets/cloth.obj", glm::vec3(0.1f, 0.5f, 0.6f));


    //
    // GPU DATA
    // --------

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);


    //   Textures
    //   --------

    GLuint noiseTexture;
    glGenTextures(1, &noiseTexture);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width;
    int height;
    int channelCount;
    unsigned char *data = (unsigned char *)stbi_load("../Assets/white_noise.png", &width, &height, &channelCount, STBI_rgb_alpha);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);


    GLuint checkeredTexture;
    glGenTextures(1, &checkeredTexture);
    glBindTexture(GL_TEXTURE_2D, checkeredTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    data = (unsigned char *)stbi_load("../Assets/checkered.png", &width, &height, &channelCount, STBI_rgb_alpha);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);


    //   Uniforms
    //   --------

    SHDR_basic.Use(); // Activate shader to set uniforms
    SHDR_basic.SetInt("NoiseTexture", 0); // Set uniforms
    SHDR_basic.SetVec3("CameraPosition", camera.Position);
    SHDR_basic.SetVec3("Light.position", light.Position);
    SHDR_basic.SetVec3("Light.direction", light.Direction);
    SHDR_basic.SetVec3("Light.color", light.Color);
    SHDR_basic.SetFloat("Light.cutOff", light.CutOff);
    SHDR_basic.SetFloat("Light.outerCutOff", light.OuterCutOff);
    SHDR_basic.SetVec3("Light.ambientIntensity", light.AmbientIntensity);
    SHDR_basic.SetVec3("Light.diffuseIntensity", light.DiffuseIntensity);
    SHDR_basic.SetVec3("Light.specularIntensity", light.SpecularIntensity);
    SHDR_basic.SetFloat("Light.constantAtt", light.ConstantAtt);
    SHDR_basic.SetFloat("Light.linearAtt", light.LinearAtt);
    SHDR_basic.SetFloat("Light.quadraticAtt", light.QuadraticAtt);
    SHDR_basic.SetFloat("Shininess", 32.0f);

    SHDR_ground.Use();
    SHDR_ground.SetInt("CheckeredTexture", 1); // Set uniforms
    SHDR_ground.SetVec3("CameraPosition", camera.Position);
    SHDR_ground.SetVec3("Light.position", light.Position);
    SHDR_ground.SetVec3("Light.direction", light.Direction);
    SHDR_ground.SetVec3("Light.color", light.Color);
    SHDR_ground.SetFloat("Light.cutOff", light.CutOff);
    SHDR_ground.SetFloat("Light.outerCutOff", light.OuterCutOff);
    SHDR_ground.SetVec3("Light.ambientIntensity", light.AmbientIntensity);
    SHDR_ground.SetVec3("Light.diffuseIntensity", light.DiffuseIntensity);
    SHDR_ground.SetVec3("Light.specularIntensity", light.SpecularIntensity);
    SHDR_ground.SetFloat("Light.constantAtt", light.ConstantAtt);
    SHDR_ground.SetFloat("Light.linearAtt", light.LinearAtt);
    SHDR_ground.SetFloat("Light.quadraticAtt", light.QuadraticAtt);
    SHDR_ground.SetFloat("Shininess", 2.0f);


    std::vector<glm::vec3> tentativePositions(cloth.Meshes[0].Vertices.size());
    std::vector<glm::vec3> deltaPositions(cloth.Meshes[0].Vertices.size());
    std::vector<unsigned int> neighborsIndices;

    Mesh *mesh = &cloth.Meshes[0];

    for (unsigned int index = 0; index < mesh->Vertices.size(); ++index)
    {
        float mass = 0.5f;
        mesh->Masses.push_back(mass);
        mesh->InvMasses.push_back(1.0f / mesh->Masses[index]);
        mesh->Velocities.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    }

    //
    // RENDER LOOP
    // -----------

    while (!glfwWindowShouldClose(window))
    {
        // Timing.
        float currentTime = (float)glfwGetTime();
        DeltaTime = currentTime - LastTime;
        LastTime = currentTime;

        // Process input.
        ProcessInput(window);
        //mesh->Vertices[cloth.TopLeftIndex].Position.x += 0.001f * glm::cos(DeltaTime);

        // Render.
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, noiseTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, checkeredTexture);


        projection = glm::perspective(glm::radians(camera.FOV), (float)SCREEN_WIDTH/(float)SCREEN_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();

        SHDR_ground.Use();
        SHDR_ground.SetMat4("Projection", projection);
        SHDR_ground.SetMat4("View", view);
        model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -4.0f, 0.0f));
        normalMatrix = glm::transpose(glm::inverse(model));
        SHDR_ground.SetMat4("Model", model);
        SHDR_ground.SetMat4("NormalMatrix", normalMatrix);
        ground.Draw(SHDR_ground);


        for (unsigned int index = 0; index < mesh->Vertices.size(); ++index)
        {
            if ((index != cloth.TopLeftIndex) && (index != cloth.TopRightIndex))
            {
                mesh->Velocities[index] += mesh->Masses[index] * glm::vec3(0.0f, -20.0f, 0.0f) * DeltaTime * DeltaTime;
                tentativePositions[index] = mesh->Vertices[index].Position + mesh->Velocities[index] * DeltaTime;
            }
        }

        // c.f. Unified Particle Physics paper Algorithm 3
        // Simple distance constraint. c.f. PBD paper 3.3
        for (unsigned int index = 0; index < SOLVER_ITERATIONS; ++index)
        {
            for (auto constraintIt = mesh->DistConstraints.begin();
                 constraintIt != mesh->DistConstraints.end();
                 ++constraintIt)
            {
                constraintIt->Solve(mesh, &tentativePositions);
            }
        }

        // Update velocities.
        for (unsigned int index = 0; index < mesh->Vertices.size(); ++index)
        {
            if ((index != cloth.TopLeftIndex) && (index != cloth.TopRightIndex))
            {
                mesh->Velocities[index] = (tentativePositions[index] - mesh->Vertices[index].Position) / DeltaTime;
            }
        }

        // Update positions.
        for (unsigned int index = 0; index < mesh->Vertices.size(); ++index)
        {
            if ((index != cloth.TopLeftIndex) && (index != cloth.TopRightIndex))
            {
                mesh->Vertices[index].Position = tentativePositions[index];
            }
        }

        cloth.Update();


        SHDR_basic.Use();
        SHDR_basic.SetMat4("Projection", projection);
        SHDR_basic.SetMat4("View", view);

        //model = glm::translate(glm::mat4(1.0f), glm::vec3(-4.0f, 0.0f, 4.0f));
        //normalMatrix = glm::transpose(glm::inverse(model));
        //SHDR_basic.SetMat4("Model", model);
        //SHDR_basic.SetMat4("NormalMatrix", normalMatrix);
        //pole.Draw(SHDR_basic);

        model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.95f, 4.0f));
        normalMatrix = glm::transpose(glm::inverse(model));
        SHDR_basic.SetMat4("Model", model);
        SHDR_basic.SetMat4("NormalMatrix", normalMatrix);
        cloth.Draw(SHDR_basic);


        // Update events and swap buffers.
        glfwPollEvents();
        glfwSwapBuffers(window);
    }


    // Not really necessary if we quit the program right then
    // and there, but I like to be thorough and explicit.
    //glDeleteVertexArrays(1, &vao);
    //glDeleteBuffers(1, &vbo);
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
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
    {
        TabIsPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE)
    {
        TabIsPressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(FORWARD, DeltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(BACKWARD, DeltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(LEFT, DeltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(RIGHT, DeltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(UP, DeltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(DOWN, DeltaTime);
    }

    if (TabIsPressed && !TabWasPressed)
    {
        if (CursorEnabled)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            CursorEnabled = false;
        }
        else
        {
            FirstMouseInput = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            CursorEnabled = true;
        }
    }

    TabWasPressed = TabIsPressed;
}


// CALLBACKS
// ---------

void
FramebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void
MouseCallback(GLFWwindow *window, double xPosition, double yPosition)
{
    if (!CursorEnabled)
    {
        if (FirstMouseInput)
        {
            LastCameraX = (float)xPosition;
            LastCameraY = (float)yPosition;
            FirstMouseInput = false;
        }

        float xOffset = (float)xPosition - LastCameraX;
        float yOffset = (float)yPosition - LastCameraY;
        LastCameraX = (float)xPosition;
        LastCameraY = (float)yPosition;

        camera.ProcessMouse(xOffset, yOffset);
    }
}

