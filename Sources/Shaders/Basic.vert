/* ==========================================================================================
 * Project Name  : ZelosEngine
 * File Name     : Basic.vert
 *
 * Creation Date : 09/28/2017
 * Last Modified : 03/12/2017 - 05:48
 * ==========================================================================================
 * Description   : VERTEX SHADER
 *                 Largely based on the tutorials found here : https://learnopengl.com/
 *
 * Author        : Mehdi Rouijel
 * ========================================================================================== */

#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec2 inTexCoords;

out VertexData
{
    vec3 position;
    vec3 color;
    vec2 texCoords;
} o;

uniform mat4 Transform;


void main()
{
    o.position = vec3((inPosition.x), inPosition.y, inPosition.z);
    o.color = inColor;
    o.texCoords = inTexCoords;

    gl_Position = Transform * vec4(o.position, 1.0f);
}
