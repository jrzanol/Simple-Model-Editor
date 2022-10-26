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

void CMesh::AllocBuffer()
{
    CalculateNormals();

    // Set Vertex buffer.
    glBindVertexArray(m_VAOId);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBOId);
    glBufferData(GL_ARRAY_BUFFER, m_Vertex.size() * sizeof(Vertex), &m_Vertex[0], GL_STATIC_DRAW);

    // Set Indices buffer.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBOId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), &m_Indices[0], GL_STATIC_DRAW);

    // Unbind the active buffer.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CMesh::CalculateNormals() {
    // https://stackoverflow.com/questions/30120636/calculating-vertex-normals-in-opengl-with-c

    // Clear each normal
    for (unsigned int i = 0; i < m_Vertex.size(); i++)
        m_Vertex[i].Normal = glm::vec3(0, 0, 0);

    // For each face calculate normals and append it
    // to the corresponding vertices of the face
    for (unsigned int i = 0; i < m_Indices.size(); i += 3) {
        glm::vec3 A = m_Vertex[m_Indices[i]].Position;
        glm::vec3 B = m_Vertex[m_Indices[i + 1LL]].Position;
        glm::vec3 C = m_Vertex[m_Indices[i + 2LL]].Position;

        auto computeFaceNormal = [](glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
            // Uses p2 as a new origin for p1,p3
            auto a = p3 - p2;
            auto b = p1 - p2;
            // Compute the cross product a X b to get the face normal
            return glm::normalize(glm::cross(a, b));
        };

        glm::vec3 normal = computeFaceNormal(A, B, C);

        m_Vertex[m_Indices[i]].Normal += normal;
        m_Vertex[m_Indices[i + 1LL]].Normal += normal;
        m_Vertex[m_Indices[i + 2LL]].Normal += normal;
    }

    // Normalize each normal
    for (unsigned int i = 0; i < m_Vertex.size(); i++)
        m_Vertex[i].Normal = glm::normalize(m_Vertex[i].Normal);
}

void CMesh::Draw(GLuint programId) const
{
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;

    unsigned int i = CUtil::m_TextureType;
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

