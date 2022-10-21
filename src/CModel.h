// by jrzanol
//

#pragma once

#include "CMesh.h"

const int MAX_OBJECT = 128;

class CModel
{
public:
	CModel();

	void Draw(GLuint) const;

	std::string m_ObjName;
	std::vector<CMesh> m_Meshes;

	// Load File or Find Object.
	static CModel* LoadModel(std::string);

private:
	void ProcessModelNode(aiNode*, const aiScene*);
	CMesh ProcessModelMesh(aiMesh*, const aiScene*);

	static int g_ListCounter;
	static CModel g_List[MAX_OBJECT];
};

