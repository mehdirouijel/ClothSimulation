/* ==========================================================================================
 * Project Name  : ClothSimulation
 * File Name     : ClothSimulation.cpp
 *
 * Creation Date : 09/24/2017
 * Last Modified : 12/12/2017 - 20:09
 * ==========================================================================================
 * Description   : Largely based on the tutorials found here : https://learnopengl.com/
 *                 Other references used:
 *                 (1) https://www.youtube.com/watch?v=b2An7OJJ-j0
 *                     (lecture on Position-Based Dynamics from the University of Utah)
 *
 *                 (2) https://github.com/jnnyt93/senior_project
 *                     (student project from the class (1))
 *
 *                 (3) https://github.com/InteractiveComputerGraphics/PositionBasedDynamics
 *                     (Position-Based Dynamics library)
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

const unsigned int SOLVER_ITERATIONS = 5;


float LastTime = 0.0f;
float DeltaTime = 0.0f;

Camera camera(glm::vec3(0.0f, 5.0f, 10.0f));
float LastCameraX = 0.5f * SCREEN_WIDTH;
float LastCameraY = 0.5f * SCREEN_HEIGHT;
bool FirstMouseInput = true;
bool CursorEnabled = true;
bool TabWasPressed = false;
bool TabIsPressed = false;
bool WireframeMode = false;
bool FWasPressed = false;
bool FIsPressed = false;
bool SimulationRunning = false;
bool SpaceWasPressed = false;
bool SpaceIsPressed = false;
bool UpdateNormals = true;


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
    float lightCutOffAngle = 18.0f;

    Light light;
    light.Position = glm::vec3(10.0f, 12.0f, 10.0f);
    light.Direction = glm::normalize(-light.Position);
    light.Color = glm::vec3(0.9f, 0.9f, 0.8f);
    light.CutOff = glm::cos(glm::radians(lightCutOffAngle));
    light.OuterCutOff = glm::cos(glm::radians(lightCutOffAngle + 5.0f));
    light.AmbientIntensity = glm::vec3(0.1f, 0.1f, 0.1f);
    light.DiffuseIntensity = glm::vec3(0.7f, 0.7f, 0.7f);
    light.SpecularIntensity = glm::vec3(0.2f, 0.2f, 0.2f);
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
    Model cloth("../Assets/cloth.obj", glm::vec3(0.1f, 0.5f, 0.6f));


    //
    // GPU DATA
    // --------
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
    SHDR_basic.SetInt("NoiseTexture", 0);
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
    SHDR_basic.SetInt("NoiseTexture", 0);
    SHDR_ground.SetInt("CheckeredTexture", 1);
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
        float mass;

        // Find closest fixed vertex
        float minDist = 99999.9f;
        unsigned int closestFixed;
        for (unsigned int closest = 0; closest < cloth.TopRow.size(); ++closest)
        {
            float dist = glm::distance(mesh->Vertices[cloth.TopRow[closest]].Position, mesh->Vertices[index].Position);
            if (dist < minDist)
            {
                minDist = dist;
                closestFixed = closest;
            }
        }

        // Mass relative to distance from closest fixed vertex.
        // minDist will be 0 for fixed vertices => infinite inverse mass => they won't move.
        mass = 50.0f / minDist;

        mesh->Masses.push_back(mass);
        mesh->InvMasses.push_back(1.0f / mesh->Masses[index]);
        mesh->Velocities.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    }


    //
    // RENDER LOOP
    // -----------

    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window))
    {
        // Timing.
        float currentTime = (float)glfwGetTime();
        DeltaTime = currentTime - LastTime;
        LastTime = currentTime;

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // Process input.
        ProcessInput(window);


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


        //
        // SIMULATION
        // ----------
        //

        if (SimulationRunning)
        {
            std::fill(tentativePositions.begin(), tentativePositions.end(), glm::vec3(0.0f, 0.0f, 0.0f));
            std::fill(deltaPositions.begin(), deltaPositions.end(), glm::vec3(0.0f, 0.0f, 0.0f));

            for (unsigned int index = 0; index < mesh->Vertices.size(); ++index)
            {
                if (std::find(cloth.TopRow.begin(), cloth.TopRow.end(), index) == cloth.TopRow.end())
                {
                    mesh->Velocities[index] += mesh->InvMasses[index] * glm::vec3(0.0f, -50.0f, 0.0f) * DeltaTime;
                    tentativePositions[index] = mesh->Vertices[index].Position + mesh->Velocities[index] * DeltaTime;
                }
            }

            // TODO(): Add collision detection.
            //  (1) Find particles (any object) in a given radius
            //  (2) Solve for collision

            // TODO(): Add bending constraint.

            // c.f. Unified Particle Physics paper Algorithm 3
            for (unsigned int index = 0; index < SOLVER_ITERATIONS; ++index)
            {
                for (auto constraintIt = mesh->DistConstraints.begin();
                     constraintIt != mesh->DistConstraints.end();
                     ++constraintIt)
                {
                    constraintIt->Solve(mesh, &deltaPositions);
                }

                // Over-relaxation
                for (unsigned int i = 0; i < mesh->Vertices.size(); ++i)
                {
                    tentativePositions[i] += 1.0f/(float)mesh->ConstraintCount[i] * deltaPositions[i];
                }
            }

            // Update velocities.
            for (unsigned int index = 0; index < mesh->Vertices.size(); ++index)
            {
                if (std::find(cloth.TopRow.begin(), cloth.TopRow.end(), index) == cloth.TopRow.end())
                {
                    mesh->Velocities[index] = (tentativePositions[index] - mesh->Vertices[index].Position) * 1.0f / DeltaTime;
                }
            }

            // Update positions.
            for (unsigned int index = 0; index < mesh->Vertices.size(); ++index)
            {
                if (std::find(cloth.TopRow.begin(), cloth.TopRow.end(), index) == cloth.TopRow.end())
                {
                    if (glm::length(mesh->Velocities[index]) > 0.001f)
                    {
                        mesh->Vertices[index].Position = tentativePositions[index];
                    }
                }
            }
        }


        //
        // RENDERING SIMULATION RESULT
        // ---------------------------
        //

        if (WireframeMode)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        cloth.Update(UpdateNormals);


        SHDR_basic.Use();
        SHDR_basic.SetMat4("Projection", projection);
        SHDR_basic.SetMat4("View", view);
        model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.95f, 4.0f));
        normalMatrix = glm::transpose(glm::inverse(model));
        SHDR_basic.SetMat4("Model", model);
        SHDR_basic.SetMat4("NormalMatrix", normalMatrix);
        cloth.Draw(SHDR_basic);


        // Update events and swap buffers.
        glfwPollEvents();
        glfwSwapBuffers(window);
    }


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
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    {
        FIsPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE)
    {
        FIsPressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        SpaceIsPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
    {
        SpaceIsPressed = false;
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

    if (FIsPressed && !FWasPressed)
    {
        if (WireframeMode)
        {
            WireframeMode = false;
        }
        else
        {
            WireframeMode = true;
        }
    }

    if (SpaceIsPressed && !SpaceWasPressed)
    {
        if (SimulationRunning)
        {
            SimulationRunning = false;
        }
        else
        {
            SimulationRunning = true;
        }
    }

    TabWasPressed = TabIsPressed;
    FWasPressed = FIsPressed;
    SpaceWasPressed = SpaceIsPressed;
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

