#include "ObjectLoader.h"
#pragma warning(disable:4996) // _CRT_SECURE_NO_WARNINGS

bool ObjectLoader::loadOBJ(const char* path,
    Model& out_model) {

    std::vector< uint32_t > vertexIndices, uvIndices, normalIndices;
    std::vector< glm::vec3 > temp_vertices;
    std::vector< glm::vec2 > temp_uvs;
    std::vector< glm::vec3 > temp_normals;

    std::string Path = path;

    std::string FinalPath = "models/" + Path;


    FILE* file = fopen(FinalPath.c_str(), "r");

    if (file == NULL) {
        printf("Impossible to open the file !\n");
        return false;
    }

    while (1) {

        char lineHeader[256] = { 0 };

        int res = fscanf(file, "%s", lineHeader);

        if(res == EOF)
            break;

        if (strcmp(lineHeader, "v") == 0) { // Vertex
            glm::vec3 vertex{};
            if (!fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z)) break;
             temp_vertices.push_back(vertex);
        } else if (strcmp(lineHeader, "vt") == 0) { // Vertex Texture
            glm::vec2 uv{};
            if (!fscanf(file, "%f %f\n", &uv.x, &uv.y)) break;
            temp_uvs.push_back(uv);
        } else if (strcmp(lineHeader, "vn") == 0) { // Vertex Normal
            glm::vec3 normal{};
            if (!fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z)) break;
            temp_normals.push_back(normal);
        } else if (strcmp(lineHeader, "f") == 0) {
            std::string vertex1, vertex2, vertex3;
            unsigned int vertexIndex[3]{}, uvIndex[3]{}, normalIndex[3]{};
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
            if (matches != 9) {
                printf("File can't be read by our simple parser : ( Try exporting with other options\n");
                return false;
            }
            vertexIndices.push_back(vertexIndex[0]);
            vertexIndices.push_back(vertexIndex[1]);
            vertexIndices.push_back(vertexIndex[2]);
            uvIndices.push_back(uvIndex[0]);
            uvIndices.push_back(uvIndex[1]);
            uvIndices.push_back(uvIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
        }
    }

    for (uint32_t i = 0; i < vertexIndices.size(); i++) {
        uint32_t vertexIndex = vertexIndices[i] - 1;
        uint32_t uvIndex = uvIndices[i] - 1;
        uint32_t normalIndex = normalIndices[i] - 1;

        glm::vec3 vertex = temp_vertices[vertexIndex];
        glm::vec2 uv = temp_uvs[uvIndex];
        glm::vec3 normal = temp_normals[normalIndex];

        out_model.vertices.push_back(vertex);
        out_model.uvs.push_back(uv);
        out_model.normals.push_back(normal);
    }
    out_model.id = std::string(path);
    fclose(file);
    return true;
}