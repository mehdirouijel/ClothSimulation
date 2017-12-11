/* ==========================================================================================
 * Project Name  : ClothSimulation
 * File Name     : Basic.vert
 *
 * Creation Date : 09/28/2017
 * Last Modified : 11/12/2017 - 16:51
 * ==========================================================================================
 * Description   : VERTEX SHADER
 *                 Largely based on the tutorials found here : https://learnopengl.com/
 *
 * Author        : Mehdi Rouijel
 * ========================================================================================== */

#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoords;
layout (location = 3) in vec3 inColor;

out VertexData
{
    vec4 Position;
    vec3 Normal;
    vec2 TexCoords;
    vec4 Color;
};

// TODO(): Look into uniform blocks
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform mat4 NormalMatrix;


void main()
{
    Position = Model * vec4(inPosition, 1.0f);
    Normal = mat3(NormalMatrix) * inNormal;
    TexCoords = inTexCoords;
    Color = vec4(inColor, 1.0f);

    gl_Position = Projection * View * Position;
}
