/* ==========================================================================================
 * Project Name  : ClothSimulation
 * File Name     : Mesh.cpp
 *
 * Creation Date : 09/12/2017 - 07:06
 * Last Modified : 12/12/2017 - 19:48
 * ==========================================================================================
 * Description   : Largely based on the tutorials found here : https://learnopengl.com/
 *
 * Author        : Mehdi Rouijel
 * ========================================================================================== */

#include "glm/glm.hpp"
#include "glad/glad.h"

#include "Shader.h"
#include "Mesh.h"


// PUBLIC METHODS
// --------------

Mesh::Mesh(std::vector<Vertex> vertices,
           std::vector<unsigned int> indices,
           std::vector<Face> faces,
           std::map<unsigned int, std::vector<unsigned int>> neighbors)
{
    Vertices = vertices;
    Indices = indices;
    Neighbors = neighbors;
    Faces = faces;

    std::vector<unsigned int> constraintCount(vertices.size());
    for (auto it = Neighbors.begin(); it != Neighbors.end(); ++it)
    {
        for (auto neighborIt = it->second.begin(); neighborIt != it->second.end(); ++neighborIt)
        {
            DistConstraints.push_back(DistanceConstraint(this, it->first, (*neighborIt), &constraintCount));
        }
    }

    ConstraintCount = constraintCount;

    Initialize();
}

void
Mesh::RecalculateNormals()
{
    // Calculate triangle normal and apply to each vertex of that triangle.
    for (auto it = Faces.begin(); it != Faces.end(); ++it)
    {
        glm::vec3 e0 = (Vertices[it->Indices[0]].Position - Vertices[it->Indices[1]].Position);
        glm::vec3 e1 = (Vertices[it->Indices[0]].Position - Vertices[it->Indices[2]].Position);

        it->Normal = glm::normalize(glm::cross(e0, e1));

        for (unsigned int index = 0; index < 3; ++index)
        {
            Vertices[it->Indices[index]].Normal = it->Normal;
        }
    }

    // The vertex' normal is the average of the normals of each triangle it belongs to.
    for (unsigned int vertexIndex = 0; vertexIndex < Vertices.size(); ++vertexIndex)
    {
        glm::vec3 sum(0.0f, 0.0f, 0.0f);
        unsigned int count = 0;

        for (auto faceIt = Faces.begin(); faceIt != Faces.end(); ++faceIt)
        {
            if ((vertexIndex == faceIt->Indices[0]) ||
                (vertexIndex == faceIt->Indices[0]) ||
                (vertexIndex == faceIt->Indices[0]))
            {
                sum += faceIt->Normal;
                ++count;
            }
        }

        sum.x /= (float)count;
        sum.y /= (float)count;
        sum.z /= (float)count;

        //if (glm::dot(Vertices[vertexIndex].Normal, sum) > 0.0f)
        //{
            Vertices[vertexIndex].Normal = sum;
        //}
        //else
        //{
            //Vertices[vertexIndex].Normal = -sum;
        //}

        Vertices[vertexIndex].Normal = glm::normalize(Vertices[vertexIndex].Normal);
    }
}

void
Mesh::Update(bool updateNormals)
{
    if (updateNormals)
    {
        RecalculateNormals();
    }

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, Vertices.size()*sizeof(Vertex), &Vertices[0], GL_STATIC_DRAW);
    glBindVertexArray(0);
}

void
Mesh::Draw(Shader shader)
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, Indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}



// PRIVATE METHODS
// ---------------

void
Mesh::Initialize()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);


    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, Vertices.size()*sizeof(Vertex), &Vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indices.size()*sizeof(unsigned int), &Indices[0], GL_STATIC_DRAW);

    // TODO(): Be smarter about this.
    // The strides and offsets won't be scaled properly if the Vertex structure is modified.
    // Use macros?

    // layout (location = 0) == Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
    // layout (location = 1) == Normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(3*sizeof(float)));
    // layout (location = 2) == TexCoords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(6*sizeof(float)));
    // layout (location = 3) == Color
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(8*sizeof(float)));
    // layout (location = 4) == Tangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(11*sizeof(float)));
    // layout (location = 5) == Bitangent
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(14*sizeof(float)));

    glBindVertexArray(0);
}
