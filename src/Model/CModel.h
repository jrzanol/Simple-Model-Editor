// by jrzanol
//

#pragma once

#include "CMesh.h"

const int MAX_OBJECT = 128;

class CModel
{
public:
	CModel();

	void Draw(GLuint, const glm::mat4&) const;
	glm::mat4& GetModelPos() const;

	std::string m_ObjName;
	std::vector<CMesh> m_Meshes;
	glm::vec3 m_Position;
	glm::vec3 m_Scale;
	float m_Angle;

	float m_TextCoord;
	int m_SelectedTexture;

	// Load File or Find Object.
	static CModel* LoadModel(std::string);

	static CModel* g_SelectedModel;

private:
	void ProcessModelNode(aiNode*, const aiScene*);
	CMesh ProcessModelMesh(aiMesh*, const aiScene*);

	static int g_ListCounter;
	static CModel g_List[MAX_OBJECT];
};

