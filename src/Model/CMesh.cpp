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

    // Unbind the active buffer.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
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

    // Set wireframe option.
    glUniform1i(glGetUniformLocation(programId, "u_wireframe"), 0);

    // Draw vertices.
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(m_Indices.size()), GL_UNSIGNED_INT, 0);

    // Set wireframe option.
    glUniform1i(glGetUniformLocation(programId, "u_wireframe"), 1);

    // Draw vertices.
    glDrawElements(GL_LINE_STRIP, static_cast<unsigned int>(m_Indices.size()), GL_UNSIGNED_INT, 0);

    // Unbind the active buffer.
    glBindVertexArray(0);
}

