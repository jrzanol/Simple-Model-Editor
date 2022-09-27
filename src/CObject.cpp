// by jrzanol
//

#include "stdafx.h"
#include "CObject.h"

int CObject::g_ListCounter = 0;
CObject CObject::g_List[MAX_OBJECT];

CObject* CObject::LoadModel(GLuint programId, std::string file)
{
    for (auto& it : g_List)
    {
        if (it.m_ObjName == file)
        { // Load .obj file.
            return &it;
        }
    }

    if (g_ListCounter >= MAX_OBJECT)
        return NULL;

    CObject* obj = &g_List[g_ListCounter];
    obj->m_ObjName = file;
    obj->m_ProgramId = programId;

    // http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/#creating-an-obj-file-in-blender

    FILE* in = fopen(file.c_str(), "rt");
    if (in == NULL)
    {
        printf("Fail reading obj file.\n");
        return NULL;
    }

    std::vector<unsigned int> uvIndices, normalIndices;
    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec2> temp_uvs;
    std::vector<glm::vec3> temp_normals;

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
                temp_vertices.push_back(vertex);
        }
        else if (strcmp(lineHeader, "vt") == 0)
        {
            if (sscanf(lineOptions, "%f %f\n", &uv.x, &uv.y) == 2)
                temp_uvs.push_back(uv);
        }
        else if (strcmp(lineHeader, "vn") == 0)
        {
            if (sscanf(lineOptions, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z) == 3)
                temp_normals.push_back(vertex);
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

            obj->m_VerticesIndices.push_back(vertexIndex[0]);
            obj->m_VerticesIndices.push_back(vertexIndex[1]);
            obj->m_VerticesIndices.push_back(vertexIndex[2]);

            uvIndices.push_back(uvIndex[0]);
            uvIndices.push_back(uvIndex[1]);
            uvIndices.push_back(uvIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
        }
    }

    fclose(in);

    for (unsigned int i = 0; i < obj->m_VerticesIndices.size(); i++)
    {
        unsigned int vertexIndex = obj->m_VerticesIndices[i];
        glm::vec3 vertex = temp_vertices[vertexIndex - 1];

        obj->m_Vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < uvIndices.size(); i++)
    {
        unsigned int vertexIndex = uvIndices[i];
        glm::vec2 uv = temp_uvs[vertexIndex - 1];

        obj->m_UVs.push_back(uv);
    }

    for (unsigned int i = 0; i < normalIndices.size(); i++)
    {
        unsigned int vertexIndex = normalIndices[i];
        glm::vec3 normal = temp_normals[vertexIndex - 1];

        obj->m_Normals.push_back(normal);
    }

    // Generate VBO buffer.
    glGenBuffers(1, &obj->m_VBOId);
    glBindBuffer(GL_ARRAY_BUFFER, obj->m_VBOId);
    glBufferData(GL_ARRAY_BUFFER, obj->m_Vertices.size() * sizeof(glm::vec3), &obj->m_Vertices[0], GL_STATIC_DRAW);

    // Generate VAO buffer.
    glGenVertexArrays(1, &obj->m_VAOId);
    glBindVertexArray(obj->m_VAOId);
    glBindBuffer(GL_ARRAY_BUFFER, obj->m_VBOId);
    GLuint posAttributePosition = glGetAttribLocation(obj->m_ProgramId, "a_position");
    glVertexAttribPointer(posAttributePosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(posAttributePosition);

    // Allocate Normals buffer.
    //glGenBuffers(1, &obj->m_NormalBuffer);
    //glBindBuffer(GL_ARRAY_BUFFER, obj->m_NormalBuffer);
    //glBufferData(GL_ARRAY_BUFFER, obj->m_Normals.size() * sizeof(glm::vec3), &obj->m_Normals[0], GL_STATIC_DRAW);

    //// 3rd attribute buffer : normals
    //glEnableVertexAttribArray(2);
    //glBindBuffer(GL_ARRAY_BUFFER, obj->m_NormalBuffer);
    //glVertexAttribPointer(
    //    2,                                // attribute
    //    3,                                // size
    //    GL_FLOAT,                         // type
    //    GL_FALSE,                         // normalized?
    //    0,                                // stride
    //    (void*)0                          // array buffer offset
    //);

    // Unbind the active buffer.
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    g_ListCounter++;
    return obj;
}

CDrawableObject::CDrawableObject(GLuint programId, std::string file) : CObject(*LoadModel(programId, file))
{
}

CDrawableObject::~CDrawableObject()
{
}

void CDrawableObject::Draw() const
{
    glBindVertexArray(m_VAOId);
    glDrawArrays(GL_TRIANGLES, 0, m_VerticesIndices.size());
}

