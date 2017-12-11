#ifndef _MESH_H_
#define _MESH_H_

/* ==========================================================================================
 * Project Name  : ClothSimulation
 * File Name     : Mesh.h
 *
 * Creation Date : 09/12/2017 - 06:58
 * Last Modified : 11/12/2017 - 16:51
 * ==========================================================================================
 * Description   : Largely based on the tutorials found here : https://learnopengl.com/
 *
 * Author        : Mehdi Rouijel
 * ========================================================================================== */

#include <vector>
#include <map>
#include "glm/glm.hpp"

#include "DistanceConstraint.h"


// IMPORTANT(): Careful changing this struct!
// It would mess up the strides and offsets in Mesh::Initialize().
struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Color;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
};

class Shader;


class Mesh
{

public:
    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;
    std::map<unsigned int, std::vector<unsigned int>> Neighbors;
    std::vector<glm::vec3> Velocities;
    std::vector<float> Masses;
    std::vector<float> InvMasses;
    std::vector<DistanceConstraint> DistConstraints;

    unsigned int VAO;

    Mesh(std::vector<Vertex> vertices,
         std::vector<unsigned int> indices,
         std::map<unsigned int, std::vector<unsigned int>> neighbors);

    void FindNeighbors();
    void Update();
    void Draw(Shader shader);


private:
    unsigned int VBO;
    unsigned int EBO;

    void Initialize();

};


#endif // _MESH_H_

