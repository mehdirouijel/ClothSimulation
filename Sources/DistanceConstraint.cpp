/* ==========================================================================================
 * Project Name  : ClothSimulation
 * File Name     : DistanceConstraint.cpp
 *
 * Creation Date : 10/12/2017 - 16:40
 * Last Modified : 11/12/2017 - 16:51
 * ==========================================================================================
 * Description   :
 *
 * Author        : Mehdi Rouijel
 * ========================================================================================== */

#include "DistanceConstraint.h"
#include "Mesh.h"


DistanceConstraint::DistanceConstraint(Mesh *mesh, unsigned int index1, unsigned int index2)
{
    Vertex1Index = index1;
    Vertex2Index = index2;

    glm::vec3 p1 = mesh->Vertices[index1].Position;
    glm::vec3 p2 = mesh->Vertices[index2].Position;
    RestLength = glm::distance(p1, p2);
}

void
DistanceConstraint::Solve(Mesh *mesh, std::vector<glm::vec3> *tentativePositions)
{
    glm::vec3 p1 = mesh->Vertices[Vertex1Index].Position;
    glm::vec3 p2 = mesh->Vertices[Vertex2Index].Position;
    float w1 = mesh->InvMasses[Vertex2Index];
    float w2 = mesh->InvMasses[Vertex2Index];

    float distance = glm::distance(p1, p2);
    glm::vec3 delta;

    delta = (1.0f/(w1 + w2)) * (distance - RestLength) * glm::normalize(p1 - p2);

    glm::vec3 deltaP1 = -delta;
    glm::vec3 deltaP2 = delta;

    (*tentativePositions)[Vertex1Index] += w1 * deltaP1 * (1.0f / mesh->DistConstraints.size());
    (*tentativePositions)[Vertex2Index] += w2 * deltaP2 * (1.0f / mesh->DistConstraints.size());
}

