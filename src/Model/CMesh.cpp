// by jrzanol
//

#include "stdafx.h"
#include "CMesh.h"
#include "CUtil.h"

CMesh::CMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const std::vector<Texture>& textures)
{
    m_Vertex = vertices;
    m_Indices = indices;
    m_Textures = textures;

    CalculateBarycentric();

    // Set Barycentric buffer.
    glGenBuffers(1, &m_BarycentricId);
    glBindBuffer(GL_ARRAY_BUFFER, m_BarycentricId);
    glBufferData(GL_ARRAY_BUFFER, m_Barycentric.size() * sizeof(glm::vec3), &m_Barycentric[0], GL_STATIC_DRAW);

    // Generate Buffers.
    glGenVertexArrays(1, &m_VAOId);
    glGenBuffers(1, &m_VBOId);
    glGenBuffers(1, &m_EBOId);

    // Set Vertex buffer.
    glBindVertexArray(m_VAOId);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBOId);
    glBufferData(GL_ARRAY_BUFFER, m_Vertex.size() * sizeof(Vertex), &m_Vertex[0], GL_STATIC_DRAW);
    
    // Set Indices buffer.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBOId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), &m_Indices[0], GL_STATIC_DRAW);

    // Vertex Positions.
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    // Vertex Normals.
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

    // Vertex Texture Coords.
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    // Vertex Tangent.
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

    // Vertex Bitangent.
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
    
    // Barycentric.
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    // Unbind the active buffer.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CMesh::CalculateBarycentric()
{
    m_Barycentric.clear();

    const int n = m_Vertex.size() / 6;
    for (int i = 0; i < n; i++)
    {
        m_Barycentric.push_back(glm::vec3(1.f, 0.f, 0.f));
        m_Barycentric.push_back(glm::vec3(0.f, 1.f, 0.f));
        m_Barycentric.push_back(glm::vec3(0.f, 0.f, 1.f));
    }
}

void CMesh::Draw(GLuint programId) const
{
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;

    for (unsigned int i = 0; i < m_Textures.size(); i++)
    {
        // Ativa a Textura atual a ser renderizada.
        glActiveTexture(GL_TEXTURE0 + i);

        std::string number;
        std::string name = m_Textures[i].m_Type;

        if (name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if (name == "texture_specular")
            number = std::to_string(specularNr++);
        else if (name == "texture_normal")
            number = std::to_string(normalNr++);
        else if (name == "texture_height")
            number = std::to_string(heightNr++);

        // now set the sampler to the correct texture unit
        glUniform1i(glGetUniformLocation(programId, (name + number).c_str()), i);

        // and finally bind the texture
        glBindTexture(GL_TEXTURE_2D, m_Textures[i].m_Id);
    }

    // Bind buffer vector of object.
    glBindVertexArray(m_VAOId);

    // Transforms.
    GLint modelPos = glGetUniformLocation(programId, "u_model");

    glm::mat4 model(1.f);
    model = glm::translate(model, glm::vec3(CUtil::m_SliderInfo.m_X, CUtil::m_SliderInfo.m_Y, 0.f));
    model = glm::scale(model, glm::vec3(CUtil::m_SliderInfo.m_ScaleX, CUtil::m_SliderInfo.m_ScaleY, 1.f));
    model = glm::rotate(model, glm::radians((float)CUtil::m_SliderInfo.m_Angle), glm::vec3(0.0f, 0.0f, 1.0f));

    glUniformMatrix4fv(modelPos, 1, GL_FALSE, glm::value_ptr(model));

    // Draw vertices.
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(m_Indices.size()), GL_UNSIGNED_INT, 0);

    // Unbind the active buffer.
    glBindVertexArray(0);
}

