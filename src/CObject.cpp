// by jrzanol
//

#include "stdafx.h"

#include "CUtil.h"
#include "CObject.h"
#include "CReadObj.h"

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

    CReadObj ro;
    ro.Read(file.c_str());

    for (unsigned int i = 0; i < ro.m_VertexIds.size(); i++)
    {
    	Vertex vertex;
        vertex.Position = ro.m_Vertices[ro.m_VertexIds[i] - 1];
        vertex.Normal = ro.m_Normals[ro.m_NormalIndices[i] - 1];
        vertex.TexCoords = ro.m_UVs[ro.m_UVIndices[i] - 1];

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
    
    // Vertex Texture Coords.
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
    GLint modelPos = glGetUniformLocation(m_ProgramId, "u_model");

    glm::mat4 model(1.f);
    model = glm::translate(model, glm::vec3(CUtil::m_SliderInfo.m_X, CUtil::m_SliderInfo.m_Y, 0.f));
    model = glm::scale(model, glm::vec3(CUtil::m_SliderInfo.m_ScaleX, CUtil::m_SliderInfo.m_ScaleY, 1.f));
    model = glm::rotate(model, glm::radians((float)CUtil::m_SliderInfo.m_Angle), glm::vec3(0.0f, 0.0f, 1.0f));

    glUniformMatrix4fv(modelPos, 1, GL_FALSE, glm::value_ptr(model));
    
    // Draw vertices.
    glDrawArrays(GL_TRIANGLES, 0, m_Vertex.size());
    
    // Unbind the active buffer.
    glBindVertexArray(0);
}

