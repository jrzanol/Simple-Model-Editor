// by jrzanol
//

#pragma once

const int MAX_OBJECT = 128;
typedef std::tuple<std::string, rapidobj::Result> ObjectData;

class CObject
{
public:
	// Load File or Find Object.
	ObjectData* LoadModel(std::string);

protected:
	GLuint m_VAOId;
	GLuint m_BufferInfo;

private:
	static int g_ListCounter;
	static ObjectData g_List[MAX_OBJECT];
};

class CDrawableObject : CObject
{
public:
	CDrawableObject();
	~CDrawableObject();
	
	void CreateBuffers();
	void Draw(GLuint);
};


