/* ==========================================================================================
 * Project Name  : ZelosEngine
 * File Name     : Basic.frag
 *
 * Creation Date : 09/28/2017
 * Last Modified : 03/12/2017 - 04:04
 * ==========================================================================================
 * Description   : FRAGMENT SHADER
 *                 Largely based on the tutorials found here : https://learnopengl.com/
 *
 * Author        : Mehdi Rouijel
 * ========================================================================================== */

#version 330 core

in VertexData
{
    vec3 position;
    vec3 color;
    vec2 texCoords;
} i;

uniform sampler2D Tex1;
uniform sampler2D Tex2;
uniform float MixValue;

out vec4 fragColor;


void main()
{
    fragColor = mix(texture(Tex1, i.texCoords), texture(Tex2, i.texCoords), MixValue);
}
