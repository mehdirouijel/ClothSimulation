/* ==========================================================================================
 * File Name     : Mesh.cpp
 *
 * Creation Date : 09/12/2017 - 07:06
 * Last Modified : 10/12/2017 - 17:43
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
           std::map<unsigned int, std::vector<unsigned int>> neighbors)
{
    Vertices = vertices;
    Indices = indices;
    Neighbors = neighbors;

    for (auto it = Neighbors.begin(); it != Neighbors.end(); ++it)
    {
        for (auto neighborIt = it->second.begin(); neighborIt != it->second.end(); ++neighborIt)
        {
            DistConstraints.push_back(DistanceConstraint(this, it->first, (*neighborIt)));
        }
    }

    Initialize();
}

void
Mesh::Update()
{
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
