/* ==========================================================================================
 * Project Name  : ClothSimulation
 * File Name     : DistanceConstraint.cpp
 *
 * Creation Date : 10/12/2017 - 16:40
 * Last Modified : 12/12/2017 - 20:02
 * ==========================================================================================
 * Description   :
 *
 * Author        : Mehdi Rouijel
 * ========================================================================================== */

#include <limits>
#include "DistanceConstraint.h"
#include "Mesh.h"


DistanceConstraint::DistanceConstraint(Mesh *mesh, unsigned int index1, unsigned int index2, std::vector<unsigned int> *constraintCount)
{
    Vertex1Index = index1;
    Vertex2Index = index2;

    glm::vec3 p1 = mesh->Vertices[index1].Position;
    glm::vec3 p2 = mesh->Vertices[index2].Position;
    RestLength = glm::distance(p1, p2);

    ++(*constraintCount)[index1];
    ++(*constraintCount)[index2];
}

void
DistanceConstraint::Solve(Mesh *mesh, std::vector<glm::vec3> *deltaPositions)
{
    glm::vec3 p1 = mesh->Vertices[Vertex1Index].Position;
    glm::vec3 p2 = mesh->Vertices[Vertex2Index].Position;
    float w1 = mesh->InvMasses[Vertex2Index];
    float w2 = mesh->InvMasses[Vertex2Index];

    // "Hack to avoid division by 0"
    if (w1 == std::numeric_limits<float>::infinity())
    {
        w1 = 0.0f;
    }
    if (w2 == std::numeric_limits<float>::infinity())
    {
        w2 = 0.0;
    }
    float sum = ((w1 + w2 == 0.0f) ? 0.000001f : (w1 + w2));

    float distance = glm::distance(p1, p2);
    glm::vec3 delta;
    float stiffness;

    // This should damp the elasticity and vertices jumping around.
    if (distance >= RestLength)
    {
        stiffness = 0.05f;
    }
    else
    {
        stiffness = 0.0f;
    }

    delta = stiffness * 1.0f / sum * (distance - RestLength) * glm::normalize(p1 - p2);

    glm::vec3 deltaP1 = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 deltaP2 = glm::vec3(0.0f, 0.0f, 0.0f);
    if (delta != glm::vec3(0.0f, 0.0f, 0.0f))
    {
        deltaP1 = -delta;
        deltaP2 = delta;
    }

    (*deltaPositions)[Vertex1Index] += w1 * deltaP1;
    (*deltaPositions)[Vertex2Index] += w2 * deltaP2;
}

