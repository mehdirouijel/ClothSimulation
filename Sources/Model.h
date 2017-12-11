#ifndef _MODEL_H_
#define _MODEL_H_

/* ==========================================================================================
 * Project Name  : ClothSimulation
 * File Name     : Model.h
 *
 * Creation Date : 09/12/2017 - 08:08
 * Last Modified : 11/12/2017 - 16:51
 * ==========================================================================================
 * Description   : Largely based on the tutorials found here : https://learnopengl.com/
 *
 * Author        : Mehdi Rouijel
 * ========================================================================================== */

#include <vector>
#include <string>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Shader.h"
#include "Mesh.h"


class Model
{

public:
    std::vector<Mesh> Meshes;
    glm::vec3 Color;
    std::string Directory;
    unsigned int TopLeftIndex;
    unsigned int TopRightIndex;
    std::vector<unsigned int> TopRow;

    Model(const std::string &path, const glm::vec3 &color = glm::vec3(0.5f, 0.5f, 0.5f));

    void Update();
    void Draw(Shader shader);


private:
    void LoadModel(const std::string &path);
    void ProcessNode(const aiScene *scene, aiNode *node);
    Mesh ProcessMesh(const aiScene *scene, aiMesh *mesh);

};


#endif // _MODEL_H_

