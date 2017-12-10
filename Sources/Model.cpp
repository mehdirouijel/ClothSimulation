/* ==========================================================================================
 * File Name     : Model.cpp
 *
 * Creation Date : 09/12/2017 - 08:09
 * Last Modified : 10/12/2017 - 18:57
 * ==========================================================================================
 * Description   : Largely based on the tutorials found here : https://learnopengl.com/
 *
 * Author        : Mehdi Rouijel
 * ========================================================================================== */

#include <iostream>
#include <algorithm>

#include "Model.h"


// PUBLIC METHODS
// --------------

Model::Model(const std::string &path, const glm::vec3 &color)
{
    Color = color;

    LoadModel(path);
}

void
Model::Update()
{
    for (unsigned int index = 0; index < Meshes.size(); ++index)
    {
        Meshes[index].Update();
    }
}

void
Model::Draw(Shader shader)
{
    for (unsigned int index = 0; index < Meshes.size(); ++index)
    {
        Meshes[index].Draw(shader);
    }
}


// PRIVATE METHODS
// ---------------

void
Model::LoadModel(const std::string &path)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path,
        aiProcess_Triangulate|
        aiProcess_JoinIdenticalVertices|
        aiProcess_CalcTangentSpace|
        aiProcess_SortByPType|
        aiProcess_GenSmoothNormals);
    if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }

    Directory = path.substr(0, path.find_last_of('/'));

    ProcessNode(scene, scene->mRootNode);
}

void
Model::ProcessNode(const aiScene *scene, aiNode *node)
{
    for (unsigned int index = 0; index < node->mNumMeshes; ++index)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[index]];
        Meshes.push_back(ProcessMesh(scene, mesh));
    }
    for (unsigned int index = 0; index < node->mNumChildren; ++index)
    {
        ProcessNode(scene, node->mChildren[index]);
    }
}

Mesh
Model::ProcessMesh(const aiScene *scene, aiMesh *mesh)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::map<unsigned int, std::vector<unsigned int>> neighbors;
    float maxY = 0.0f;
    float minX = 9999.9f;
    float maxX = 0.0f;


    for (unsigned int index = 0; index < mesh->mNumVertices; ++index)
    {
        Vertex vertex;

        if (mesh->HasPositions())
        {
            vertex.Position = glm::vec3(mesh->mVertices[index].x,
                                        mesh->mVertices[index].y,
                                        mesh->mVertices[index].z);
        }

        if (mesh->HasNormals())
        {
            vertex.Normal = glm::vec3(mesh->mNormals[index].x,
                                      mesh->mNormals[index].y,
                                      mesh->mNormals[index].z);
        }

        if (mesh->HasTextureCoords(0))
        {
            vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][index].x,
                                         mesh->mTextureCoords[0][index].y);
        }

        if (mesh->HasTangentsAndBitangents())
        {
            vertex.Tangent = glm::vec3(mesh->mTangents[index].x,
                                       mesh->mTangents[index].y,
                                       mesh->mTangents[index].z);

            vertex.Bitangent = glm::vec3(mesh->mBitangents[index].x,
                                         mesh->mBitangents[index].y,
                                         mesh->mBitangents[index].z);
        }

        vertex.Color = Color;

        vertices.push_back(vertex);

        // TODO(): Ad hoc way of selecting vertices to pin in place.
        // Move to something better. Mouse-select vertices?
        if (vertex.Position.y >= maxY)
        {
            maxY = vertex.Position.y;

            if (vertex.Position.x <= minX)
            {
                minX = vertex.Position.x;
                TopLeftIndex = index;
            }
            else if (vertex.Position.x >= maxX)
            {
                maxX = vertex.Position.x;
                TopRightIndex = index;
            }
        }
    }

    for (unsigned int faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex)
    {
        aiFace face = mesh->mFaces[faceIndex];

        for (unsigned int index = 0; index < face.mNumIndices; ++index)
        {
            indices.push_back(face.mIndices[index]);
        }

        // Find neighbors.
        for (unsigned int vertexIndex = 0; vertexIndex < face.mNumIndices; ++vertexIndex)
        {
            auto ret = neighbors.insert(std::pair<unsigned int, std::vector<unsigned int>>(
                    face.mIndices[vertexIndex], std::vector<unsigned int>()));

            // ret will be an iterator pointing to:
            //  - the inserted element, if an equivalent key was not found
            //  - the existing element with the equivalent key

            for (unsigned int neighborIndex = 0; neighborIndex < face.mNumIndices; ++neighborIndex)
            {
                if (face.mIndices[vertexIndex] != face.mIndices[neighborIndex])
                {
                    // We don't want to store the vertex itself as one of its neighbors.

                    auto begin = ret.first->second.begin();
                    auto end = ret.first->second.end();
                    if (std::find(begin, end, face.mIndices[neighborIndex]) == end)
                    {
                        // The neighbor is not stored yet.

                        ret.first->second.push_back(face.mIndices[neighborIndex]);
                    }
                }
            }
        }
    }

    return Mesh(vertices, indices, neighbors);
}

