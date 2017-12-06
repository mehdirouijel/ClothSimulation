/* ==========================================================================================
 * Project Name  : ZelosEngine
 * File Name     : Basic.frag
 *
 * Creation Date : 09/28/2017
 * Last Modified : 06/12/2017 - 02:57
 * ==========================================================================================
 * Description   : FRAGMENT SHADER
 *                 Largely based on the tutorials found here : https://learnopengl.com/
 *
 * Author        : Mehdi Rouijel
 * ========================================================================================== */

#version 330 core

in VertexData
{
    vec4 Position;
    vec2 TexCoords;
};

uniform sampler2D Tex1;
uniform sampler2D Tex2;
uniform float MixValue;

out vec4 FragColor;


void main()
{
    FragColor = mix(texture(Tex1, TexCoords), texture(Tex2, TexCoords), MixValue);
}
