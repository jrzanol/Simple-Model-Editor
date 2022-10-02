// by jrzanol
//

#include "stdafx.h"
#include "CReadObj.h"

bool CReadObj::Read(const char* file)
{
    FILE* in = fopen(file, "rt");
    if (in == NULL)
    {
        printf("Fail reading obj file.\n");
        return false;
    }

    char line[128];
    char lineHeader[32];
    char* lineOptions;

    while (fgets(line, sizeof(line), in))
    {
        if (sscanf(line, "%s", lineHeader) != 1)
            continue;

        lineOptions = &line[2];

        glm::vec2 uv;
        glm::vec3 vertex;

        if (strcmp(lineHeader, "v") == 0)
        {
            if (sscanf(lineOptions, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z) == 3)
                m_Vertices.push_back(vertex);
        }
        else if (strcmp(lineHeader, "vt") == 0)
        {
            if (sscanf(lineOptions, "%f %f\n", &uv.x, &uv.y) == 2)
                m_UVs.push_back(uv);
        }
        else if (strcmp(lineHeader, "vn") == 0)
        {
            if (sscanf(lineOptions, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z) == 3)
                m_Normals.push_back(vertex);
        }
        else if (strcmp(lineHeader, "f") == 0)
        {
            std::string vertex1, vertex2, vertex3;
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];

            int matches = sscanf(lineOptions, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
            if (matches != 9)
            {
                printf("Not triangulate face.\n");
                return NULL;
            }

            m_VertexIds.push_back(vertexIndex[0]);
            m_VertexIds.push_back(vertexIndex[1]);
            m_VertexIds.push_back(vertexIndex[2]);
            m_UVIndices.push_back(uvIndex[0]);
            m_UVIndices.push_back(uvIndex[1]);
            m_UVIndices.push_back(uvIndex[2]);
            m_NormalIndices.push_back(normalIndex[0]);
            m_NormalIndices.push_back(normalIndex[1]);
            m_NormalIndices.push_back(normalIndex[2]);
        }
    }

    fclose(in);
    return true;
}

