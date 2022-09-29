// by jrzanol
//

#include "stdafx.h"

#include "CUtil.h"
#include "CObject.h"

int CObject::g_ListCounter = 0;
CObject CObject::g_List[MAX_OBJECT];

CObject* CObject::LoadModel(std::string file)
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

    // http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/#creating-an-obj-file-in-blender

    FILE* in = fopen(file.c_str(), "rt");
    if (in == NULL)
    {
        printf("Fail reading obj file.\n");
        return NULL;
    }

    std::vector<unsigned int> vertexIds, uvIndices, normalIndices;
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

            vertexIds.push_back(vertexIndex[0]);
            vertexIds.push_back(vertexIndex[1]);
            vertexIds.push_back(vertexIndex[2]);
            uvIndices.push_back(uvIndex[0]);
            uvIndices.push_back(uvIndex[1]);
            uvIndices.push_back(uvIndex[2]);
            normalIndices.push_back(normalIndex[0]);
            normalIndices.push_back(normalIndex[1]);
            normalIndices.push_back(normalIndex[2]);
        }
    }

    fclose(in);

    for (unsigned int i = 0; i < vertexIds.size(); i++)
    {
    	Vertex vertex;
        vertex.Position = temp_vertices[vertexIds[i] - 1];
        vertex.Normal = temp_normals[normalIndices[i] - 1];
        vertex.TexCoords = temp_uvs[uvIndices[i] - 1];

        obj->m_Vertex.push_back(vertex);
    }

    // Generate Buffers.
    glGenVertexArrays(1, &obj->m_VAOId);
    glGenBuffers(1, &obj->m_VBOId);
    //glGenBuffers(1, &obj->m_EBOId);
    
    // Set Vertex buffer.
    glBindVertexArray(obj->m_VAOId);
    glBindBuffer(GL_ARRAY_BUFFER, obj->m_VBOId);
    glBufferData(GL_ARRAY_BUFFER, obj->m_Vertex.size() * sizeof(Vertex), &obj->m_Vertex[0], GL_STATIC_DRAW);

    // Set Indices buffer.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->m_EBOId);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj->m_VertexIds.size() * sizeof(unsigned int), &obj->m_VertexIds[0], GL_STATIC_DRAW);
                 
    // Vertex Positions.
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    
    // Vertex Normals.
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    
    // Vertex Texture Coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    // Unbind the active buffer.
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    g_ListCounter++;
    return obj;
}

CDrawableObject::CDrawableObject(GLuint programId, std::string file) : CObject(*LoadModel(file))
{
	m_ProgramId = programId;
}

CDrawableObject::~CDrawableObject()
{
}

void CDrawableObject::Draw() const
{
    // Bind buffer vector of object.
    glBindVertexArray(m_VAOId);
    
    // Transforms.
    GLint projPos = glGetUniformLocation(m_ProgramId, "u_proj");
    GLint modelPos = glGetUniformLocation(m_ProgramId, "u_model");

    glm::mat4 projection = glm::perspective(glm::radians(90.f), (float)CUtil::g_MaxY / (float)CUtil::g_MaxX, 0.1f, 100.0f);
    glm::mat4 model(1.f);
    model = glm::translate(model, glm::vec3(CUtil::g_SliderInfo.m_X, CUtil::g_SliderInfo.m_Y, 0.f));
    model = glm::scale(model, glm::vec3(CUtil::g_SliderInfo.m_ScaleX, CUtil::g_SliderInfo.m_ScaleY, 1.f));
    model = glm::rotate(model, glm::radians((float)CUtil::g_SliderInfo.m_Angle), glm::vec3(0.0f, 0.0f, 1.0f));

    glUniformMatrix4fv(projPos, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(modelPos, 1, GL_FALSE, glm::value_ptr(model));
    
    // Draw vertices.
    glDrawArrays(GL_TRIANGLES, 0, m_Vertex.size());
    
    // Unbind the active buffer.
    glBindVertexArray(0);
}

