// by jrzanol
//

#pragma once

const int MAX_OBJECT = 128;

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

class CObject
{
public:
	// Load File or Find Object.
	static CObject* LoadModel(std::string);

protected:
	GLuint m_VAOId;
	GLuint m_VBOId;
	//GLuint m_EBOId;
	
	std::vector<Vertex> m_Vertex;
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
	
private:
	GLuint m_ProgramId;
};


