/* ==========================================================================================
 * Project Name  : ZelosEngine
 *
 * File Name     : Shader.h
 *
 * Creation Date : 09/27/2017
 * ==========================================================================================
 * Description   : Largely based on the tutorials found here : https://learnopengl.com/
 *
 * Author        : Mehdi Rouijel
 * ========================================================================================== */

#ifndef _SHADER_H_
#define _SHADER_H_


class ShaderProgram
{
public:
    unsigned int id_;

    ShaderProgram(const char *vShaderPath, const char *fShaderPath);

    void Use();
    void SetBool(const std::string &name, bool value) const;
    void SetInt(const std::string &name, int value) const;
    void SetFloat(const std::string &name, float value) const;
    void SetMat4(const std::string &name, glm::mat4 value) const;
};


#endif // _SHADER_H_
