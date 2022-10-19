// by jrzanol
//

#pragma once

#include "CTexture.h"

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
};

class CMesh
{
public:
	CMesh(const std::vector<Vertex>&, const std::vector<unsigned int>&, const std::vector<Texture>&);

	void Draw(GLuint) const;

private:
	GLuint m_VBOId;
	GLuint m_EBOId;
	GLuint m_VAOId;
	GLuint m_BarycentricId;

	void CalculateBarycentric();

	std::vector<Vertex> m_Vertex;
	std::vector<Texture> m_Textures;
	std::vector<unsigned int> m_Indices;
	std::vector<glm::vec3> m_Barycentric;
};

