// by jrzanol
//

#pragma once

const int MAX_OBJECT = 128;

class CObject
{
public:
	// Load File or Find Object.
	static CObject* LoadModel(GLuint, std::string);

protected:
	GLuint m_VAOId;
	GLuint m_VBOId;
	GLuint m_UVBuffer;
	GLuint m_NormalBuffer;
	GLuint m_ProgramId;

	std::vector<unsigned int> m_VerticesIndices;
	std::vector<glm::vec3> m_Vertices;
	std::vector<glm::vec2> m_UVs;
	std::vector<glm::vec3> m_Normals;
	std::string m_ObjName;

private:
	static int g_ListCounter;
	static CObject g_List[MAX_OBJECT];
};

class CDrawableObject : CObject
{
public:
	CDrawableObject(GLuint, std::string);
	~CDrawableObject();
	
	void Draw() const;
};


