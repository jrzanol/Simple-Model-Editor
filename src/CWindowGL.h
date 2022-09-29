// by jrzanol
//

#pragma once

#include "CUtil.h"
#include "CCamera.h"
#include "CObject.h"

class CWindowGL : CCamera
{
public:
	CWindowGL();
	~CWindowGL();

	bool Initialize();
	void Cleanup();
	bool Render();

private:
	std::list<CDrawableObject*> m_DrawObject;
	
	GLuint CompileShader(const char*, GLenum);
	GLuint LinkProgram(GLuint, GLuint);
	GLuint m_ProgramId;

	static const char* g_VertexShader;
	static const char* g_FragmentShader;
};

