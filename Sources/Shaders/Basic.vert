/* ==========================================================================================
 * Project Name  : ZelosEngine
 * File Name     : Basic.vert
 *
 * Creation Date : 09/28/2017
 * Last Modified : 06/12/2017 - 02:57
 * ==========================================================================================
 * Description   : VERTEX SHADER
 *                 Largely based on the tutorials found here : https://learnopengl.com/
 *
 * Author        : Mehdi Rouijel
 * ========================================================================================== */

#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 2) in vec2 inTexCoords;

out VertexData
{
    vec4 Position;
    vec2 TexCoords;
};

// TODO: Look into uniform blocks
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;


void main()
{
    Position = Projection * View * Model * vec4(inPosition, 1.0f);
    TexCoords = inTexCoords;

    gl_Position = Position;
}
