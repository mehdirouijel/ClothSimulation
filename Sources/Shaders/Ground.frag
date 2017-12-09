/* ==========================================================================================
 * Project Name  : ZelosEngine
 * File Name     : Ground.frag
 *
 * Creation Date : 09/28/2017
 * Last Modified : 09/12/2017 - 17:29
 * ==========================================================================================
 * Description   : FRAGMENT SHADER
 *                 Largely based on the tutorials found here : https://learnopengl.com/
 *
 * Author        : Mehdi Rouijel
 * ========================================================================================== */

#version 330 core

struct light
{
    vec3 position;
    vec3 direction;
    vec3 color;

    float cutOff;
    float outerCutOff;

    vec3 ambientIntensity;
    vec3 diffuseIntensity;
    vec3 specularIntensity;

    float constantAtt;
    float linearAtt;
    float quadraticAtt;
};

in VertexData
{
    vec4 Position;
    vec3 Normal;
    vec2 TexCoords;
    vec4 Color;
};

uniform vec3 CameraPosition;
uniform sampler2D NoiseTexture;
uniform sampler2D CheckeredTexture;
uniform float Shininess;
uniform light Light;

out vec4 FragColor;


void main()
{
    vec3 lightDirection = normalize(Light.position - Position.xyz);
    vec3 normal = normalize(Normal);
    vec3 finalColor;


    // Ambient Component
    // -----------------

    vec3 ambient = Light.ambientIntensity * texture(CheckeredTexture, TexCoords).stp;


    // Diffuse Component
    // -----------------

    float diffuseCoeff = max(dot(normal, lightDirection), 0.0f);
    vec3 diffuse = Light.diffuseIntensity * diffuseCoeff * texture(CheckeredTexture, TexCoords).stp;


    // Specular Component
    // ------------------

    vec3 cameraDirection = normalize(CameraPosition - Position.xyz);
    vec3 reflectDirection = reflect(-lightDirection, normal);

    float specularCoeff = pow(max(dot(cameraDirection, reflectDirection), 0.0f), Shininess);
    vec3 specular = Light.specularIntensity * specularCoeff * texture(CheckeredTexture, TexCoords).stp;


    // Spotlight Effect
    // ----------------

    float theta = dot(lightDirection, normalize(-Light.direction));
    float epsilon = (Light.cutOff - Light.outerCutOff);
    float intensity = clamp(((theta - Light.outerCutOff) / epsilon), 0.0f, 1.0f);

    diffuse *= intensity;
    specular *= intensity;


    // Attenuation
    // -----------

    float d = length(Light.position - Position.xyz);
    float attenuation = 1.0f / (Light.constantAtt + Light.linearAtt*d + Light.quadraticAtt*(d*d));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    

    finalColor = ambient + diffuse + specular;

    // Dithering
    // ---------
    // (to limit color banding; c.f.: https://www.shadertoy.com/view/MlV3R1)

    float noise = texture(NoiseTexture, 2.0f*TexCoords).r;
    finalColor += mix(-0.5f/128.0f, 0.5f/128.0f, noise);


    FragColor = vec4(finalColor, 1.0f);
}

