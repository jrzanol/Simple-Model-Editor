// by jrzanol
//

#pragma once

class CReadObj
{
public:
    bool Read(const char*);

    std::vector<unsigned int> m_VertexIds;
    std::vector<unsigned int> m_UVIndices;
    std::vector<unsigned int> m_NormalIndices;
    std::vector<glm::vec3> m_Vertices;
    std::vector<glm::vec2> m_UVs;
    std::vector<glm::vec3> m_Normals;
};

